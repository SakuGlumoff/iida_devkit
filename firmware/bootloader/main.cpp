#include "SEGGER_RTT.h"
#include "bootloader.hpp"
#include "debug_print.h"
#include "memorymap.hpp"
#include "pinmap.hpp"
#include "stm32l552xx.h"
#include "system.hpp"
#include "uart.hpp"
#include "ymodem.hpp"

#include <cstdint>
#include <cstring>

extern "C" int debug_print_callback(char* debugMessage, unsigned int length) {
	return SEGGER_RTT_Write(0, debugMessage, length);
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
	SEGGER_RTT_Init();
}

static void _Deinit() {
	// TODO: Initialize system and peripherals.
	SystemDeinit();
}

extern "C" int main() {
	_Init();

	DBG_PRINTF_DEBUG("Bootloader started.");

	Uart dbg_uart(LPUART1, DBG_UART_TX, DBG_UART_RX);

	char const* const msg = "Bootloader started.";
	for (size_t i = 0; i < strlen(msg); i++) {
		dbg_uart.Putc(static_cast<uint8_t>(msg[i]));
	}

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
