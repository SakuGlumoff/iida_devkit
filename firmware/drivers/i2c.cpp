#include "i2c.hpp"

#include "error.hpp"
#include "gpio.hpp"
#include "stm32l552xx.h"
#include "system.hpp"

#include <cstdint>

static constexpr inline uint8_t _AddrEncodeWrite(uint8_t const addr) {
	return (addr & ~(1U << 7UL));
}

static constexpr inline uint8_t _AddrEncodeRead(uint8_t const addr) {
	return (addr | (1U << 7UL));
}

enum I2cInstance {
	I2C_I2C2,
	I2C_INSTANCE_COUNT,
};

/**
 * @brief Instance storage for I2C interrupts.
 *        This array is used to dispatch interrupts to the correct instance.
 */
static I2c* _i2cInstances[I2C_INSTANCE_COUNT] = {nullptr};

I2c::I2c(
	I2C_TypeDef*  i2c,
	GPIO_TypeDef* txPort,
	uint32_t      txPin,
	GPIO_TypeDef* rxPort,
	uint32_t      rxPin
):
_txPin(
	txPort,
	txPin,
	Gpio::Mode::ALTERNATE_FUNCTION,
	Gpio::Type::PUSH_PULL,
	Gpio::PullUp::PULL_UP,
	Gpio::Speed::MEDIUM
),
_rxPin(
	rxPort,
	rxPin,
	Gpio::Mode::ALTERNATE_FUNCTION,
	Gpio::Type::PUSH_PULL,
	Gpio::PullUp::PULL_UP,
	Gpio::Speed::MEDIUM
),
_i2c(i2c) {
	if (_i2c == I2C2) {
		RCC->APB1ENR1 |= RCC_APB1ENR1_I2C2EN;
		// Set to use HSI (16 MHz) as basis for the clock
		RCC->CCIPR1 &= ~RCC_CCIPR1_I2C2SEL;
		RCC->CCIPR1 |= RCC_CCIPR1_I2C2SEL_1;
		_txPin.SetAlternateFunction(Gpio::AlternateFunction::AF4);
		_rxPin.SetAlternateFunction(Gpio::AlternateFunction::AF4);
		_i2cInstances[I2C_I2C2] = this;
	} else {
		PANIC("Unsupported I2C");
	}

	_i2c->CR1 = 0UL;
	_i2c->CR1 |= (I2C_CR1_TXIE | I2C_CR1_RXIE);
	_i2c->CR2 = 0UL;
	_i2c->TIMINGR &= ~I2C_TIMINGR_PRESC;
	// t_presc = (PRESC+1) x t_i2cclk
	// 10 kHz clock
	_i2c->TIMINGR |= (150UL << I2C_TIMINGR_PRESC_Pos);

	_i2c->CR1 |= I2C_CR1_PE;
}

I2c::~I2c() {
	_DisableIrq();
	_i2c->CR1 &= ~USART_CR1_UE;
	_i2c->TIMINGR = 0UL;
	_i2c->CR2     = 0UL;
	_i2c->CR1     = 0UL;
	if (_i2c == I2C2) {
		RCC->APB1ENR1 &= ~RCC_APB1ENR1_I2C2EN;
		RCC->CCIPR1 &= ~RCC_CCIPR1_I2C2SEL;
		_i2cInstances[I2C_I2C2] = nullptr;
	} else {
		PANIC("Unsupported I2C");
	}
	// NOTE: GPIO deinitialization is done in the Gpio destructor.
}

error_code_t
I2c::RegisterCallback(I2c::Callbacks type, I2c::Callback callback) {
	switch (type) {
		case I2c::Callbacks::TX:
			_txCallback = callback;
			break;
		case I2c::Callbacks::RX:
			_rxCallback = callback;
			break;
		default:
			return -ERROR_INVALID_ARGUMENT;
	}
	return ERROR_NONE;
}

error_code_t I2c::Transmit(
	uint8_t  slaveAddress,
	uint8_t  registerAddress,
	uint8_t* data,
	uint32_t size,
	TickType timeout
) {
	if (data == nullptr) {
		return -ERROR_INVALID_ARGUMENT;
	}
	if (size >= 0xFEUL) {
		return -ERROR_INVALID_ARGUMENT;
	}

	error_code_t ret  = ERROR_NONE;
	uint32_t     sent = 0UL;

	_DisableIrq();

	_i2c->CR2 &= ~I2C_CR2_ADD10;
	_i2c->CR2 &= ~I2C_CR2_SADD;
	_i2c->CR2 |= slaveAddress << 1UL;
	_i2c->CR2 &= ~I2C_CR2_RD_WRN;
	_i2c->CR2 &= ~I2C_CR2_NBYTES;
	_i2c->CR2 |= (size + 1) << I2C_CR2_NBYTES_Pos;

	_i2c->CR2 |= I2C_CR2_START;

	TickType start = GetTicks();
	_i2c->TXDR     = registerAddress & I2C_TXDR_TXDATA;

	while (true) {
		uint32_t volatile isr = _i2c->ISR;
		if (isr & I2C_ISR_TC) {
			_i2c->CR2 |= I2C_CR2_STOP;
			break;
		} else if (isr & I2C_ISR_TXIS) {
			if (sent >= size) {
				break;
			}
			_i2c->TXDR = data[sent++] & I2C_TXDR_TXDATA;
		} else {
			if (isr & I2C_ISR_NACKF) {
				_i2c->ICR |= I2C_ICR_NACKCF;
				ret = -ERROR_NAK;
				break;
			}
			if (GetTicks() - start > timeout) {
				_i2c->CR2 |= I2C_CR2_STOP;
				ret = -ERROR_TIMEOUT;
				break;
			}
		}
	}

	_EnableIrq();
	return ret;
}

error_code_t I2c::Receive(
	uint8_t  slaveAddress,
	uint8_t  registerAddress,
	uint8_t* data,
	uint32_t size,
	TickType timeout
) {
	if (data == nullptr) {
		return -ERROR_INVALID_ARGUMENT;
	}
	if (size >= 0xFFUL) {
		return -ERROR_INVALID_ARGUMENT;
	}

	error_code_t ret  = ERROR_NONE;
	uint32_t     read = 0UL;

	_DisableIrq();

	// Write register address pointer first.
	_i2c->CR2 &= ~I2C_CR2_ADD10;
	_i2c->CR2 &= ~I2C_CR2_SADD;
	_i2c->CR2 |= slaveAddress << 1UL;
	_i2c->CR2 &= ~I2C_CR2_RD_WRN;
	_i2c->CR2 &= ~I2C_CR2_NBYTES;
	_i2c->CR2 |= 1UL << I2C_CR2_NBYTES_Pos;

	_i2c->CR2 |= I2C_CR2_START;

	TickType start = GetTicks();
	_i2c->TXDR     = registerAddress & I2C_TXDR_TXDATA;

	while (true) {
		uint32_t volatile isr = _i2c->ISR;
		if (isr & I2C_ISR_TC) {
			break;
		} else {
			if (isr & I2C_ISR_NACKF) {
				_i2c->ICR |= I2C_ICR_NACKCF;
				ret = -ERROR_NAK;
				break;
			}
			if (GetTicks() - start > timeout) {
				_i2c->CR2 |= I2C_CR2_STOP;
				ret = -ERROR_TIMEOUT;
				break;
			}
		}
	}

	if (ret != ERROR_NONE) {
		_EnableIrq();
		return ret;
	}

	// And then read the data.
	_i2c->CR2 |= I2C_CR2_RD_WRN;
	_i2c->CR2 &= ~I2C_CR2_NBYTES;
	_i2c->CR2 |= size << I2C_CR2_NBYTES_Pos;

	_i2c->CR2 |= I2C_CR2_START;

	while (true) {
		uint32_t volatile isr = _i2c->ISR;
		if (isr & I2C_ISR_TC) {
			_i2c->CR2 |= I2C_CR2_STOP;
			break;
		} else if (isr & I2C_ISR_RXNE) {
			if (read >= size) {
				break;
			}
			data[read++] = (uint8_t)(_i2c->RXDR & I2C_RXDR_RXDATA);
		} else {
			if (isr & I2C_ISR_NACKF) {
				_i2c->ICR |= I2C_ICR_NACKCF;
				ret = -ERROR_NAK;
				break;
			}
			if (GetTicks() - start > timeout) {
				_i2c->CR2 |= I2C_CR2_STOP;
				ret = -ERROR_TIMEOUT;
				break;
			}
		}
	}

	_EnableIrq();
	return ret;
}

void I2c::_IrqHandler(uint32_t isr) {
	(void)isr;
}

extern "C" {
	void I2C2_EV_IRQHandler(void) {
		if (_i2cInstances[I2C_I2C2] != nullptr) {
			_i2cInstances[I2C_I2C2]->_IrqHandler(I2C2->ISR);
		}
	}

	void I2C1_ER_IRQHandler(void) {
		if (_i2cInstances[I2C_I2C2] != nullptr) {
			_i2cInstances[I2C_I2C2]->_IrqHandler(I2C2->ISR);
		}
	}
}
