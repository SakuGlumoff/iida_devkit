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
	__disable_irq();

	// Setup vector table
	SCB->VTOR = APP_CODE_START;

	typedef void (*FunctionPointer)(void);
	FunctionPointer appResetHandler = reinterpret_cast<FunctionPointer>(
	    *((uint32_t volatile*)(APP_CODE_START + 4))
	);

	// Set main stack pointer
	uint32_t volatile msp = *((uint32_t volatile*)APP_CODE_START);
	__set_MSP(msp);

	// Jump to application's reset handler
	appResetHandler();
}
