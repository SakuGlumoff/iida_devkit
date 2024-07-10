#include "SEGGER_RTT.h"
#include "bootloader.hpp"
#include "debug_print.h"
#include "errno.hpp"
#include "error.hpp"
#include "flash.hpp"
#include "memorymap.hpp"
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
	DBG_PRINTF_DEBUG(
	    "Writing %lu bytes of the new image to 0x%08X",
	    size,
	    _imageOffset
	);

	// TODO: Write the image to the flash memory.
	(void)buffer;
	(void)size;

	_imageOffset += size;
}

static bool _FlashTest() {
	constexpr uint32_t PATTERN_SIZE    = 0x10000UL;
	constexpr uint8_t  PATTERN_BYTE    = 0xAAU;
	constexpr uint32_t FLASH_TEST_ADDR = (DEVICE_FLASH_START + 0x10000UL);

	static uint8_t pattern[PATTERN_SIZE] = {};
	memset(pattern, PATTERN_BYTE, PATTERN_SIZE);

	DBG_PRINTF_DEBUG("Testing flash memory.");

	error_code_t   err   = ERROR_NONE;
	TickType const start = GetTicks();

	err = Flash::Erase(FLASH_TEST_ADDR, PATTERN_SIZE);
	if (err) {
		DBG_PRINTF_ERROR("Could not erase flash memory: %d", err);
		return false;
	}

	err = Flash::Write(FLASH_TEST_ADDR, pattern, PATTERN_SIZE);
	if (err) {
		DBG_PRINTF_ERROR("Could not write to flash memory: %d", err);
		return false;
	}

	for (uint32_t i = 0; i < PATTERN_SIZE; i++) {
		uint8_t byte = *reinterpret_cast<uint8_t*>(i + FLASH_TEST_ADDR);
		if (byte != PATTERN_BYTE) {
			DBG_PRINTF_ERROR(
			    "Pattern (0x%02X) not written @ 0x%08X",
			    PATTERN_BYTE,
			    i + FLASH_TEST_ADDR
			);
			return false;
		}
	}

	err = Flash::Erase(FLASH_TEST_ADDR, PATTERN_SIZE);
	if (err) {
		DBG_PRINTF_ERROR("Could not erase flash memory: %d", err);
		return false;
	}

	for (uint32_t i = 0; i < PATTERN_SIZE; i++) {
		uint8_t byte = *reinterpret_cast<uint8_t*>(i + FLASH_TEST_ADDR);
		if (byte != 0xFFU) {
			DBG_PRINTF_ERROR(
			    "Pattern not erased @ 0x%08X",
			    i + FLASH_TEST_ADDR
			);
			return false;
		}
	}

	TickType const end = GetTicks();
	DBG_PRINTF_DEBUG("Flash test took %llu ticks.", end - start);

	return true;
}

extern "C" int main() {
	_Init();

	bool test = _FlashTest();
	if (!test) {
		PANIC("Flash test failed.");
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
		DBG_PRINTF_DEBUG("No valid image found.");
		_imageOffset = 0UL;
		XModem::DownloadImage(_HandleNewImagePacket);
	}

	PANIC("Bootloader failed to start application.");

	return 0;
}
