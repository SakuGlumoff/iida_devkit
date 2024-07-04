#include "SEGGER_RTT.h"
#include "bootloader.hpp"
#include "debug_print.h"
#include "memorymap.hpp"
#include "system.hpp"
#include "ymodem.hpp"

#include <cstdint>
#include <cstring>

extern "C" int debug_print_callback(char* debugMessage, unsigned int length) {
#ifdef CONFIG_UART
	// TODO: Print using UART.
#else
	SEGGER_RTT_Write(0, debugMessage, length);
	return 0;
#endif
}

static inline bool _VerifyImage() {
	AppHeader volatile* const appHeader =
	    (reinterpret_cast<AppHeader*>(APP_HEADER_START));

	if (appHeader->id != APP_HEADER_ID) {
		return false;
	}

	if (appHeader->crc != CalculateAppCrc32(appHeader->size)) {
		return false;
	}

	return true;
}

static void _Init() {
	// TODO: Initialize system and peripherals.
	SystemInit();
}

static void _Deinit() {
	// TODO: Initialize system and peripherals.
	SystemDeinit();
}

extern "C" int main() {
	_Init();

	while (true) {
		bool imageGood = _VerifyImage();
		if (imageGood) {
			// Firmware installed.

			// Disable peripherals before jumping to it.
			_Deinit();

			StartApplication();
		}
		YModem::ReceiveNewAppImage();
	}

	return 0;
}
