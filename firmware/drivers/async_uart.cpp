#include "async_uart.hpp"

#include "error.hpp"
#include "gpio.hpp"
#include "interrupts.hpp"
#include "stm32l552xx.h"
#include "system.hpp"

#include <cstdint>

enum UartInstance {
	UART_USART1,
	UART_USART2,
	UART_INSTANCE_COUNT
};

/**
 * @brief Instance storage for UARTs.
 *        This array is used to dispatch interrupts to the correct instance.
 */
static AsyncUart* _uartInstances[UART_INSTANCE_COUNT] = {nullptr, nullptr};

static void _USART1_IRQHandler(void) {
	if (_uartInstances[UART_USART1] != nullptr) {
		_uartInstances[UART_USART1]->_IrqHandler(USART1->ISR);
	}
}

static void _USART2_IRQHandler(void) {
	if (_uartInstances[UART_USART2] != nullptr) {
		_uartInstances[UART_USART2]->_IrqHandler(USART2->ISR);
	}
}

static void DMA1_Channel1_IRQHandler(void) {
	if (DMA1->ISR & DMA_ISR_HTIF1) {
		DMA1->IFCR |= DMA_IFCR_CHTIF1;
		if (_uartInstances[UART_USART1] != nullptr) {
			_uartInstances[UART_USART1]->_RxHandler();
		}
	}
	if (DMA1->ISR & DMA_ISR_TCIF1) {
		DMA1->IFCR |= DMA_IFCR_CTCIF1;
		if (_uartInstances[UART_USART1] != nullptr) {
			_uartInstances[UART_USART1]->_RxHandler();
		}
	}
}

static void DMA1_Channel2_IRQHandler(void) {
	if (DMA1->ISR & DMA_ISR_HTIF2) {
		DMA1->IFCR |= DMA_IFCR_CHTIF2;
		if (_uartInstances[UART_USART2] != nullptr) {
			_uartInstances[UART_USART2]->_RxHandler();
		}
	}
	if (DMA1->ISR & DMA_ISR_TCIF2) {
		DMA1->IFCR |= DMA_IFCR_CTCIF2;
		if (_uartInstances[UART_USART2] != nullptr) {
			_uartInstances[UART_USART2]->_RxHandler();
		}
	}
}

AsyncUart::
	AsyncUart(USART_TypeDef* uart, GPIO_TypeDef* txPort, uint32_t txPin, GPIO_TypeDef* rxPort, uint32_t rxPin, AsyncUartInterface* interface):
_txPin(
	txPort,
	txPin,
	Gpio::Mode::ALTERNATE_FUNCTION,
	Gpio::Type::PUSH_PULL,
	Gpio::PullUp::PULL_UP,
	Gpio::Speed::LOW
),
_rxPin(
	rxPort,
	rxPin,
	Gpio::Mode::ALTERNATE_FUNCTION,
	Gpio::Type::PUSH_PULL,
	Gpio::PullUp::PULL_UP,
	Gpio::Speed::LOW
),
_uart(uart),
_interface(interface) {
	uint32_t dmaMuxRx = 0UL;
	RCC->AHB1ENR |= RCC_AHB1ENR_DMA1EN;
	if (_uart == USART1) {
		RCC->APB2ENR |= RCC_APB2ENR_USART1EN;
		// Set to use SYSCLK as basis for the clock
		RCC->CCIPR1 &= ~RCC_CCIPR1_USART1SEL;
		RCC->CCIPR1 |= RCC_CCIPR1_USART1SEL_0;
		_uartInstances[UART_USART1] = this;
		RegisterIrqHandler(USART1_IRQn, _USART1_IRQHandler);
		// DMA: 25 -> RX, 26 -> TX
		dmaMuxRx    = 25UL;
		_dmaChannel = DMA1_Channel1;
		_dmaMux     = DMAMUX1_Channel1;
		RegisterIrqHandler(DMA1_Channel1_IRQn, DMA1_Channel1_IRQHandler);
	} else if (_uart == USART2) {
		RCC->APB1ENR1 |= RCC_APB1ENR1_USART2EN;
		// Set to use SYSCLK as basis for the clock
		RCC->CCIPR1 &= ~RCC_CCIPR1_USART2SEL;
		RCC->CCIPR1 |= RCC_CCIPR1_USART2SEL_0;
		_uartInstances[UART_USART2] = this;
		RegisterIrqHandler(USART2_IRQn, _USART2_IRQHandler);
		// DMA: 27 -> RX, 28 -> TX
		dmaMuxRx    = 27UL;
		_dmaChannel = DMA1_Channel2;
		_dmaMux     = DMAMUX1_Channel2;
		RegisterIrqHandler(DMA1_Channel2_IRQn, DMA1_Channel2_IRQHandler);
	} else {
		PANIC("Unsupported UART");
	}

	_dmaChannel->CCR = 0UL;
	_dmaChannel->CCR |=
		(DMA_CCR_TCIE | DMA_CCR_HTIE | DMA_CCR_CIRC | DMA_CCR_MINC
	     | DMA_CCR_PL_1);
	_dmaMux->CCR = 0UL;
	_dmaMux->CCR |= dmaMuxRx & DMAMUX_CxCR_DMAREQ_ID;
	_dmaChannel->CNDTR = _RX_BUFFER_SIZE;
	_dmaChannel->CPAR  = reinterpret_cast<uint32_t>(&(_uart->RDR));
	_dmaChannel->CM0AR = reinterpret_cast<uint32_t>(_rxBuffer);

	constexpr uint32_t brr =
		static_cast<uint32_t>(static_cast<float>(SYSCLK_HZ) / 115'200UL)
		& USART_BRR_BRR;

	_txPin.SetAlternateFunction(Gpio::AlternateFunction::AF7);
	_rxPin.SetAlternateFunction(Gpio::AlternateFunction::AF7);

	_uart->CR1 = 0UL;
	_uart->CR1 |=
		(USART_CR1_TE | USART_CR1_RE | USART_CR1_IDLEIE | USART_CR1_TCIE);
	_uart->CR2   = 0UL;
	_uart->CR3   = 0UL;
	_uart->PRESC = 0UL;
	_uart->BRR   = 0UL;
	_uart->BRR |= brr;

	_uart->CR1 |= USART_CR1_UE;
}

AsyncUart::~AsyncUart() {
	_DisableIrq();
	_dmaMux->CCR       = 0UL;
	_dmaChannel->CCR   = 0UL;
	_dmaChannel->CNDTR = 0UL;
	_dmaChannel->CPAR  = 0UL;
	_dmaChannel->CM0AR = 0UL;
	_uart->CR1 &= ~USART_CR1_UE;
	if (_uart == USART1) {
		RCC->APB1RSTR1 |= RCC_APB2RSTR_USART1RST;
		RCC->APB2ENR &= ~RCC_APB2ENR_USART1EN;
		_uartInstances[UART_USART1] = nullptr;
		DeregisterIrqHandler(USART1_IRQn);
		DeregisterIrqHandler(DMA1_Channel1_IRQn);
	} else if (_uart == USART2) {
		RCC->APB1RSTR1 |= RCC_APB1RSTR1_USART2RST;
		RCC->APB1ENR2 &= ~RCC_APB1ENR1_USART2EN;
		_uartInstances[UART_USART2] = nullptr;
		DeregisterIrqHandler(USART2_IRQn);
		DeregisterIrqHandler(DMA1_Channel2_IRQn);
	} else {
		PANIC("Unsupported UART");
	}
	// NOTE: GPIO deinitialization is done in the Gpio destructor.
}

error_code_t AsyncUart::Transmit(uint8_t* data, uint32_t size) {
	if (data == nullptr) {
		return -ERROR_INVALID_ARGUMENT;
	}
	if (_txRemaining != 0UL) {
		return -ERROR_BUSY;
	}
	_DisableIrq();
	_txRemaining = size - 1UL;
	_txData      = data + 1UL;
	_txStarted   = true;
	_uart->TDR   = *data;
	_EnableIrq();
	return ERROR_NONE;
}

error_code_t AsyncUart::Start() {
	_DisableIrq();
	_rxStarted = true;
	_dmaChannel->CCR |= DMA_CCR_EN;
	_EnableIrq();
	return ERROR_NONE;
}

void AsyncUart::_RxHandler() {
	static uint32_t oldHead       = UINT32_MAX;
	uint32_t        dataRemaining = DMA1_Channel1->CNDTR & DMA_CNDTR_NDT;
	uint32_t        head          = _RX_BUFFER_SIZE - dataRemaining;
	if (head != oldHead) {
		if (head > oldHead) {
			_interface->RxCallback(&_rxBuffer[oldHead], head - oldHead);
		} else {
			_interface->RxCallback(
				&_rxBuffer[oldHead], _RX_BUFFER_SIZE - oldHead
			);
			if (head > 0) {
				_interface->RxCallback(&_rxBuffer[0], head);
			}
		}
	}
	oldHead = head;
}

void AsyncUart::_IrqHandler(uint32_t isr) {
	uint32_t volatile errors =
		isr & (USART_ISR_PE | USART_ISR_FE | USART_ISR_NE | USART_ISR_ORE);
	if (errors) {
		if (_txStarted) {
			_txStarted = false;
			_txErrors++;
			_interface->TxErrorCallback(nullptr, 0UL);
		}
		if (_rxStarted) {
			_rxStarted = false;
			_rxErrors++;
			_interface->RxErrorCallback(nullptr, 0UL);
		}
		_uart->ICR |= errors;
	}
	if (isr & USART_ISR_TC) {
		if (_txStarted) {
			if (_txRemaining == 0UL) {
				_interface->TxCallback(_txData - _txSent, _txSent);
				_txStarted = false;
				_txData    = nullptr;
				_txSent    = 0UL;
			} else {
				_txData++;
				_txRemaining--;
				_txSent++;
				_uart->TDR = *_txData;
			}
		}
		// Clear the interrupt
		_uart->ICR |= USART_ICR_TCCF;
	}
	if (isr & USART_ISR_IDLE) {
		_uart->ICR |= USART_ICR_IDLECF;
		if (_rxStarted) {
			_RxHandler();
		}
	}
}
