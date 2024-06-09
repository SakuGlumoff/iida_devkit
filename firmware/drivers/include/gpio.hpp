#pragma once

#include "error.hpp"
#include "stm32l552xx.h"

#include <cstdint>
#include <type_traits>

class Gpio {
	public:
	enum class Mode : uint32_t {
		INPUT = 0UL,
		OUTPUT,
		ALTERNATE_FUNCTION,
		ANALOG
	};

	enum class Type : uint32_t {
		PUSH_PULL = 0UL,
		OPEN_DRAIN
	};

	enum class Speed : uint32_t {
		LOW = 0UL,
		MEDIUM,
		HIGH,
		VERY_HIGH
	};

	enum class PullUp : uint32_t {
		NONE = 0UL,
		PULL_UP,
		PULL_DOWN
	};

	enum class AlternateFunction : uint32_t {
		AF0 = 0UL,
		AF1,
		AF2,
		AF3,
		AF4,
		AF5,
		AF6,
		AF7,
		AF8,
		AF9,
		AF10,
		AF11,
		AF12,
		AF13,
		AF14,
		AF15
	};

	void SetMode(Mode mode) {
		_port->MODER &= ~(0b11 << (_pin * 2UL));
		uint32_t val = static_cast<uint32_t>(mode);
		_port->MODER |= (val << (_pin * 2UL));
	}

	void SetType(Type type) {
		_port->OTYPER &= ~(0b1 << _pin);
		uint32_t val = static_cast<uint32_t>(type);
		_port->OTYPER |= (val << _pin);
	}

	void SetSpeed(Speed speed) {
		_port->OSPEEDR &= ~(0b11 << (_pin * 2UL));
		uint32_t val = static_cast<uint32_t>(speed);
		_port->OSPEEDR |= (val << (_pin * 2UL));
	}

	void SetPullUp(PullUp pullup) {
		_port->PUPDR &= ~(0b11 << (_pin * 2UL));
		uint32_t val = static_cast<uint32_t>(pullup);
		_port->PUPDR |= (val << (_pin * 2UL));
	}

	void SetAlternateFunction(AlternateFunction af) {
		uint32_t idx = (_pin <= 7UL) ? 0UL : 1UL;
		_port->AFR[idx] &= ~(0b1111 << ((_pin % 8UL) * 4UL));
		uint32_t val = static_cast<uint32_t>(af);
		_port->AFR[idx] |= (val << ((_pin % 8UL) * 4UL));
	}

	void Write(bool on) {
		if (on) {
			_port->BSRR = (0b1 << _pin);
		} else {
			_port->BSRR = (0b1 << (_pin + 16UL));
		}
	}

	bool Read() {
		return _port->IDR & (0b1 << _pin);
	}

	Gpio(
	    GPIO_TypeDef*     port,
	    uint32_t          pin,
	    Mode              mode   = Mode::ANALOG,
	    Type              type   = Type::PUSH_PULL,
	    Speed             speed  = Speed::LOW,
	    PullUp            pullup = PullUp::NONE,
	    AlternateFunction af     = AlternateFunction::AF0
	) {
		switch (reinterpret_cast<uint32_t>(port)) {
			case GPIOA_BASE:
				RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN;
				break;
			case GPIOB_BASE:
				RCC->AHB2ENR |= RCC_AHB2ENR_GPIOBEN;
				break;
			case GPIOC_BASE:
				RCC->AHB2ENR |= RCC_AHB2ENR_GPIOCEN;
				break;
			case GPIOD_BASE:
				RCC->AHB2ENR |= RCC_AHB2ENR_GPIODEN;
				break;
			case GPIOE_BASE:
				RCC->AHB2ENR |= RCC_AHB2ENR_GPIOEEN;
				break;
			case GPIOF_BASE:
				RCC->AHB2ENR |= RCC_AHB2ENR_GPIOFEN;
				break;
			case GPIOG_BASE:
				RCC->AHB2ENR |= RCC_AHB2ENR_GPIOGEN;
				break;
			case GPIOH_BASE:
				RCC->AHB2ENR |= RCC_AHB2ENR_GPIOHEN;
				break;
			default:
				PANIC(
				    "Bad GPIO port: 0x%08X",
				    reinterpret_cast<uint32_t>(port)
				);
		}
		if (pin > _PIN_MAX) {
			PANIC("Bad GPIO pin: %u", pin);
		}
		_port = port;
		_pin  = pin;
		SetMode(mode);
		SetType(type);
		SetSpeed(speed);
		SetPullUp(pullup);
		SetAlternateFunction(af);
	}

	~Gpio() {
		SetMode(Mode::ANALOG);
		SetType(Type::PUSH_PULL);
		SetSpeed(Speed::LOW);
		SetPullUp(PullUp::NONE);
		SetAlternateFunction(AlternateFunction::AF0);
	}

	private:
	static constexpr uint32_t _PIN_MAX = 15UL;
	GPIO_TypeDef*             _port    = nullptr;
	uint32_t                  _pin     = _PIN_MAX + 1;
};
