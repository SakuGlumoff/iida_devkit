#include "SEGGER_RTT.h"
#include "config.hpp"
#include "debug_print.h"
#include "gpio.hpp"
#include "pinmap.hpp"
#include "system.hpp"

extern "C" int debug_print_callback(char* debugMessage, unsigned int length) {
	unsigned avail = SEGGER_RTT_GetAvailWriteSpace(0);
	if (avail < length) {
		return DBG_PRINT_BUFFER_TOO_SMALL;
	}
	return SEGGER_RTT_Write(0, debugMessage, length);
}

/**
 * @brief Initialize system and peripherals.
 */
static void _Init() {
	SystemInit();
	SEGGER_RTT_Init();
}

int main(void) {
	_Init();

	bool ledState = true;
	Gpio led(
	    LED_DBG_A,
	    Gpio::Mode::OUTPUT,
	    Gpio::Type::PUSH_PULL,
	    Gpio::PullUp::PULL_UP,
	    Gpio::Speed::LOW
	);
	led = 1;

	while (true) {
		if (ledState) {
			DBG_PRINTF_TRACE("Setting LED on");
		} else {
			DBG_PRINTF_TRACE("Setting LED off");
		}
		led      = ledState;
		ledState = !ledState;
		Sleep(msToTicks(500));
	}

	return 0;
}
