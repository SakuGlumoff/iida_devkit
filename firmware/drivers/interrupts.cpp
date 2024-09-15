#include "interrupts.hpp"

#include "stm32l552xx.h"

#include <cstdint>

#define __VECTOR_LOC __attribute__((section(".isr_vector")))

extern "C" void Default_Handler(void) {
	__NOP();
}

static constexpr uint32_t _ISR_VECTOR_TABLE_SIZE = 125UL;

static IrqHandler_t _registeredHandlers[_ISR_VECTOR_TABLE_SIZE] = {};

void RegisterIrqHandler(IRQn_Type irqn, IrqHandler_t handler) {
	if (irqn < 0) {
		return;
	}
	_registeredHandlers[irqn] = handler;
}

void DeregisterIrqHandler(IRQn_Type irqn) {
	if (irqn < 0) {
		return;
	}
	_registeredHandlers[irqn] = Default_Handler;
}

extern "C" {
	extern uint32_t _estack;

	extern void Reset_Handler(void);

	__WEAK void NMI_Handler(void) {
		while (true) {
			__NOP();
		}
	}

	__WEAK void HardFault_Handler(void) {
		while (true) {
			__NOP();
		}
	}

	__WEAK void MemManage_Handler(void) {
		while (true) {
			__NOP();
		}
	}

	__WEAK void BusFault_Handler(void) {
		while (true) {
			__NOP();
		}
	}

	__WEAK void UsageFault_Handler(void) {
		while (true) {
			__NOP();
		}
	}

	__WEAK void SecureFault_Handler(void) {
		while (true) {
			__NOP();
		}
	}

	__WEAK void SVC_Handler(void) {
		while (true) {
			__NOP();
		}
	}

	__WEAK void DebugMon_Handler(void) {
		while (true) {
			__NOP();
		}
	}

	__WEAK void PendSV_Handler(void) {
		while (true) {
			__NOP();
		}
	}

	__WEAK void SysTick_Handler(void) {
		while (true) {
			__NOP();
		}
	}

	void WWDG_IRQHandler(void) {
		if (_registeredHandlers[WWDG_IRQn] != nullptr) {
			_registeredHandlers[WWDG_IRQn]();
		}
	}

	void PVD_PVM_IRQHandler(void) {
		if (_registeredHandlers[PVD_PVM_IRQn] != nullptr) {
			_registeredHandlers[PVD_PVM_IRQn]();
		}
	}

	void RTC_IRQHandler(void) {
		if (_registeredHandlers[RTC_IRQn] != nullptr) {
			_registeredHandlers[RTC_IRQn]();
		}
	}

	void RTC_S_IRQHandler(void) {
		if (_registeredHandlers[RTC_S_IRQn] != nullptr) {
			_registeredHandlers[RTC_S_IRQn]();
		}
	}

	void TAMP_IRQHandler(void) {
		if (_registeredHandlers[TAMP_IRQn] != nullptr) {
			_registeredHandlers[TAMP_IRQn]();
		}
	}

	void TAMP_S_IRQHandler(void) {
		if (_registeredHandlers[TAMP_S_IRQn] != nullptr) {
			_registeredHandlers[TAMP_S_IRQn]();
		}
	}

	void FLASH_IRQHandler(void) {
		if (_registeredHandlers[FLASH_IRQn] != nullptr) {
			_registeredHandlers[FLASH_IRQn]();
		}
	}

	void FLASH_S_IRQHandler(void) {
		if (_registeredHandlers[FLASH_S_IRQn] != nullptr) {
			_registeredHandlers[FLASH_S_IRQn]();
		}
	}

	void GTZC_IRQHandler(void) {
		if (_registeredHandlers[GTZC_IRQn] != nullptr) {
			_registeredHandlers[GTZC_IRQn]();
		}
	}

	void RCC_IRQHandler(void) {
		if (_registeredHandlers[RCC_IRQn] != nullptr) {
			_registeredHandlers[RCC_IRQn]();
		}
	}

	void RCC_S_IRQHandler(void) {
		if (_registeredHandlers[RCC_S_IRQn] != nullptr) {
			_registeredHandlers[RCC_S_IRQn]();
		}
	}

	void EXTI0_IRQHandler(void) {
		if (_registeredHandlers[EXTI0_IRQn] != nullptr) {
			_registeredHandlers[EXTI0_IRQn]();
		}
	}

	void EXTI1_IRQHandler(void) {
		if (_registeredHandlers[EXTI1_IRQn] != nullptr) {
			_registeredHandlers[EXTI1_IRQn]();
		}
	}

	void EXTI2_IRQHandler(void) {
		if (_registeredHandlers[EXTI2_IRQn] != nullptr) {
			_registeredHandlers[EXTI2_IRQn]();
		}
	}

	void EXTI3_IRQHandler(void) {
		if (_registeredHandlers[EXTI3_IRQn] != nullptr) {
			_registeredHandlers[EXTI3_IRQn]();
		}
	}

	void EXTI4_IRQHandler(void) {
		if (_registeredHandlers[EXTI4_IRQn] != nullptr) {
			_registeredHandlers[EXTI4_IRQn]();
		}
	}

	void EXTI5_IRQHandler(void) {
		if (_registeredHandlers[EXTI5_IRQn] != nullptr) {
			_registeredHandlers[EXTI5_IRQn]();
		}
	}

	void EXTI6_IRQHandler(void) {
		if (_registeredHandlers[EXTI6_IRQn] != nullptr) {
			_registeredHandlers[EXTI6_IRQn]();
		}
	}

	void EXTI7_IRQHandler(void) {
		if (_registeredHandlers[EXTI7_IRQn] != nullptr) {
			_registeredHandlers[EXTI7_IRQn]();
		}
	}

	void EXTI8_IRQHandler(void) {
		if (_registeredHandlers[EXTI8_IRQn] != nullptr) {
			_registeredHandlers[EXTI8_IRQn]();
		}
	}

	void EXTI9_IRQHandler(void) {
		if (_registeredHandlers[EXTI9_IRQn] != nullptr) {
			_registeredHandlers[EXTI9_IRQn]();
		}
	}

	void EXTI10_IRQHandler(void) {
		if (_registeredHandlers[EXTI10_IRQn] != nullptr) {
			_registeredHandlers[EXTI10_IRQn]();
		}
	}

	void EXTI11_IRQHandler(void) {
		if (_registeredHandlers[EXTI11_IRQn] != nullptr) {
			_registeredHandlers[EXTI11_IRQn]();
		}
	}

	void EXTI12_IRQHandler(void) {
		if (_registeredHandlers[EXTI12_IRQn] != nullptr) {
			_registeredHandlers[EXTI12_IRQn]();
		}
	}

	void EXTI13_IRQHandler(void) {
		if (_registeredHandlers[EXTI13_IRQn] != nullptr) {
			_registeredHandlers[EXTI13_IRQn]();
		}
	}

	void EXTI14_IRQHandler(void) {
		if (_registeredHandlers[EXTI14_IRQn] != nullptr) {
			_registeredHandlers[EXTI14_IRQn]();
		}
	}

	void EXTI15_IRQHandler(void) {
		if (_registeredHandlers[EXTI15_IRQn] != nullptr) {
			_registeredHandlers[EXTI15_IRQn]();
		}
	}

	void DMAMUX1_IRQHandler(void) {
		if (_registeredHandlers[DMAMUX1_IRQn] != nullptr) {
			_registeredHandlers[DMAMUX1_IRQn]();
		}
	}

	void DMAMUX1_S_IRQHandler(void) {
		if (_registeredHandlers[DMAMUX1_S_IRQn] != nullptr) {
			_registeredHandlers[DMAMUX1_S_IRQn]();
		}
	}

	void DMA1_Channel1_IRQHandler(void) {
		if (_registeredHandlers[DMA1_Channel1_IRQn] != nullptr) {
			_registeredHandlers[DMA1_Channel1_IRQn]();
		}
	}

	void DMA1_Channel2_IRQHandler(void) {
		if (_registeredHandlers[DMA1_Channel2_IRQn] != nullptr) {
			_registeredHandlers[DMA1_Channel2_IRQn]();
		}
	}

	void DMA1_Channel3_IRQHandler(void) {
		if (_registeredHandlers[DMA1_Channel3_IRQn] != nullptr) {
			_registeredHandlers[DMA1_Channel3_IRQn]();
		}
	}

	void DMA1_Channel4_IRQHandler(void) {
		if (_registeredHandlers[DMA1_Channel4_IRQn] != nullptr) {
			_registeredHandlers[DMA1_Channel4_IRQn]();
		}
	}

	void DMA1_Channel5_IRQHandler(void) {
		if (_registeredHandlers[DMA1_Channel5_IRQn] != nullptr) {
			_registeredHandlers[DMA1_Channel5_IRQn]();
		}
	}

	void DMA1_Channel6_IRQHandler(void) {
		if (_registeredHandlers[DMA1_Channel6_IRQn] != nullptr) {
			_registeredHandlers[DMA1_Channel6_IRQn]();
		}
	}

	void DMA1_Channel7_IRQHandler(void) {
		if (_registeredHandlers[DMA1_Channel7_IRQn] != nullptr) {
			_registeredHandlers[DMA1_Channel7_IRQn]();
		}
	}

	void DMA1_Channel8_IRQHandler(void) {
		if (_registeredHandlers[DMA1_Channel8_IRQn] != nullptr) {
			_registeredHandlers[DMA1_Channel8_IRQn]();
		}
	}

	void ADC1_2_IRQHandler(void) {
		if (_registeredHandlers[ADC1_2_IRQn] != nullptr) {
			_registeredHandlers[ADC1_2_IRQn]();
		}
	}

	void DAC_IRQHandler(void) {
		if (_registeredHandlers[DAC_IRQn] != nullptr) {
			_registeredHandlers[DAC_IRQn]();
		}
	}

	void FDCAN1_IT0_IRQHandler(void) {
		if (_registeredHandlers[FDCAN1_IT0_IRQn] != nullptr) {
			_registeredHandlers[FDCAN1_IT0_IRQn]();
		}
	}

	void FDCAN1_IT1_IRQHandler(void) {
		if (_registeredHandlers[FDCAN1_IT1_IRQn] != nullptr) {
			_registeredHandlers[FDCAN1_IT1_IRQn]();
		}
	}

	void TIM1_BRK_IRQHandler(void) {
		if (_registeredHandlers[TIM1_BRK_IRQn] != nullptr) {
			_registeredHandlers[TIM1_BRK_IRQn]();
		}
	}

	void TIM1_UP_IRQHandler(void) {
		if (_registeredHandlers[TIM1_UP_IRQn] != nullptr) {
			_registeredHandlers[TIM1_UP_IRQn]();
		}
	}

	void TIM1_TRG_COM_IRQHandler(void) {
		if (_registeredHandlers[TIM1_TRG_COM_IRQn] != nullptr) {
			_registeredHandlers[TIM1_TRG_COM_IRQn]();
		}
	}

	void TIM1_CC_IRQHandler(void) {
		if (_registeredHandlers[TIM1_CC_IRQn] != nullptr) {
			_registeredHandlers[TIM1_CC_IRQn]();
		}
	}

	void TIM2_IRQHandler(void) {
		if (_registeredHandlers[TIM2_IRQn] != nullptr) {
			_registeredHandlers[TIM2_IRQn]();
		}
	}

	void TIM3_IRQHandler(void) {
		if (_registeredHandlers[TIM3_IRQn] != nullptr) {
			_registeredHandlers[TIM3_IRQn]();
		}
	}

	void TIM4_IRQHandler(void) {
		if (_registeredHandlers[TIM4_IRQn] != nullptr) {
			_registeredHandlers[TIM4_IRQn]();
		}
	}

	void TIM5_IRQHandler(void) {
		if (_registeredHandlers[TIM5_IRQn] != nullptr) {
			_registeredHandlers[TIM5_IRQn]();
		}
	}

	void TIM6_IRQHandler(void) {
		if (_registeredHandlers[TIM6_IRQn] != nullptr) {
			_registeredHandlers[TIM6_IRQn]();
		}
	}

	void TIM7_IRQHandler(void) {
		if (_registeredHandlers[TIM7_IRQn] != nullptr) {
			_registeredHandlers[TIM7_IRQn]();
		}
	}

	void TIM8_BRK_IRQHandler(void) {
		if (_registeredHandlers[TIM8_BRK_IRQn] != nullptr) {
			_registeredHandlers[TIM8_BRK_IRQn]();
		}
	}

	void TIM8_UP_IRQHandler(void) {
		if (_registeredHandlers[TIM8_UP_IRQn] != nullptr) {
			_registeredHandlers[TIM8_UP_IRQn]();
		}
	}

	void TIM8_TRG_COM_IRQHandler(void) {
		if (_registeredHandlers[TIM8_TRG_COM_IRQn] != nullptr) {
			_registeredHandlers[TIM8_TRG_COM_IRQn]();
		}
	}

	void TIM8_CC_IRQHandler(void) {
		if (_registeredHandlers[TIM8_CC_IRQn] != nullptr) {
			_registeredHandlers[TIM8_CC_IRQn]();
		}
	}

	void I2C1_EV_IRQHandler(void) {
		if (_registeredHandlers[I2C1_EV_IRQn] != nullptr) {
			_registeredHandlers[I2C1_EV_IRQn]();
		}
	}

	void I2C1_ER_IRQHandler(void) {
		if (_registeredHandlers[I2C1_ER_IRQn] != nullptr) {
			_registeredHandlers[I2C1_ER_IRQn]();
		}
	}

	void I2C2_EV_IRQHandler(void) {
		if (_registeredHandlers[I2C2_EV_IRQn] != nullptr) {
			_registeredHandlers[I2C2_EV_IRQn]();
		}
	}

	void I2C2_ER_IRQHandler(void) {
		if (_registeredHandlers[I2C2_ER_IRQn] != nullptr) {
			_registeredHandlers[I2C2_ER_IRQn]();
		}
	}

	void SPI1_IRQHandler(void) {
		if (_registeredHandlers[SPI1_IRQn] != nullptr) {
			_registeredHandlers[SPI1_IRQn]();
		}
	}

	void SPI2_IRQHandler(void) {
		if (_registeredHandlers[SPI2_IRQn] != nullptr) {
			_registeredHandlers[SPI2_IRQn]();
		}
	}

	void USART1_IRQHandler(void) {
		if (_registeredHandlers[USART1_IRQn] != nullptr) {
			_registeredHandlers[USART1_IRQn]();
		}
	}

	void USART2_IRQHandler(void) {
		if (_registeredHandlers[USART2_IRQn] != nullptr) {
			_registeredHandlers[USART2_IRQn]();
		}
	}

	void USART3_IRQHandler(void) {
		if (_registeredHandlers[USART3_IRQn] != nullptr) {
			_registeredHandlers[USART3_IRQn]();
		}
	}

	void UART4_IRQHandler(void) {
		if (_registeredHandlers[UART4_IRQn] != nullptr) {
			_registeredHandlers[UART4_IRQn]();
		}
	}

	void UART5_IRQHandler(void) {
		if (_registeredHandlers[UART5_IRQn] != nullptr) {
			_registeredHandlers[UART5_IRQn]();
		}
	}

	void LPUART1_IRQHandler(void) {
		if (_registeredHandlers[LPUART1_IRQn] != nullptr) {
			_registeredHandlers[LPUART1_IRQn]();
		}
	}

	void LPTIM1_IRQHandler(void) {
		if (_registeredHandlers[LPTIM1_IRQn] != nullptr) {
			_registeredHandlers[LPTIM1_IRQn]();
		}
	}

	void LPTIM2_IRQHandler(void) {
		if (_registeredHandlers[LPTIM2_IRQn] != nullptr) {
			_registeredHandlers[LPTIM2_IRQn]();
		}
	}

	void TIM15_IRQHandler(void) {
		if (_registeredHandlers[TIM15_IRQn] != nullptr) {
			_registeredHandlers[TIM15_IRQn]();
		}
	}

	void TIM16_IRQHandler(void) {
		if (_registeredHandlers[TIM16_IRQn] != nullptr) {
			_registeredHandlers[TIM16_IRQn]();
		}
	}

	void TIM17_IRQHandler(void) {
		if (_registeredHandlers[TIM17_IRQn] != nullptr) {
			_registeredHandlers[TIM17_IRQn]();
		}
	}

	void COMP_IRQHandler(void) {
		if (_registeredHandlers[COMP_IRQn] != nullptr) {
			_registeredHandlers[COMP_IRQn]();
		}
	}

	void USB_FS_IRQHandler(void) {
		if (_registeredHandlers[USB_FS_IRQn] != nullptr) {
			_registeredHandlers[USB_FS_IRQn]();
		}
	}

	void CRS_IRQHandler(void) {
		if (_registeredHandlers[CRS_IRQn] != nullptr) {
			_registeredHandlers[CRS_IRQn]();
		}
	}

	void FMC_IRQHandler(void) {
		if (_registeredHandlers[FMC_IRQn] != nullptr) {
			_registeredHandlers[FMC_IRQn]();
		}
	}

	void OCTOSPI1_IRQHandler(void) {
		if (_registeredHandlers[OCTOSPI1_IRQn] != nullptr) {
			_registeredHandlers[OCTOSPI1_IRQn]();
		}
	}

	void SDMMC1_IRQHandler(void) {
		if (_registeredHandlers[SDMMC1_IRQn] != nullptr) {
			_registeredHandlers[SDMMC1_IRQn]();
		}
	}

	void DMA2_Channel1_IRQHandler(void) {
		if (_registeredHandlers[DMA2_Channel1_IRQn] != nullptr) {
			_registeredHandlers[DMA2_Channel1_IRQn]();
		}
	}

	void DMA2_Channel2_IRQHandler(void) {
		if (_registeredHandlers[DMA2_Channel2_IRQn] != nullptr) {
			_registeredHandlers[DMA2_Channel2_IRQn]();
		}
	}

	void DMA2_Channel3_IRQHandler(void) {
		if (_registeredHandlers[DMA2_Channel3_IRQn] != nullptr) {
			_registeredHandlers[DMA2_Channel3_IRQn]();
		}
	}

	void DMA2_Channel4_IRQHandler(void) {
		if (_registeredHandlers[DMA2_Channel4_IRQn] != nullptr) {
			_registeredHandlers[DMA2_Channel4_IRQn]();
		}
	}

	void DMA2_Channel5_IRQHandler(void) {
		if (_registeredHandlers[DMA2_Channel5_IRQn] != nullptr) {
			_registeredHandlers[DMA2_Channel5_IRQn]();
		}
	}

	void DMA2_Channel6_IRQHandler(void) {
		if (_registeredHandlers[DMA2_Channel6_IRQn] != nullptr) {
			_registeredHandlers[DMA2_Channel6_IRQn]();
		}
	}

	void DMA2_Channel7_IRQHandler(void) {
		if (_registeredHandlers[DMA2_Channel7_IRQn] != nullptr) {
			_registeredHandlers[DMA2_Channel7_IRQn]();
		}
	}

	void DMA2_Channel8_IRQHandler(void) {
		if (_registeredHandlers[DMA2_Channel8_IRQn] != nullptr) {
			_registeredHandlers[DMA2_Channel8_IRQn]();
		}
	}

	void I2C3_EV_IRQHandler(void) {
		if (_registeredHandlers[I2C3_EV_IRQn] != nullptr) {
			_registeredHandlers[I2C3_EV_IRQn]();
		}
	}

	void I2C3_ER_IRQHandler(void) {
		if (_registeredHandlers[I2C3_ER_IRQn] != nullptr) {
			_registeredHandlers[I2C3_ER_IRQn]();
		}
	}

	void SAI1_IRQHandler(void) {
		if (_registeredHandlers[SAI1_IRQn] != nullptr) {
			_registeredHandlers[SAI1_IRQn]();
		}
	}

	void SAI2_IRQHandler(void) {
		if (_registeredHandlers[SAI2_IRQn] != nullptr) {
			_registeredHandlers[SAI2_IRQn]();
		}
	}

	void TSC_IRQHandler(void) {
		if (_registeredHandlers[TSC_IRQn] != nullptr) {
			_registeredHandlers[TSC_IRQn]();
		}
	}

	void RNG_IRQHandler(void) {
		if (_registeredHandlers[RNG_IRQn] != nullptr) {
			_registeredHandlers[RNG_IRQn]();
		}
	}

	void FPU_IRQHandler(void) {
		if (_registeredHandlers[FPU_IRQn] != nullptr) {
			_registeredHandlers[FPU_IRQn]();
		}
	}

	void HASH_IRQHandler(void) {
		if (_registeredHandlers[HASH_IRQn] != nullptr) {
			_registeredHandlers[HASH_IRQn]();
		}
	}

	void LPTIM3_IRQHandler(void) {
		if (_registeredHandlers[LPTIM3_IRQn] != nullptr) {
			_registeredHandlers[LPTIM3_IRQn]();
		}
	}

	void SPI3_IRQHandler(void) {
		if (_registeredHandlers[SPI3_IRQn] != nullptr) {
			_registeredHandlers[SPI3_IRQn]();
		}
	}

	void I2C4_ER_IRQHandler(void) {
		if (_registeredHandlers[I2C4_ER_IRQn] != nullptr) {
			_registeredHandlers[I2C4_ER_IRQn]();
		}
	}

	void I2C4_EV_IRQHandler(void) {
		if (_registeredHandlers[I2C4_EV_IRQn] != nullptr) {
			_registeredHandlers[I2C4_EV_IRQn]();
		}
	}

	void DFSDM1_FLT0_IRQHandler(void) {
		if (_registeredHandlers[DFSDM1_FLT0_IRQn] != nullptr) {
			_registeredHandlers[DFSDM1_FLT0_IRQn]();
		}
	}

	void DFSDM1_FLT1_IRQHandler(void) {
		if (_registeredHandlers[DFSDM1_FLT1_IRQn] != nullptr) {
			_registeredHandlers[DFSDM1_FLT1_IRQn]();
		}
	}

	void DFSDM1_FLT2_IRQHandler(void) {
		if (_registeredHandlers[DFSDM1_FLT2_IRQn] != nullptr) {
			_registeredHandlers[DFSDM1_FLT2_IRQn]();
		}
	}

	void DFSDM1_FLT3_IRQHandler(void) {
		if (_registeredHandlers[DFSDM1_FLT3_IRQn] != nullptr) {
			_registeredHandlers[DFSDM1_FLT3_IRQn]();
		}
	}

	void UCPD1_IRQHandler(void) {
		if (_registeredHandlers[UCPD1_IRQn] != nullptr) {
			_registeredHandlers[UCPD1_IRQn]();
		}
	}

	void ICACHE_IRQHandler(void) {
		if (_registeredHandlers[ICACHE_IRQn] != nullptr) {
			_registeredHandlers[ICACHE_IRQn]();
		}
	}

	__VECTOR_LOC IrqHandler_t g_pfnVectors[_ISR_VECTOR_TABLE_SIZE] = {
		reinterpret_cast<IrqHandler_t>(_estack),
		Reset_Handler,
		NMI_Handler,
		HardFault_Handler,
		MemManage_Handler,
		BusFault_Handler,
		UsageFault_Handler,
		SecureFault_Handler,
		nullptr,
		nullptr,
		nullptr,
		SVC_Handler,
		DebugMon_Handler,
		nullptr,
		PendSV_Handler,
		SysTick_Handler,
		WWDG_IRQHandler,
		PVD_PVM_IRQHandler,
		RTC_IRQHandler,
		RTC_S_IRQHandler,
		TAMP_IRQHandler,
		TAMP_S_IRQHandler,
		FLASH_IRQHandler,
		FLASH_S_IRQHandler,
		GTZC_IRQHandler,
		RCC_IRQHandler,
		RCC_S_IRQHandler,
		EXTI0_IRQHandler,
		EXTI1_IRQHandler,
		EXTI2_IRQHandler,
		EXTI3_IRQHandler,
		EXTI4_IRQHandler,
		EXTI5_IRQHandler,
		EXTI6_IRQHandler,
		EXTI7_IRQHandler,
		EXTI8_IRQHandler,
		EXTI9_IRQHandler,
		EXTI10_IRQHandler,
		EXTI11_IRQHandler,
		EXTI12_IRQHandler,
		EXTI13_IRQHandler,
		EXTI14_IRQHandler,
		EXTI15_IRQHandler,
		DMAMUX1_IRQHandler,
		DMAMUX1_S_IRQHandler,
		DMA1_Channel1_IRQHandler,
		DMA1_Channel2_IRQHandler,
		DMA1_Channel3_IRQHandler,
		DMA1_Channel4_IRQHandler,
		DMA1_Channel5_IRQHandler,
		DMA1_Channel6_IRQHandler,
		DMA1_Channel7_IRQHandler,
		DMA1_Channel8_IRQHandler,
		ADC1_2_IRQHandler,
		DAC_IRQHandler,
		FDCAN1_IT0_IRQHandler,
		FDCAN1_IT1_IRQHandler,
		TIM1_BRK_IRQHandler,
		TIM1_UP_IRQHandler,
		TIM1_TRG_COM_IRQHandler,
		TIM1_CC_IRQHandler,
		TIM2_IRQHandler,
		TIM3_IRQHandler,
		TIM4_IRQHandler,
		TIM5_IRQHandler,
		TIM6_IRQHandler,
		TIM7_IRQHandler,
		TIM8_BRK_IRQHandler,
		TIM8_UP_IRQHandler,
		TIM8_TRG_COM_IRQHandler,
		TIM8_CC_IRQHandler,
		I2C1_EV_IRQHandler,
		I2C1_ER_IRQHandler,
		I2C2_EV_IRQHandler,
		I2C2_ER_IRQHandler,
		SPI1_IRQHandler,
		SPI2_IRQHandler,
		USART1_IRQHandler,
		USART2_IRQHandler,
		USART3_IRQHandler,
		UART4_IRQHandler,
		UART5_IRQHandler,
		LPUART1_IRQHandler,
		LPTIM1_IRQHandler,
		LPTIM2_IRQHandler,
		TIM15_IRQHandler,
		TIM16_IRQHandler,
		TIM17_IRQHandler,
		COMP_IRQHandler,
		USB_FS_IRQHandler,
		CRS_IRQHandler,
		FMC_IRQHandler,
		OCTOSPI1_IRQHandler,
		nullptr,
		SDMMC1_IRQHandler,
		nullptr,
		DMA2_Channel1_IRQHandler,
		DMA2_Channel2_IRQHandler,
		DMA2_Channel3_IRQHandler,
		DMA2_Channel4_IRQHandler,
		DMA2_Channel5_IRQHandler,
		DMA2_Channel6_IRQHandler,
		DMA2_Channel7_IRQHandler,
		DMA2_Channel8_IRQHandler,
		I2C3_EV_IRQHandler,
		I2C3_ER_IRQHandler,
		SAI1_IRQHandler,
		SAI2_IRQHandler,
		TSC_IRQHandler,
		nullptr,
		RNG_IRQHandler,
		FPU_IRQHandler,
		HASH_IRQHandler,
		nullptr,
		LPTIM3_IRQHandler,
		SPI3_IRQHandler,
		I2C4_ER_IRQHandler,
		I2C4_EV_IRQHandler,
		DFSDM1_FLT0_IRQHandler,
		DFSDM1_FLT1_IRQHandler,
		DFSDM1_FLT2_IRQHandler,
		DFSDM1_FLT3_IRQHandler,
		UCPD1_IRQHandler,
		ICACHE_IRQHandler,
		nullptr
	};
}
