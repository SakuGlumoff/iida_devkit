#include "bootloader.hpp"

#include "crc16.hpp"
#include "memorymap.hpp"
#include "stm32l552xx.h"

#include <cstdint>
#include <etl/crc32.h>

uint16_t CalculateAppCrc16(uint32_t startAddress, uint32_t size) {
	uint16_t                ret;
	uint8_t const volatile* appImage =
	    reinterpret_cast<uint8_t*>(startAddress);

	for (size_t i = 0; i < size; i++) {
		uint8_t const val = appImage[i];
		ret               = UpdateCrc16(ret, val);
	}

	return ret;
}

void StartApplication() {
	using IsrFunc                 = void (*)(void);
	IsrFunc const appResetHandler = *reinterpret_cast<IsrFunc*>(
	    reinterpret_cast<uint32_t*>(APP_CODE_START + 4UL)
	);

	SCB->VTOR = APP_CODE_START;

	uint32_t const appMsp = *reinterpret_cast<uint32_t*>(APP_CODE_START);
	__set_MSP(appMsp);

	appResetHandler();
}
