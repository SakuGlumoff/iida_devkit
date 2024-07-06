#pragma once

#include "errno.hpp"
#include "gpio.hpp"
#include "stm32l552xx.h"
#include "system.hpp"

#include <cstdint>

class Uart {
	public:
	enum Error {
		ERROR_BUSY = 1,
		ERROR_TIMEOUT,
		ERROR_INVALID_ARGUMENT,
		ERROR_COUNT
	};

	enum class Callbacks {
		TX,
		RX
	};

	using IrqHandler = void (Uart::*)(uint32_t);
	using Callback   = void (*)(uint8_t*, uint32_t);

	Uart(
	    USART_TypeDef* uart,
	    GPIO_TypeDef*  txPort,
	    uint32_t       txPin,
	    GPIO_TypeDef*  rxPort,
	    uint32_t       rxPin
	);
	~Uart();

	error_code_t RegisterCallback(Callbacks type, Callback callback);

	error_code_t Getc(uint8_t& c, TickType timeout = 0UL);
	error_code_t Putc(uint8_t c, TickType timeout = 0UL);

	error_code_t Transmit(uint8_t* data, uint32_t size);
	error_code_t Receive(uint8_t* data, uint32_t size);

	private:
	Callback       _txCallback  = nullptr;
	Callback       _rxCallback  = nullptr;
	uint32_t       _txRemaining = 0UL;
	uint32_t       _rxRemaining = 0UL;
	uint8_t*       _txData      = nullptr;
	uint8_t*       _rxData      = nullptr;
	bool           _txStarted   = false;
	bool           _rxStarted   = false;
	Gpio           _txPin;
	Gpio           _rxPin;
	USART_TypeDef* _uart;

	void _DisableIrq() {
		if (_uart == USART1) {
			__NVIC_DisableIRQ(USART1_IRQn);
		} else if (_uart == LPUART1) {
			__NVIC_DisableIRQ(LPUART1_IRQn);
		}
	}

	void _EnableIrq() {
		if (_uart == USART1) {
			__NVIC_EnableIRQ(USART1_IRQn);
		} else if (_uart == LPUART1) {
			__NVIC_EnableIRQ(LPUART1_IRQn);
		}
	}

	public:
	void _IrqHandler(uint32_t isr);
};
