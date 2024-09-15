#pragma once

#include "async_uart.hpp"
#include "errno.hpp"
#include "gpio.hpp"
#include "stm32l552xx.h"

#include <cstdint>

class Modem: public AsyncUartInterface {
public:
	enum Error {
		ERROR_BUSY             = AsyncUart::ERROR_BUSY,
		ERROR_TIMEOUT          = AsyncUart::ERROR_TIMEOUT,
		ERROR_INVALID_ARGUMENT = AsyncUart::ERROR_INVALID_ARGUMENT,
		ERROR_UNKNOWN,
	};

	Modem(
		USART_TypeDef* uart,
		GPIO_TypeDef*  txPort,
		uint32_t       txPin,
		GPIO_TypeDef*  rxPort,
		uint32_t       rxPin,
		GPIO_TypeDef*  resetPort,
		uint32_t       resetPin
	);
	~Modem();

	/**
	 * @brief Send data to the modem.
	 *
	 * @param[in] data Data to send.
	 * @param[in] size Size of the data to send.
	 *
	 * @return error_code_t ERROR_NONE if successful. Otherwise, an error
	 *                      code of Modem::Error.
	 */
	error_code_t Send(uint8_t* data, uint32_t size);
	error_code_t Reset(bool hwReset = true);

protected:
	void RxCallback(uint8_t* data, uint32_t size) override;
	void RxErrorCallback(uint8_t* data, uint32_t size) override;
	void TxCallback(uint8_t* data, uint32_t size) override;
	void TxErrorCallback(uint8_t* data, uint32_t size) override;

private:
	AsyncUart _uart;
	Gpio      _resetPin;

	enum class State {
		IDLE = 0,
		ERROR,
		UNDER_RESET,
		TRANSMITTING,
	} _state;
};
