#pragma once

#include "errno.hpp"
#include "gpio.hpp"
#include "stm32l552xx.h"
#include "system.hpp"

#include <cstdint>

class I2c {
public:
	enum Error {
		ERROR_BUSY = 1,
		ERROR_TIMEOUT,
		ERROR_NAK,
		ERROR_INVALID_ARGUMENT,
	};

	enum class Callbacks {
		TX,
		RX
	};

	using IrqHandler = void (I2c::*)(uint32_t);
	using Callback   = void (*)(uint8_t*, uint32_t);

	I2c(I2C_TypeDef*  i2c,
	    GPIO_TypeDef* sdaPort,
	    uint32_t      sdaPin,
	    GPIO_TypeDef* sclPort,
	    uint32_t      sclPin);
	~I2c();

	/**
	 * @brief Register a callback for the I2C.
	 *
	 * @param[in] type     Type of callback.
	 * @param[in] callback Callback function.
	 *
	 * @return error_code_t ERROR_NONE if successful. Otherwise, an error
	 *                      code of I2c::Error.
	 */
	error_code_t RegisterCallback(Callbacks type, Callback callback);

	/**
	 * @brief Transmit data over the I2C.
	 *
	 * @param[in] slaveAddress    Address of the device to transmit to.
	 * @param[in] registerAddress Register address to write to.
	 * @param[in] data            Data to transmit.
	 * @param[in] size            Size of the data to transmit.
	 * @param[in] timeout         Timeout in ticks.
	 *
	 * @return error_code_t ERROR_NONE if successful. Otherwise, an error
	 *                      code of I2c::Error.
	 */
	error_code_t Transmit(
		uint8_t  slaveAddress,
		uint8_t  registerAddress,
		uint8_t* data,
		uint32_t size,
		TickType timeout = 0UL
	);

	/**
	 * @brief Receive data over the I2C.
	 *
	 * @param[in]     slaveAddress    Address of the device to read from.
	 * @param[in]     registerAddress Register address to read from.
	 * @param[in,out] data            Data received.
	 * @param[in]     size            Size of the data to receive.
	 * @param[in]     timeout         Timeout in ticks.
	 *
	 * @return error_code_t ERROR_NONE if successful. Otherwise, an error
	 *                      code of I2c::Error.
	 */
	error_code_t Receive(
		uint8_t  slaveAddress,
		uint8_t  registerAddress,
		uint8_t* data,
		uint32_t size,
		TickType timeout = 0UL
	);

private:
	Callback     _txCallback = nullptr;
	Callback     _rxCallback = nullptr;
	Gpio         _txPin;
	Gpio         _rxPin;
	I2C_TypeDef* _i2c;

	void _DisableIrq() {
		if (_i2c == I2C2) {
			__NVIC_DisableIRQ(I2C2_EV_IRQn);
			__NVIC_DisableIRQ(I2C2_ER_IRQn);
		}
	}

	void _EnableIrq() {
		if (_i2c == I2C2) {
			__NVIC_EnableIRQ(I2C2_EV_IRQn);
			__NVIC_EnableIRQ(I2C2_ER_IRQn);
		}
	}

public:
	/**
	 * @brief IRQ handler for the I2C instance.
	 *
	 * @note DO NOT CALL THIS YOURSELF! This is called by the IRQ handler
	 *       and has to be a public method to be callable by the IRQ
	 *       handler.
	 */
	void _IrqHandler(uint32_t isr);
};
