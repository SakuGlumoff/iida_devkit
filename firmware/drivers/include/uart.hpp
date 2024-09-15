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

	Uart(
		USART_TypeDef* uart,
		GPIO_TypeDef*  txPort,
		uint32_t       txPin,
		GPIO_TypeDef*  rxPort,
		uint32_t       rxPin
	);
	~Uart();

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
	 * @brief Transmit data over the UART in a blocking mode.
	 *
	 * @param[in] data    Data to transmit.
	 * @param[in] size    Size of the data to transmit.
	 * @param[in] timeout Timeout in ticks.
	 *
	 * @return error_code_t ERROR_NONE if successful. Otherwise, an error
	 *                      code of Uart::Error.
	 */
	error_code_t Puts(uint8_t* data, uint32_t size, TickType timeout = 0UL);

	/**
	 * @brief Receive data over the UART in a blocking mode.
	 *
	 * @param[out] data    Data received.
	 * @param[in]  size    Size of the data to receive.
	 * @param[in]  timeout Timeout in ticks.
	 *
	 * @return error_code_t ERROR_NONE if successful. Otherwise, an error
	 *                      code of Uart::Error.
	 */
	error_code_t Gets(uint8_t* data, uint32_t size, TickType timeout = 0UL);

private:
	Gpio           _txPin;
	Gpio           _rxPin;
	USART_TypeDef* _uart           = nullptr;
	uint8_t*       _txData         = nullptr;
	uint8_t*       _rxData         = nullptr;
	bool volatile _txStarted       = false;
	bool volatile _rxStarted       = false;
	uint32_t volatile _txSent      = 0UL;
	uint32_t volatile _txRemaining = 0UL;
	uint32_t volatile _rxReceived  = 0UL;
	uint32_t volatile _rxRemaining = 0UL;

	void _DisableIrq() {
		if (_uart == USART1) {
			__NVIC_DisableIRQ(USART1_IRQn);
		} else if (_uart == USART2) {
			__NVIC_DisableIRQ(USART2_IRQn);
		}
	}

	void _EnableIrq() {
		if (_uart == USART1) {
			__NVIC_EnableIRQ(USART1_IRQn);
		} else if (_uart == USART2) {
			__NVIC_EnableIRQ(USART2_IRQn);
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
	void _IrqHandler();
};
