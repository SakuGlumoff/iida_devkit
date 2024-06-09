#include "stm32l552xx.h"

extern "C" {
	void SystemInit(void) {}

	void NMI_Handler(void) {
		while (true) {}
	}

	void HardFault_Handler(void) {
		while (true) {}
	}

	void MemManage_Handler(void) {
		while (true) {}
	}

	void BusFault_Handler(void) {
		while (true) {}
	}

	void UsageFault_Handler(void) {
		while (true) {}
	}

	void SecureFault_Handler(void) {
		while (true) {}
	}

	void SVC_Handler(void) {
		while (true) {}
	}

	void DebugMon_Handler(void) {
		while (true) {}
	}

	void PendSV_Handler(void) {
		while (true) {}
	}

	void SysTick_Handler(void) {
		while (true) {}
	}
}
