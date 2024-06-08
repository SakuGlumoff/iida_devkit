#include "bootloader.hpp"

// TODO: Include MCU header.

#include "memorymap.hpp"

#include <cstdint>
#include <etl/crc32.h>

uint32_t CalculateAppCrc32(uint32_t const appSize) {
	if (appSize > APP_CODE_SIZE) {
		return 0UL;
	}

	etl::crc32              ret;
	uint8_t const volatile* appImage =
	    reinterpret_cast<uint8_t*>(APP_CODE_START);

	for (size_t i = 0; i < appSize; i++) {
		uint8_t const val = appImage[i];
		ret.add(val);
	}

	return ret.value();
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
