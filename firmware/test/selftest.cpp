#include "selftest.hpp"

#include "SEGGER_RTT.h"
#include "debug_print.h"
#include "errno.hpp"
#include "error.hpp"
#include "flash.hpp"
#include "memorymap.hpp"
#include "selftest.hpp"
#include "stm32l552xx.h"
#include "system.hpp"

#include <cinttypes>
#include <cstring>

bool SelfTest::FlashTest() {
	constexpr uint32_t PATTERN_SIZE    = 0x1'0000UL;
	constexpr uint8_t  PATTERN_BYTE    = 0xAAU;
	constexpr uint32_t FLASH_TEST_ADDR = (DEVICE_FLASH_START + 0x1'0000UL);

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
				"Pattern not erased @ 0x%08X", i + FLASH_TEST_ADDR
			);
			return false;
		}
	}

	TickType const end      = GetTicks();
	uint32_t const duration = end - start;
	DBG_PRINTF_DEBUG("Flash test took %" PRIu32 " ticks.", duration);

	return true;
}

bool SelfTest::RunAll() {
	if (!FlashTest()) {
		DBG_PRINTF_ERROR("Flash test failed.");
		return false;
	}

	return true;
}
