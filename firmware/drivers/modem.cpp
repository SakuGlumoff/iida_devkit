#include "modem.hpp"

#include "async_uart.hpp"
#include "gpio.hpp"
#include "pinmap.hpp"

#include <cstdint>
#include <cstring>

Modem::Modem(
	USART_TypeDef* uart,
	GPIO_TypeDef*  txPort,
	uint32_t       txPin,
	GPIO_TypeDef*  rxPort,
	uint32_t       rxPin,
	GPIO_TypeDef*  resetPort,
	uint32_t       resetPin
):
_uart(uart, txPort, txPin, rxPort, rxPin, this),
_resetPin(
	resetPort,
	resetPin,
	Gpio::Mode::OUTPUT,
	Gpio::Type::OPEN_DRAIN,
	Gpio::PullUp::NONE,
	Gpio::Speed::LOW
) {
	// Reset pin is active low.
	resetPin = true;
}

Modem::~Modem() {}

error_code_t Modem::Send(uint8_t* data, uint32_t size) {
	_state           = State::TRANSMITTING;
	error_code_t err = _uart.Transmit(data, size);
	if (err) {
		return err;
	}
	while (_state == State::TRANSMITTING) {
		Sleep(msToTicks(100));
	}
	if (_state == State::ERROR) {
		return -Modem::ERROR_UNKNOWN;
	}
	return ERROR_NONE;
}

error_code_t Modem::Reset(bool hwReset) {
	if (hwReset) {
		_resetPin = 0;
		Sleep(msToTicks(100));
		_resetPin = 1;
	} else {
		_state = State::UNDER_RESET;
		error_code_t err =
			Send((uint8_t*)"AT+CFUN=16\r\n", strlen("AT+CFUN=16\r\n"));
		if (err) {
			return err;
		}
		while (_state == State::UNDER_RESET) {
			Sleep(msToTicks(100));
		}
		if (_state == State::ERROR) {
			// Error happened -> Use HW reset.
			return Reset(true);
		}
	}
	return ERROR_NONE;
}

void Modem::TxCallback(uint8_t* data, uint32_t size) {
	(void)data;
	(void)size;
}

void Modem::RxCallback(uint8_t* data, uint32_t size) {
	if (strncmp((char const*)data, "OK\r\n", size) == 0) {
		switch (_state) {
			case State::UNDER_RESET:
				_state = State::IDLE;
				break;
			default:
				break;
		}
	}
}

void Modem::RxErrorCallback(uint8_t* data, uint32_t size) {
	(void)data;
	(void)size;
	_state = State::ERROR;
}

void Modem::TxErrorCallback(uint8_t* data, uint32_t size) {
	(void)data;
	(void)size;
	_state = State::ERROR;
}
