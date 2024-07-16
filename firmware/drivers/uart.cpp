#include "uart.hpp"

#include "error.hpp"
#include "gpio.hpp"
#include "stm32l552xx.h"
#include "system.hpp"

#include <cstdint>

enum UartInstance {
	UART_USART1,
	UART_LPUART1,
	UART_INSTANCE_COUNT
};

/**
 * @brief Instance storage for UART interrupts.
 *        This array is used to dispatch interrupts to the correct instance.
 */
static Uart* _uartInstances[UART_INSTANCE_COUNT] = {nullptr, nullptr};

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
	uint32_t brr = 0UL;
	if (_uart == USART1) {
		constexpr uint32_t brrValue = static_cast<uint32_t>(
		    static_cast<float>(SYSCLK_HZ) / 115'200UL
		);
		brr = (brrValue & USART_BRR_BRR);
		RCC->APB2ENR |= RCC_APB2ENR_USART1EN;
		// Set to use SYSCLK as basis for the clock
		RCC->CCIPR1 &= ~RCC_CCIPR1_USART1SEL;
		RCC->CCIPR1 |= RCC_CCIPR1_USART1SEL_0;
		_txPin.SetAlternateFunction(Gpio::AlternateFunction::AF7);
		_rxPin.SetAlternateFunction(Gpio::AlternateFunction::AF7);
		_uartInstances[UART_USART1] = this;
	} else if (_uart == LPUART1) {
		constexpr uint32_t brrValue = static_cast<uint32_t>(
		    static_cast<float>(SYSCLK_HZ) / 115'200UL * 256UL
		);
		brr = (brrValue & USART_BRR_LPUART);
		RCC->APB1ENR2 |= RCC_APB1ENR2_LPUART1EN;
		// Set to use SYSCLK as basis for the clock
		RCC->CCIPR1 &= ~RCC_CCIPR1_LPUART1SEL;
		RCC->CCIPR1 |= RCC_CCIPR1_LPUART1SEL_0;
		_txPin.SetAlternateFunction(Gpio::AlternateFunction::AF8);
		_rxPin.SetAlternateFunction(Gpio::AlternateFunction::AF8);
		_uartInstances[UART_LPUART1] = this;
	} else {
		PANIC("Unsupported UART");
	}

	_uart->CR1 = 0UL;
	_uart->CR1 |=
	    (USART_CR1_TE | USART_CR1_RE | USART_CR1_RXNEIE_RXFNEIE
	     | USART_CR1_TCIE);
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
	} else if (_uart == LPUART1) {
		RCC->APB1ENR2 &= ~RCC_APB1ENR2_LPUART1EN;
		_uartInstances[UART_LPUART1] = nullptr;
	} else {
		PANIC("Unsupported UART");
	}
	// NOTE: GPIO deinitialization is done in the Gpio destructor.
}

error_code_t
Uart::RegisterCallback(Uart::Callbacks type, Uart::Callback callback) {
	switch (type) {
		case Uart::Callbacks::TX:
			_txCallback = callback;
			break;
		case Uart::Callbacks::RX:
			_rxCallback = callback;
			break;
		default:
			return -ERROR_INVALID_ARGUMENT;
	}
	return ERROR_NONE;
}

error_code_t Uart::Getc(uint8_t& c, TickType timeout) {
	if (_rxRemaining != 0UL) {
		return -ERROR_BUSY;
	}
	_DisableIrq();
	TickType timeoutEnd = GetTicks() + timeout;
	while (!(_uart->ISR & USART_ISR_RXNE)) {
		if (GetTicks() >= timeoutEnd) {
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
	_uart->ICR |= USART_ICR_TCCF;
	_uart->TDR          = c;
	TickType timeoutEnd = GetTicks() + timeout;
	while (!(_uart->ISR & USART_ISR_TC)) {
		if (GetTicks() >= timeoutEnd) {
			_EnableIrq();
			return -ERROR_TIMEOUT;
		}
		__NOP();
	}
	_uart->ICR |= USART_ICR_TCCF;
	_EnableIrq();
	return ERROR_NONE;
}

error_code_t Uart::Transmit(uint8_t* data, uint32_t size) {
	if (data == nullptr) {
		return -ERROR_INVALID_ARGUMENT;
	}
	if (_txRemaining != 0UL) {
		return -ERROR_BUSY;
	}
	_txRemaining = size - 1UL;
	_txData      = data + 1UL;
	_DisableIrq();
	_uart->CR1 |= USART_CR1_TXEIE;
	_txStarted = true;
	_uart->TDR = *data;
	_EnableIrq();
	return ERROR_NONE;
}

error_code_t Uart::Receive(uint8_t* data, uint32_t size) {
	if (data == nullptr) {
		return -ERROR_INVALID_ARGUMENT;
	}
	if (_rxRemaining != 0UL) {
		return -ERROR_BUSY;
	}
	_DisableIrq();
	_uart->CR1 |= USART_CR1_RXNEIE;
	_rxReceived  = 0UL;
	_rxRemaining = size;
	_rxData      = data;
	_rxStarted   = true;
	_EnableIrq();
	return ERROR_NONE;
}

void Uart::_IrqHandler(uint32_t isr) {
	if (isr & USART_ISR_TC) {
		if (_txStarted) {
			if (_txRemaining == 0UL) {
				if (_txCallback != nullptr) {
					_txCallback(_txData - _txSent, _txSent);
				}
				_txStarted = false;
				_txData    = nullptr;
				_txSent    = 0UL;
			} else {
				_uart->TDR = *_txData;
				_txData++;
				_txRemaining--;
				_txSent++;
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
				if (_rxCallback != nullptr) {
					_rxCallback(
					    _rxData - _rxReceived,
					    _rxReceived
					);
				}
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

extern "C" {
	void USART1_IRQHandler(void) {
		if (_uartInstances[UART_USART1] != nullptr) {
			_uartInstances[UART_USART1]->_IrqHandler(USART1->ISR);
		}
	}

	void LPUART1_IRQHandler(void) {
		if (_uartInstances[UART_LPUART1] != nullptr) {
			_uartInstances[UART_LPUART1]->_IrqHandler(LPUART1->ISR);
		}
	}
}
