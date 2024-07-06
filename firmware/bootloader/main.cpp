#include "SEGGER_RTT.h"
#include "bootloader.hpp"
#include "debug_print.h"
#include "error.hpp"
#include "memorymap.hpp"
#include "system.hpp"
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
		DBG_PRINTF_DEBUG("Invalid application header ID.");
		return false;
	}

	if (appHeader->crc != CalculateAppCrc32(appHeader->size)) {
		DBG_PRINTF_DEBUG("Invalid application CRC.");
		return false;
	}

	DBG_PRINTF_DEBUG("Application image verified.");
	return true;
}

/**
 * @brief Initialize system and peripherals.
 */
static void _Init() {
	SystemInit();
	SEGGER_RTT_Init();
}

/**
 * @brief Deinitialize system and peripherals.
 */
static void _Deinit() {
	SystemDeinit();
}

extern "C" int main() {
	_Init();

	while (true) {
		DBG_PRINTF_DEBUG("Bootloader started.");
		Sleep(1'000);
	}

	while (true) {
		DBG_PRINTF_DEBUG("Testing image.");
		bool imageGood = _VerifyImage();
		if (imageGood) {
			DBG_PRINTF_DEBUG("Image is good.");

			DBG_PRINTF_DEBUG("Disabling peripherals.");
			_Deinit();

			DBG_PRINTF_DEBUG("Starting application.");
			StartApplication();
		}
		DBG_PRINTF_DEBUG("Image is bad -> waiting for new image.");
		YModem::ReceiveNewAppImage();
	}

	PANIC("Bootloader failed to start application.");

	return 0;
}
