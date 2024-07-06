#include "system.hpp"

#include "stm32l552xx.h"

static TickType volatile _systickCounter = 0;

static void _initPwr() {
	RCC->APB1ENR1 |= RCC_APB1ENR1_PWREN;
	PWR->CR1 &= ~PWR_CR1_VOS;
	PWR->CR1 |= (0UL << PWR_CR1_VOS_Pos);
	PWR->CR1 |= PWR_CR1_DBP;
}

static void _deinitPwr() {
	RCC->BDCR = 0x00000000UL;
	PWR->CR1  = 0x00000400UL;
}

static void _initClocks() {
#ifdef CONFIG_HSE
	// Enable HSE.
	RCC->CR |= RCC_CR_HSEON;
	while (!(RCC->CR & RCC_CR_HSERDY)) {}
#else
	// Enable HSI.
	RCC->CR |= RCC_CR_HSION;
	while (!(RCC->CR & RCC_CR_HSIRDY)) {}
#endif

	// Enable LSI.
	RCC->CSR |= RCC_CSR_LSION;
	while (!(RCC->CSR & RCC_CSR_LSIRDY)) {}

	// Configure AHB clock.
	RCC->CFGR &= ~(RCC_CFGR_HPRE | RCC_CFGR_PPRE1 | RCC_CFGR_PPRE2);
	// NOTE: HPRE has to be set to DIV2 before enabling PLL as clock source
	// if PLL clock exceeds 80 MHz.
	RCC->CFGR |=
	    ((static_cast<uint32_t>(HPREDiv::DIV2) << RCC_CFGR_HPRE_Pos)
	     | (static_cast<uint32_t>(PPRE1) << RCC_CFGR_PPRE1_Pos)
	     | (static_cast<uint32_t>(PPRE2) << RCC_CFGR_PPRE2_Pos));

	// Configure PLLSAI1
	RCC->CR &= ~RCC_CR_PLLSAI1ON;
	while (RCC->CR & RCC_CR_PLLSAI1RDY) {}

	static_assert(PLLSAI1_M >= 1UL && PLLSAI1_M <= 8UL);
	static_assert(PLLSAI1_N >= 8UL && PLLSAI1_N <= 86UL);
	RCC->PLLSAI1CFGR &=
	    ~(RCC_PLLSAI1CFGR_PLLSAI1SRC | RCC_PLLSAI1CFGR_PLLSAI1M
	      | RCC_PLLSAI1CFGR_PLLSAI1N | RCC_PLLSAI1CFGR_PLLSAI1P
	      | RCC_PLLSAI1CFGR_PLLSAI1Q | RCC_PLLSAI1CFGR_PLLSAI1R);
	RCC->PLLSAI1CFGR |= ((PLLSAI1_M - 1UL) << RCC_PLLSAI1CFGR_PLLSAI1M_Pos);
	RCC->PLLSAI1CFGR |= (PLLSAI1_N << RCC_PLLSAI1CFGR_PLLSAI1N_Pos);
	RCC->PLLSAI1CFGR |=
	    (static_cast<uint32_t>(PLLSAI1_P) << RCC_PLLSAI1CFGR_PLLSAI1P_Pos);
	RCC->PLLSAI1CFGR |=
	    (static_cast<uint32_t>(PLLSAI1_Q) << RCC_PLLSAI1CFGR_PLLSAI1Q_Pos);
	RCC->PLLSAI1CFGR |=
	    (static_cast<uint32_t>(PLLSAI1_R) << RCC_PLLSAI1CFGR_PLLSAI1R_Pos);
#ifdef CONFIG_HSE
	// HSE as source.
	RCC->PLLSAI1CFGR |= (3UL << RCC_PLLSAI1CFGR_PLLSAI1SRC_Pos);
#else
	// HSI as source.
	RCC->PLLSAI1CFGR |= (2UL << RCC_PLLSAI1CFGR_PLLSAI1SRC_Pos);
#endif
	RCC->PLLSAI1CFGR |=
	    (RCC_PLLSAI1CFGR_PLLSAI1PEN | RCC_PLLSAI1CFGR_PLLSAI1QEN
	     | RCC_PLLSAI1CFGR_PLLSAI1REN);

	RCC->CR |= RCC_CR_PLLSAI1ON;
	while (!(RCC->CR & RCC_CR_PLLSAI1RDY)) {}

	// Configure PLLSAI2
	RCC->CR &= ~RCC_CR_PLLSAI2ON;
	while (RCC->CR & RCC_CR_PLLSAI2RDY) {}

	RCC->PLLSAI2CFGR &=
	    ~(RCC_PLLSAI2CFGR_PLLSAI2SRC | RCC_PLLSAI2CFGR_PLLSAI2M
	      | RCC_PLLSAI2CFGR_PLLSAI2N | RCC_PLLSAI2CFGR_PLLSAI2P);
	RCC->PLLSAI2CFGR |= ((PLLSAI2_M - 1UL) << RCC_PLLSAI2CFGR_PLLSAI2M_Pos);
	RCC->PLLSAI2CFGR |= (PLLSAI2_N << RCC_PLLSAI2CFGR_PLLSAI2N_Pos);
	RCC->PLLSAI2CFGR |=
	    (static_cast<uint32_t>(PLLSAI2_P) << RCC_PLLSAI2CFGR_PLLSAI2P_Pos);
#ifdef CONFIG_HSE
	// HSE as source.
	RCC->PLLSAI2CFGR |= (3UL << RCC_PLLSAI2CFGR_PLLSAI2SRC_Pos);
#else
	// HSI as source.
	RCC->PLLSAI2CFGR |= (2UL << RCC_PLLSAI2CFGR_PLLSAI2SRC_Pos);
#endif
	RCC->PLLSAI2CFGR |= (RCC_PLLSAI2CFGR_PLLSAI2PEN);

	RCC->CR |= RCC_CR_PLLSAI2ON;
	while (!(RCC->CR & RCC_CR_PLLSAI2RDY)) {}

	// Configure PLL
	RCC->CR &= ~RCC_CR_PLLON;
	while (RCC->CR & RCC_CR_PLLRDY) {}

	static_assert(PLL_M >= 1UL && PLL_M <= 8UL);
	static_assert(PLL_N >= 8UL && PLL_N <= 86UL);
	RCC->PLLCFGR &=
	    ~(RCC_PLLCFGR_PLLM | RCC_PLLCFGR_PLLN | RCC_PLLCFGR_PLLP
	      | RCC_PLLCFGR_PLLQ | RCC_PLLCFGR_PLLR | RCC_PLLCFGR_PLLSRC);
	RCC->PLLCFGR |= ((PLL_M - 1UL) << RCC_PLLCFGR_PLLM_Pos);
	RCC->PLLCFGR |= (PLL_N << RCC_PLLCFGR_PLLN_Pos);
	RCC->PLLCFGR |= (static_cast<uint32_t>(PLL_P) << RCC_PLLCFGR_PLLP_Pos);
	RCC->PLLCFGR |= (static_cast<uint32_t>(PLL_Q) << RCC_PLLCFGR_PLLQ_Pos);
	RCC->PLLCFGR |= (static_cast<uint32_t>(PLL_R) << RCC_PLLCFGR_PLLR_Pos);

#ifdef CONFIG_HSE
	// HSE as source.
	RCC->PLLCFGR |= (3UL << RCC_PLLCFGR_PLLSRC_Pos);
#else
	// HSI as source.
	RCC->PLLCFGR |= (2UL << RCC_PLLCFGR_PLLSRC_Pos);
#endif

	RCC->PLLCFGR |=
	    (RCC_PLLCFGR_PLLPEN | RCC_PLLCFGR_PLLQEN | RCC_PLLCFGR_PLLREN);

	RCC->CR |= RCC_CR_PLLON;
	while (!(RCC->CR & RCC_CR_PLLRDY)) {}

	// Select PLL as system clock source.
	RCC->CFGR |= (3UL << RCC_CFGR_SW_Pos);
	while (!(RCC->CFGR & (3UL << RCC_CFGR_SWS_Pos))) {}
	uint32_t volatile tmp = RCC->CFGR;
	tmp &= ~RCC_CFGR_HPRE;
	tmp |= static_cast<uint32_t>(HPRE);
	RCC->CFGR = tmp;

#ifdef CONFIG_LSE
	RCC->BDCR &= ~(RCC_BDCR_LSEDRV | RCC_BDCR_LSESYSEN);
	// Set high drive capability for LSE.
	RCC->BDCR |= (3UL << RCC_BDCR_LSEDRV_Pos);
	// Enable LSE.
	RCC->BDCR |= RCC_BDCR_LSEON;
	while (!(RCC->BDCR & RCC_BDCR_LSERDY)) {}
#endif
}

static void _deinitClocks() {
	RCC->CR          = 0x00000063UL;
	RCC->CFGR        = 0x00000000UL;
	RCC->PLLCFGR     = 0x00001000UL;
	RCC->PLLSAI1CFGR = 0x00001000UL;
	RCC->PLLSAI2CFGR = 0x00001000UL;
	RCC->APB1ENR1    = 0x00000000UL;
}

static void _initFlash() {
	FLASH->ACR &= ~FLASH_ACR_LATENCY;
	FLASH->ACR |= FLASH_ACR_LATENCY_4WS;
}

static void _deinitFlash() {
	FLASH->ACR = 0x00000000UL;
}

static void _initSysTick() {
	SysTick->LOAD = (SYSCLK_HZ / 1'000UL) - 1UL;
	SysTick->VAL  = 0UL;
	SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk | SysTick_CTRL_TICKINT_Msk
	              | SysTick_CTRL_ENABLE_Msk;
	__NVIC_EnableIRQ(SysTick_IRQn);
}

static void _deinitSysTick() {
	SysTick->CTRL = 0UL;
	SysTick->LOAD = 0UL;
	SysTick->VAL  = 0UL;
	__NVIC_DisableIRQ(SysTick_IRQn);
}

TickType GetTicks() {
	return _systickCounter;
}

void Sleep(TickType ticks) {
	TickType end = _systickCounter + ticks;
	while (_systickCounter < end) {
		__NOP();
	}
}

void SystemInit() {
#if (__FPU_PRESENT == 1) && (__FPU_USED == 1)
	// Set CP10 and CP11 full access.
	SCB->CPACR |= ((3UL << 20U) | (3UL << 22U));
#endif
	_systickCounter = 0;
	_initFlash();
	_initPwr();
	_initClocks();
	_initSysTick();
}

void SystemDeinit() {
	_deinitClocks();
	_deinitPwr();
	_deinitFlash();
	_deinitSysTick();
	SCB->CPACR &= ~((3UL << 20U) | (3UL << 22U));
}

extern "C" {
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
		_systickCounter++;
	}
}
