#pragma once

#include <cstdint>

// TODO: Include MCU header.

enum GpioPort {
	GPIO_PORT_A = 0,
	GPIO_PORT_B,
	GPIO_PORT_C,
	GPIO_PORT_D,
	GPIO_PORT_E,
	GPIO_PORT_F,
	GPIO_PORT_G,
	GPIO_PORT_H,
	GPIO_PORT_MAX = GPIO_PORT_H
};

static constexpr uint32_t GPIO_PIN_MAX = 31UL;

template<GpioPort port, unsigned pin>
class Gpio {
	public:
	enum Mode : uint32_t {
		MODE_TODO = 0UL,
	};

	enum Type : uint32_t {
		TYPE_TODO = 0UL,
	};

	enum Speed : uint32_t {
		SPEED_TODO = 0UL,
	};

	enum PullUp : uint32_t {
		PULL_UP_TODO = 0UL,
	};

	enum AlternateFunction : uint32_t {
		AF_TODO = 0UL,
	};

	Gpio(
	    Mode              mode   = MODE_TODO,
	    Type              type   = TYPE_TODO,
	    Speed             speed  = SPEED_TODO,
	    PullUp            pullup = PULL_UP_TODO,
	    AlternateFunction af     = AF_TODO
	): _port(port), _pin(pin) {
		static_assert(port <= GPIO_PORT_MAX);
		static_assert(pin <= GPIO_PIN_MAX);
	}

	~Gpio() {
		// TODO: Reset the pin settings to default values.
	}

	private:
	GpioPort _port;
	uint32_t _pin;
};
