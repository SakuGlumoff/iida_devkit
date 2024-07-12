#include "SEGGER_RTT.h"
#include "bootloader.hpp"
#include "common.hpp"
#include "debug_print.h"
#include "errno.hpp"
#include "error.hpp"
#include "flash.hpp"
#include "memorymap.hpp"
#include "selftest.hpp"
#include "stm32l552xx.h"
#include "system.hpp"
#include "xmodem.hpp"

#include <cstdint>
#include <cstring>

static uint32_t _imageOffset = 0;

extern "C" int debug_print_callback(char* debugMessage, unsigned int length) {
	unsigned avail = SEGGER_RTT_GetAvailWriteSpace(0);
	if (avail < length) {
		return DBG_PRINT_BUFFER_TOO_SMALL;
	}
	return SEGGER_RTT_Write(0, debugMessage, length);
}

static inline bool _VerifyImage() {
	AppHeader volatile* const appHeader =
	    (reinterpret_cast<AppHeader*>(APP_HEADER_START));

	if (appHeader->id != APP_HEADER_ID) {
		DBG_PRINTF_DEBUG("Invalid application header ID.");
		return false;
	}

	uint16_t crc = CalculateAppCrc16(APP_CODE_START, appHeader->size);

	if (appHeader->crc != crc) {
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

static error_code_t _HandleNewImagePacket(uint8_t* buffer, uint32_t size) {
	DBG_PRINTF_DEBUG(
	    "Writing %lu bytes of the new image to 0x%08X",
	    size,
	    APP_HEADER_START + _imageOffset
	);

	if ((APP_HEADER_START + _imageOffset + size)
	    > (APP_HEADER_START + APP_HEADER_SIZE + APP_CODE_SIZE)) {
		DBG_PRINTF_ERROR(
		    "New image packet won't fit in the application area."
		);
		return -1;
	}

	error_code_t err =
	    Flash::Write(APP_HEADER_START + _imageOffset, buffer, size);
	if (err) {
		DBG_PRINTF_ERROR(
		    "Failed to write new image packet to flash: %d",
		    err
		);
		return err;
	}

	_imageOffset += size;

	return ERROR_NONE;
}

extern "C" int main() {
	_Init();

#ifdef CONFIG_SELF_TEST
	bool selfTest = SelfTest::RunAll();
	if (!selfTest) {
		PANIC("Self tests failed.");
	}
#endif

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
