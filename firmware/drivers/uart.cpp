#include "uart.hpp"

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
static Uart* _uartInstances[UART_INSTANCE_COUNT] = {nullptr, nullptr};

static void _USART1_IRQHandler(void) {
	if (_uartInstances[UART_USART1] != nullptr) {
		_uartInstances[UART_USART1]->_IrqHandler();
	}
}

static void _USART2_IRQHandler(void) {
	if (_uartInstances[UART_USART2] != nullptr) {
		_uartInstances[UART_USART2]->_IrqHandler();
	}
}

Uart::Uart(
	USART_TypeDef* uart,
	GPIO_TypeDef*  txPort,
	uint32_t       txPin,
	GPIO_TypeDef*  rxPort,
	uint32_t       rxPin
):
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
_uart(uart) {
	if (_uart == USART1) {
		RCC->APB2ENR |= RCC_APB2ENR_USART1EN;
		// Set to use SYSCLK as basis for the clock
		RCC->CCIPR1 &= ~RCC_CCIPR1_USART1SEL;
		RCC->CCIPR1 |= RCC_CCIPR1_USART1SEL_0;
		_uartInstances[UART_USART1] = this;
		RegisterIrqHandler(USART1_IRQn, _USART1_IRQHandler);
	} else if (_uart == USART2) {
		RCC->APB1ENR1 |= RCC_APB1ENR1_USART2EN;
		// Set to use SYSCLK as basis for the clock
		RCC->CCIPR1 &= ~RCC_CCIPR1_USART2SEL;
		RCC->CCIPR1 |= RCC_CCIPR1_USART2SEL_0;
		_uartInstances[UART_USART2] = this;
		RegisterIrqHandler(USART2_IRQn, _USART2_IRQHandler);
	} else {
		PANIC("Unsupported UART");
	}

	constexpr uint32_t brr =
		static_cast<uint32_t>(static_cast<float>(SYSCLK_HZ) / 115'200UL)
		& USART_BRR_BRR;

	_txPin.SetAlternateFunction(Gpio::AlternateFunction::AF7);
	_rxPin.SetAlternateFunction(Gpio::AlternateFunction::AF7);

	_uart->CR1 = 0UL;
	_uart->CR1 |=
		(USART_CR1_TE | USART_CR1_RE | USART_CR1_RXNEIE_RXFNEIE | USART_CR1_TCIE
	    );
	_uart->CR2   = 0UL;
	_uart->CR3   = 0UL;
	_uart->PRESC = 0UL;
	_uart->BRR   = 0UL;
	_uart->BRR |= brr;

	_uart->CR1 |= USART_CR1_UE;
}

Uart::~Uart() {
	_DisableIrq();
	_uart->CR1 &= ~USART_CR1_UE;
	_uart->BRR = 0UL;
	_uart->CR3 = 0UL;
	_uart->CR2 = 0UL;
	_uart->CR1 = 0UL;
	if (_uart == USART1) {
		RCC->APB2ENR &= ~RCC_APB2ENR_USART1EN;
		_uartInstances[UART_USART1] = nullptr;
		DeregisterIrqHandler(USART1_IRQn);
	} else if (_uart == USART2) {
		RCC->APB1ENR2 &= ~RCC_APB1ENR1_USART2EN;
		_uartInstances[UART_USART2] = nullptr;
		DeregisterIrqHandler(USART2_IRQn);
	} else {
		PANIC("Unsupported UART");
	}
	// NOTE: GPIO deinitialization is done in the Gpio destructor.
}

error_code_t Uart::Getc(uint8_t& c, TickType timeout) {
	if (_rxRemaining != 0UL) {
		return -ERROR_BUSY;
	}
	_DisableIrq();
	TickType timeoutStart = GetTicks();
	while (!(_uart->ISR & USART_ISR_RXNE)) {
		if (timeout && (GetTicks() >= timeoutStart + timeout)) {
			_rxRemaining = 0UL;
			_EnableIrq();
			return -ERROR_TIMEOUT;
		}
		__NOP();
	}
	c = _uart->RDR;
	_EnableIrq();
	return ERROR_NONE;
}

error_code_t Uart::Putc(uint8_t c, TickType timeout) {
	if (_txRemaining != 0UL) {
		return -ERROR_BUSY;
	}
	_DisableIrq();
	_uart->TDR            = c;
	TickType timeoutStart = GetTicks();
	_uart->ICR |= USART_ICR_TCCF;
	while (!(_uart->ISR & USART_ISR_TC)) {
		if (timeout && (GetTicks() >= timeoutStart + timeout)) {
			_txRemaining = 0UL;
			_EnableIrq();
			return -ERROR_TIMEOUT;
		}
		__NOP();
	}
	_uart->ICR |= USART_ICR_TCCF;
	_EnableIrq();
	return ERROR_NONE;
}

error_code_t Uart::Puts(uint8_t* data, uint32_t size, TickType timeout) {
	if (data == nullptr) {
		return -ERROR_INVALID_ARGUMENT;
	}
	if (_txRemaining != 0UL) {
		return -ERROR_BUSY;
	}
	_DisableIrq();
	TickType timeoutStart = GetTicks();
	_txRemaining          = size - 1UL;
	_txData               = data + 1UL;
	_txStarted            = true;
	_uart->TDR            = *data;
	_uart->CR1 |= USART_CR1_TXEIE;
	_EnableIrq();
	while (_txStarted) {
		if (timeout && (GetTicks() >= timeoutStart + timeout)) {
			_txStarted   = false;
			_txRemaining = 0UL;
			return -ERROR_TIMEOUT;
		}
		__NOP();
	}
	return ERROR_NONE;
}

error_code_t Uart::Gets(uint8_t* data, uint32_t size, TickType timeout) {
	if (data == nullptr) {
		return -ERROR_INVALID_ARGUMENT;
	}
	if (_rxRemaining != 0UL) {
		return -ERROR_BUSY;
	}
	_DisableIrq();
	TickType timeoutStart = GetTicks();
	_rxReceived           = 0UL;
	_rxRemaining          = size;
	_rxData               = data;
	_rxStarted            = true;
	_uart->CR1 |= USART_CR1_RXNEIE;
	_EnableIrq();
	while (_rxStarted) {
		if (timeout && (GetTicks() >= timeoutStart + timeout)) {
			_rxStarted   = false;
			_rxRemaining = 0UL;
			return -ERROR_TIMEOUT;
		}
		__NOP();
	}
	return ERROR_NONE;
}

void Uart::_IrqHandler() {
	uint32_t volatile isr = _uart->ISR;
	if (isr & USART_ISR_TC) {
		if (_txStarted) {
			if (_txRemaining == 0UL) {
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
	if (isr & USART_ISR_RXNE_RXFNE) {
		if (_rxStarted) {
			*_rxData = _uart->RDR;
			_rxData++;
			_rxRemaining--;
			_rxReceived++;
			if (_rxRemaining == 0UL) {
				_rxStarted  = false;
				_rxData     = nullptr;
				_rxReceived = 0UL;
			}
		} else {
			// Clear the interrupt
			uint32_t volatile rdr = _uart->RDR;
			(void)rdr;
		}
	}
}
