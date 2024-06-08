// TODO: Include MCU header.

#include <cstdint>
#include <etl/algorithm.h>

#define NORETURN __attribute__((noreturn))
#define ALIAS(x) __attribute__((weak, alias(#x)))
#define WEAK_IRQ(_name, _alias)                                                \
	extern "C" void NORETURN ALIAS(_alias) _name(void)

extern "C" uint32_t const _estack;

extern "C" void DefaultHandler(void) {
	while (true) {
		__NOP();
	}
}

extern "C" void ResetHandler(void) {
	using InitFunction = void (*)(void);

	// Copy data from ROM to RAM.
	extern uint8_t _sdata;
	extern uint8_t _edata;
	extern uint8_t _etext;
	etl::copy_s(&_etext, &_etext + (&_edata - &_sdata), &_sdata, &_edata);

	// Initialize the bss section.
	extern uint8_t _sbss;
	extern uint8_t _ebss;
	etl::fill(&_sbss, &_ebss, 0U);

	// Call initializers.
	extern InitFunction __preinit_array_start;
	extern InitFunction __preinit_array_end;
	etl::for_each(
	    &__preinit_array_start,
	    &__preinit_array_end,
	    [](InitFunction const f) {
		    f();
	    }
	);
	extern InitFunction __init_array_start;
	extern InitFunction __init_array_end;
	etl::for_each(
	    &__init_array_start,
	    &__init_array_end,
	    [](InitFunction const f) {
		    f();
	    }
	);

	asm("b main");
}

extern "C" void NMI_Handler(void) {
	while (true) {
		__NOP();
	}
}

extern "C" void HardFault_Handler(void) {
	while (true) {
		__NOP();
	}
}

extern "C" void MemManage_Handler(void) {
	while (true) {
		__NOP();
	}
}

extern "C" void BusFault_Handler(void) {
	while (true) {
		__NOP();
	}
}

extern "C" void UsageFault_Handler(void) {
	while (true) {
		__NOP();
	}
}

extern "C" void SVC_Handler(void) {
	while (true) {
		__NOP();
	}
}

extern "C" void DebugMon_Handler(void) {
	while (true) {
		__NOP();
	}
}

extern "C" void PendSV_Handler(void) {
	while (true) {
		__NOP();
	}
}

// External interrupts
WEAK_IRQ(WWDG_IRQHandler, DefaultHandler);
WEAK_IRQ(PVD_IRQHandler, DefaultHandler);
WEAK_IRQ(TAMP_STAMP_IRQHandler, DefaultHandler);
WEAK_IRQ(RTC_WKUP_IRQHandler, DefaultHandler);
WEAK_IRQ(FLASH_IRQHandler, DefaultHandler);
WEAK_IRQ(RCC_IRQHandler, DefaultHandler);
WEAK_IRQ(EXTI0_IRQHandler, DefaultHandler);
WEAK_IRQ(EXTI1_IRQHandler, DefaultHandler);
WEAK_IRQ(EXTI2_IRQHandler, DefaultHandler);
WEAK_IRQ(EXTI3_IRQHandler, DefaultHandler);
WEAK_IRQ(EXTI4_IRQHandler, DefaultHandler);
WEAK_IRQ(DMA1_Stream0_IRQHandler, DefaultHandler);
WEAK_IRQ(DMA1_Stream1_IRQHandler, DefaultHandler);
WEAK_IRQ(DMA1_Stream2_IRQHandler, DefaultHandler);
WEAK_IRQ(DMA1_Stream3_IRQHandler, DefaultHandler);
WEAK_IRQ(DMA1_Stream4_IRQHandler, DefaultHandler);
WEAK_IRQ(DMA1_Stream5_IRQHandler, DefaultHandler);
WEAK_IRQ(DMA1_Stream6_IRQHandler, DefaultHandler);
WEAK_IRQ(ADC_IRQHandler, DefaultHandler);
WEAK_IRQ(CAN1_TX_IRQHandler, DefaultHandler);
WEAK_IRQ(CAN1_RX0_IRQHandler, DefaultHandler);
WEAK_IRQ(CAN1_RX1_IRQHandler, DefaultHandler);
WEAK_IRQ(CAN1_SCE_IRQHandler, DefaultHandler);
WEAK_IRQ(EXTI9_5_IRQHandler, DefaultHandler);
WEAK_IRQ(TIM1_BRK_TIM9_IRQHandler, DefaultHandler);
WEAK_IRQ(TIM1_UP_TIM10_IRQHandler, DefaultHandler);
WEAK_IRQ(TIM1_TRG_COM_TIM11_IRQHandler, DefaultHandler);
WEAK_IRQ(TIM1_CC_IRQHandler, DefaultHandler);
WEAK_IRQ(TIM2_IRQHandler, DefaultHandler);
WEAK_IRQ(TIM3_IRQHandler, DefaultHandler);
WEAK_IRQ(TIM4_IRQHandler, DefaultHandler);
WEAK_IRQ(I2C1_EV_IRQHandler, DefaultHandler);
WEAK_IRQ(I2C1_ER_IRQHandler, DefaultHandler);
WEAK_IRQ(I2C2_EV_IRQHandler, DefaultHandler);
WEAK_IRQ(I2C2_ER_IRQHandler, DefaultHandler);
WEAK_IRQ(SPI1_IRQHandler, DefaultHandler);
WEAK_IRQ(SPI2_IRQHandler, DefaultHandler);
WEAK_IRQ(USART1_IRQHandler, DefaultHandler);
WEAK_IRQ(USART2_IRQHandler, DefaultHandler);
WEAK_IRQ(USART3_IRQHandler, DefaultHandler);
WEAK_IRQ(EXTI15_10_IRQHandler, DefaultHandler);
WEAK_IRQ(RTC_Alarm_IRQHandler, DefaultHandler);
WEAK_IRQ(OTG_FS_WKUP_IRQHandler, DefaultHandler);
WEAK_IRQ(TIM8_BRK_TIM12_IRQHandler, DefaultHandler);
WEAK_IRQ(TIM8_UP_TIM13_IRQHandler, DefaultHandler);
WEAK_IRQ(TIM8_TRG_COM_TIM14_IRQHandler, DefaultHandler);
WEAK_IRQ(TIM8_CC_IRQHandler, DefaultHandler);
WEAK_IRQ(DMA1_Stream7_IRQHandler, DefaultHandler);
WEAK_IRQ(FMC_IRQHandler, DefaultHandler);
WEAK_IRQ(SDIO_IRQHandler, DefaultHandler);
WEAK_IRQ(TIM5_IRQHandler, DefaultHandler);
WEAK_IRQ(SPI3_IRQHandler, DefaultHandler);
WEAK_IRQ(UART4_IRQHandler, DefaultHandler);
WEAK_IRQ(UART5_IRQHandler, DefaultHandler);
WEAK_IRQ(TIM6_DAC_IRQHandler, DefaultHandler);
WEAK_IRQ(TIM7_IRQHandler, DefaultHandler);
WEAK_IRQ(DMA2_Stream0_IRQHandler, DefaultHandler);
WEAK_IRQ(DMA2_Stream1_IRQHandler, DefaultHandler);
WEAK_IRQ(DMA2_Stream2_IRQHandler, DefaultHandler);
WEAK_IRQ(DMA2_Stream3_IRQHandler, DefaultHandler);
WEAK_IRQ(DMA2_Stream4_IRQHandler, DefaultHandler);
WEAK_IRQ(CAN2_TX_IRQHandler, DefaultHandler);
WEAK_IRQ(CAN2_RX0_IRQHandler, DefaultHandler);
WEAK_IRQ(CAN2_RX1_IRQHandler, DefaultHandler);
WEAK_IRQ(CAN2_SCE_IRQHandler, DefaultHandler);
WEAK_IRQ(OTG_FS_IRQHandler, DefaultHandler);
WEAK_IRQ(DMA2_Stream5_IRQHandler, DefaultHandler);
WEAK_IRQ(DMA2_Stream6_IRQHandler, DefaultHandler);
WEAK_IRQ(DMA2_Stream7_IRQHandler, DefaultHandler);
WEAK_IRQ(USART6_IRQHandler, DefaultHandler);
WEAK_IRQ(I2C3_EV_IRQHandler, DefaultHandler);
WEAK_IRQ(I2C3_ER_IRQHandler, DefaultHandler);
WEAK_IRQ(OTG_HS_EP1_OUT_IRQHandler, DefaultHandler);
WEAK_IRQ(OTG_HS_EP1_IN_IRQHandler, DefaultHandler);
WEAK_IRQ(OTG_HS_WKUP_IRQHandler, DefaultHandler);
WEAK_IRQ(OTG_HS_IRQHandler, DefaultHandler);
WEAK_IRQ(DCMI_IRQHandler, DefaultHandler);
WEAK_IRQ(FPU_IRQHandler, DefaultHandler);
WEAK_IRQ(SPI4_IRQHandler, DefaultHandler);
WEAK_IRQ(SAI1_IRQHandler, DefaultHandler);
WEAK_IRQ(SAI2_IRQHandler, DefaultHandler);
WEAK_IRQ(QUADSPI_IRQHandler, DefaultHandler);
WEAK_IRQ(CEC_IRQHandler, DefaultHandler);
WEAK_IRQ(SPDIF_RX_IRQHandler, DefaultHandler);
WEAK_IRQ(FMPI2C1_EV_IRQHandler, DefaultHandler);
WEAK_IRQ(FMPI2C1_ER_IRQHandler, DefaultHandler);

// clang-format off
__attribute__((section(".isr_vector"))) const volatile uintptr_t _isr_vector[] = {
	// Stack pointer
	reinterpret_cast<uintptr_t>(&_estack),
	// Cortex M4 interrupts
	reinterpret_cast<uintptr_t>(ResetHandler),
	reinterpret_cast<uintptr_t>(NMI_Handler),
	reinterpret_cast<uintptr_t>(HardFault_Handler),
	reinterpret_cast<uintptr_t>(MemManage_Handler),
	reinterpret_cast<uintptr_t>(BusFault_Handler),
	reinterpret_cast<uintptr_t>(UsageFault_Handler),
	reinterpret_cast<uintptr_t>(nullptr),
	reinterpret_cast<uintptr_t>(nullptr),
	reinterpret_cast<uintptr_t>(nullptr),
	reinterpret_cast<uintptr_t>(nullptr),
	reinterpret_cast<uintptr_t>(SVC_Handler),
	reinterpret_cast<uintptr_t>(DebugMon_Handler),
	reinterpret_cast<uintptr_t>(nullptr),
	reinterpret_cast<uintptr_t>(PendSV_Handler),
	reinterpret_cast<uintptr_t>(SysTick_Handler),
	// External interrupts
	reinterpret_cast<uintptr_t>(WWDG_IRQHandler),
	reinterpret_cast<uintptr_t>(PVD_IRQHandler),
	reinterpret_cast<uintptr_t>(TAMP_STAMP_IRQHandler),
	reinterpret_cast<uintptr_t>(RTC_WKUP_IRQHandler),
	reinterpret_cast<uintptr_t>(FLASH_IRQHandler),
	reinterpret_cast<uintptr_t>(RCC_IRQHandler),
	reinterpret_cast<uintptr_t>(EXTI0_IRQHandler),
	reinterpret_cast<uintptr_t>(EXTI1_IRQHandler),
	reinterpret_cast<uintptr_t>(EXTI2_IRQHandler),
	reinterpret_cast<uintptr_t>(EXTI3_IRQHandler),
	reinterpret_cast<uintptr_t>(EXTI4_IRQHandler),
	reinterpret_cast<uintptr_t>(DMA1_Stream0_IRQHandler),
	reinterpret_cast<uintptr_t>(DMA1_Stream1_IRQHandler),
	reinterpret_cast<uintptr_t>(DMA1_Stream2_IRQHandler),
	reinterpret_cast<uintptr_t>(DMA1_Stream3_IRQHandler),
	reinterpret_cast<uintptr_t>(DMA1_Stream4_IRQHandler),
	reinterpret_cast<uintptr_t>(DMA1_Stream5_IRQHandler),
	reinterpret_cast<uintptr_t>(DMA1_Stream6_IRQHandler),
	reinterpret_cast<uintptr_t>(ADC_IRQHandler),
	reinterpret_cast<uintptr_t>(CAN1_TX_IRQHandler),
	reinterpret_cast<uintptr_t>(CAN1_RX0_IRQHandler),
	reinterpret_cast<uintptr_t>(CAN1_RX1_IRQHandler),
	reinterpret_cast<uintptr_t>(CAN1_SCE_IRQHandler),
	reinterpret_cast<uintptr_t>(EXTI9_5_IRQHandler),
	reinterpret_cast<uintptr_t>(TIM1_BRK_TIM9_IRQHandler),
	reinterpret_cast<uintptr_t>(TIM1_UP_TIM10_IRQHandler),
	reinterpret_cast<uintptr_t>(TIM1_TRG_COM_TIM11_IRQHandler),
	reinterpret_cast<uintptr_t>(TIM1_CC_IRQHandler),
	reinterpret_cast<uintptr_t>(TIM2_IRQHandler),
	reinterpret_cast<uintptr_t>(TIM3_IRQHandler),
	reinterpret_cast<uintptr_t>(TIM4_IRQHandler),
	reinterpret_cast<uintptr_t>(I2C1_EV_IRQHandler),
	reinterpret_cast<uintptr_t>(I2C1_ER_IRQHandler),
	reinterpret_cast<uintptr_t>(I2C2_EV_IRQHandler),
	reinterpret_cast<uintptr_t>(I2C2_ER_IRQHandler),
	reinterpret_cast<uintptr_t>(SPI1_IRQHandler),
	reinterpret_cast<uintptr_t>(SPI2_IRQHandler),
	reinterpret_cast<uintptr_t>(USART1_IRQHandler),
	reinterpret_cast<uintptr_t>(USART2_IRQHandler),
	reinterpret_cast<uintptr_t>(USART3_IRQHandler),
	reinterpret_cast<uintptr_t>(EXTI15_10_IRQHandler),
	reinterpret_cast<uintptr_t>(RTC_Alarm_IRQHandler),
	reinterpret_cast<uintptr_t>(OTG_FS_WKUP_IRQHandler),
	reinterpret_cast<uintptr_t>(TIM8_BRK_TIM12_IRQHandler),
	reinterpret_cast<uintptr_t>(TIM8_UP_TIM13_IRQHandler),
	reinterpret_cast<uintptr_t>(TIM8_TRG_COM_TIM14_IRQHandler),
	reinterpret_cast<uintptr_t>(TIM8_CC_IRQHandler),
	reinterpret_cast<uintptr_t>(DMA1_Stream7_IRQHandler),
	reinterpret_cast<uintptr_t>(FMC_IRQHandler),
	reinterpret_cast<uintptr_t>(SDIO_IRQHandler),
	reinterpret_cast<uintptr_t>(TIM5_IRQHandler),
	reinterpret_cast<uintptr_t>(SPI3_IRQHandler),
	reinterpret_cast<uintptr_t>(UART4_IRQHandler),
	reinterpret_cast<uintptr_t>(UART5_IRQHandler),
	reinterpret_cast<uintptr_t>(TIM6_DAC_IRQHandler),
	reinterpret_cast<uintptr_t>(TIM7_IRQHandler),
	reinterpret_cast<uintptr_t>(DMA2_Stream0_IRQHandler),
	reinterpret_cast<uintptr_t>(DMA2_Stream1_IRQHandler),
	reinterpret_cast<uintptr_t>(DMA2_Stream2_IRQHandler),
	reinterpret_cast<uintptr_t>(DMA2_Stream3_IRQHandler),
	reinterpret_cast<uintptr_t>(DMA2_Stream4_IRQHandler),
	reinterpret_cast<uintptr_t>(nullptr),
	reinterpret_cast<uintptr_t>(nullptr),
	reinterpret_cast<uintptr_t>(CAN2_TX_IRQHandler),
	reinterpret_cast<uintptr_t>(CAN2_RX0_IRQHandler),
	reinterpret_cast<uintptr_t>(CAN2_RX1_IRQHandler),
	reinterpret_cast<uintptr_t>(CAN2_SCE_IRQHandler),
	reinterpret_cast<uintptr_t>(OTG_FS_IRQHandler),
	reinterpret_cast<uintptr_t>(DMA2_Stream5_IRQHandler),
	reinterpret_cast<uintptr_t>(DMA2_Stream6_IRQHandler),
	reinterpret_cast<uintptr_t>(DMA2_Stream7_IRQHandler),
	reinterpret_cast<uintptr_t>(USART6_IRQHandler),
	reinterpret_cast<uintptr_t>(I2C3_EV_IRQHandler),
	reinterpret_cast<uintptr_t>(I2C3_ER_IRQHandler),
	reinterpret_cast<uintptr_t>(OTG_HS_EP1_OUT_IRQHandler),
	reinterpret_cast<uintptr_t>(OTG_HS_EP1_IN_IRQHandler),
	reinterpret_cast<uintptr_t>(OTG_HS_WKUP_IRQHandler),
	reinterpret_cast<uintptr_t>(OTG_HS_IRQHandler),
	reinterpret_cast<uintptr_t>(DCMI_IRQHandler),
	reinterpret_cast<uintptr_t>(nullptr),
	reinterpret_cast<uintptr_t>(nullptr),
	reinterpret_cast<uintptr_t>(FPU_IRQHandler),
	reinterpret_cast<uintptr_t>(nullptr),
	reinterpret_cast<uintptr_t>(nullptr),
	reinterpret_cast<uintptr_t>(SPI4_IRQHandler),
	reinterpret_cast<uintptr_t>(nullptr),
	reinterpret_cast<uintptr_t>(nullptr),
	reinterpret_cast<uintptr_t>(SAI1_IRQHandler),
	reinterpret_cast<uintptr_t>(nullptr),
	reinterpret_cast<uintptr_t>(nullptr),
	reinterpret_cast<uintptr_t>(nullptr),
	reinterpret_cast<uintptr_t>(SAI2_IRQHandler),
	reinterpret_cast<uintptr_t>(QUADSPI_IRQHandler),
	reinterpret_cast<uintptr_t>(CEC_IRQHandler),
	reinterpret_cast<uintptr_t>(SPDIF_RX_IRQHandler),
	reinterpret_cast<uintptr_t>(FMPI2C1_EV_IRQHandler),
	reinterpret_cast<uintptr_t>(FMPI2C1_ER_IRQHandler),
};