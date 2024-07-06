#include "SEGGER_RTT.h"
#include "bootloader.hpp"
#include "debug_print.h"
#include "error.hpp"
#include "memorymap.hpp"
#include "stm32l552xx.h"
#include "system.hpp"
#include "xmodem.hpp"

#include <cstdint>
#include <cstring>

static uint32_t _imageOffset = 0;

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

static void _HandleNewImagePacket(uint8_t* buffer, uint32_t size) {
	// TODO: Write the image to the flash memory.
	DBG_PRINTF_DEBUG(
	    "Writing %lu bytes of the new image to 0x%08X",
	    size,
	    _imageOffset
	);
	(void)buffer;
	(void)size;

	_imageOffset += size;
}

extern "C" int main() {
	_Init();

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
		DBG_PRINTF_DEBUG("No valid image found.");
		_imageOffset = 0UL;
		XModem::DownloadImage(_HandleNewImagePacket);
	}

	PANIC("Bootloader failed to start application.");

	return 0;
}
