#pragma once

#include "stm32l552xx.h"

#include <cstdint>

enum class GpioPort : uint32_t {
	A = 0,
	B,
	C,
	D,
	E,
	F,
	G,
	H,
	MAX = H
};

static constexpr uint32_t GPIO_PIN_MAX = 15UL;

template<GpioPort port, unsigned pin>
class Gpio {
	public:
	enum class Mode : uint32_t {
		MODE_TODO = 0UL,
	};

	enum class Type : uint32_t {
		TYPE_TODO = 0UL,
	};

	enum class Speed : uint32_t {
		SPEED_TODO = 0UL,
	};

	enum class PullUp : uint32_t {
		PULL_UP_TODO = 0UL,
	};

	enum class AlternateFunction : uint32_t {
		AF_TODO = 0UL,
	};

	void SetMode(Mode mode) {
		(void)mode;
	}

	void SetType(Type type) {
		(void)type;
	}

	void SetSpeed(Speed speed) {
		(void)speed;
	}

	void SetPullUp(PullUp pullup) {
		(void)pullup;
	}

	void SetAlternateFunction(AlternateFunction af) {
		(void)af;
	}

	Gpio(
	    Mode              mode   = Mode::MODE_TODO,
	    Type              type   = Type::TYPE_TODO,
	    Speed             speed  = Speed::SPEED_TODO,
	    PullUp            pullup = PullUp::PULL_UP_TODO,
	    AlternateFunction af     = AlternateFunction::AF_TODO
	) {
		static_assert(port <= GpioPort::MAX);
		static_assert(pin <= GPIO_PIN_MAX);
		switch (port) {
			case GpioPort::A:
				_port = GPIOA_S;
				break;
			case GpioPort::B:
				_port = GPIOB_S;
				break;
			case GpioPort::C:
				_port = GPIOC_S;
				break;
			case GpioPort::D:
				_port = GPIOD_S;
				break;
			case GpioPort::E:
				_port = GPIOE_S;
				break;
			case GpioPort::F:
				_port = GPIOF_S;
				break;
			case GpioPort::G:
				_port = GPIOG_S;
				break;
			case GpioPort::H:
				_port = GPIOH_S;
				break;
			default:
				break;
		}
		_pin = pin;
		SetMode(mode);
		SetType(type);
		SetSpeed(speed);
		SetPullUp(pullup);
		SetAlternateFunction(af);
	}

	~Gpio() {
		// TODO: Reset the pin settings to default values.
	}

	private:
	GPIO_TypeDef* _port = nullptr;
	uint32_t      _pin  = GPIO_PIN_MAX + 1;
};
