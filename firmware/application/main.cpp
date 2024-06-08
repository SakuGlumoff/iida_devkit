#include "config.hpp"
#include "debug_print.h"

extern "C" int debug_print_callback(char* debugMessage, unsigned int length) {
#ifdef CONFIG_UART
	// TODO: Print using UART.
#else
	(void)debugMessage;
	(void)length;
	return 0;
#endif
}

static void _Init() {
	// TODO: Initialize system and peripherals.
}

int main(void) {
	_Init();

	bool ledState = true;

	while (true) {
#ifdef CONFIG_LED
		// TODO: Set LED according to ledState.
#endif
		if (ledState) {
			DBG_PRINTF_TRACE("Setting LED on");
		} else {
			DBG_PRINTF_TRACE("Setting LED off");
		}
		ledState = !ledState;
		// TODO: Add 500 ms delay.
	}

	return 0;
}
