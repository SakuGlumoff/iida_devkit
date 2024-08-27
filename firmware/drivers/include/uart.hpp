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

	/**
	 * @brief Register a callback for the UART.
	 *
	 * @param[in] type     Type of callback.
	 * @param[in] callback Callback function.
	 *
	 * @return error_code_t ERROR_NONE if successful. Otherwise, an error
	 *                      code of Uart::Error.
	 */
	error_code_t RegisterCallback(Callbacks type, Callback callback);

	/**
	 * @brief Get a character from the UART.
	 *
	 * @param[out] c       Character received.
	 * @param[in]  timeout Timeout in ticks.
	 *
	 * @return error_code_t ERROR_NONE if succesful. Otherwise, an error
	 *                      code of Uart::Error.
	 */
	error_code_t Getc(uint8_t& c, TickType timeout = 0UL);

	/**
	 * @brief Put a character to the UART.
	 *
	 * @param[in] c       Character to send.
	 * @param[in] timeout Timeout in ticks.
	 *
	 * @return error_code_t ERROR_NONE if successful. Otherwise, an error
	 *                      code of Uart::Error.
	 */
	error_code_t Putc(uint8_t c, TickType timeout = 0UL);

	/**
	 * @brief Transmit data over the UART.
	 *
	 * @param[in] data Data to transmit.
	 * @param[in] size Size of the data to transmit.
	 *
	 * @return error_code_t ERROR_NONE if successful. Otherwise, an error
	 *                      code of Uart::Error.
	 */
	error_code_t Transmit(uint8_t* data, uint32_t size);

	/**
	 * @brief Receive data over the UART.
	 *
	 * @param[in,out] data Data received.
	 * @param[in]  size Size of the data to receive.
	 *
	 * @return error_code_t ERROR_NONE if successful. Otherwise, an error
	 *                      code of Uart::Error.
	 */
	error_code_t Receive(uint8_t* data, uint32_t size);

private:
	Callback       _txCallback  = nullptr;
	Callback       _rxCallback  = nullptr;
	uint32_t       _txSent      = 0UL;
	uint32_t       _txRemaining = 0UL;
	uint32_t       _rxReceived  = 0UL;
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
	/**
	 * @brief IRQ handler for the UART instance.
	 *
	 * @note DO NOT CALL THIS YOURSELF! This is called by the IRQ handler
	 *       and has to be a public method to be callable by the IRQ
	 *       handler.
	 */
	void _IrqHandler(uint32_t isr);
};
