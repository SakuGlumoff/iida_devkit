#include "stm32l552xx.h"

enum class PLLxPDiv : uint32_t {
	DIV7  = 0UL,
	DIV17 = 1UL,
};

enum class PLLxQDiv : uint32_t {
	DIV2 = 0UL,
	DIV4,
	DIV6,
	DIV8,
};

enum class PLLxRDiv : uint32_t {
	DIV2 = 0UL,
	DIV4,
	DIV6,
	DIV8,
};

enum class HPREDiv : uint32_t {
	DIV1 = 0UL,
	DIV2 = 0b1000UL,
	DIV4,
	DIV8,
	DIV16,
	DIV64,
	DIV128,
	DIV256,
	DIV512,
};

enum class PPRExDiv : uint32_t {
	DIV1 = 0UL,
	DIV2 = 0b100UL,
	DIV4,
	DIV8,
	DIV16,
};

static constexpr uint32_t LSE_HZ     = 32'768UL;
static constexpr uint32_t LSI_HZ     = 32'000UL;
static constexpr uint32_t HSE_HZ     = 16'000'000UL;
static constexpr uint32_t HSI_HZ     = 16'000'000UL;
static constexpr uint32_t MSI_HZ     = 16'000'000UL;
static constexpr uint32_t PLL_CLK_HZ = 100'000'000UL;
static constexpr uint32_t SYSCLK_HZ  = PLL_CLK_HZ;

static constexpr uint32_t PLL_M = 2UL;
static constexpr uint32_t PLL_N = 25UL;
static constexpr PLLxPDiv PLL_P = PLLxPDiv::DIV7;
static constexpr PLLxQDiv PLL_Q = PLLxQDiv::DIV2;
static constexpr PLLxRDiv PLL_R = PLLxRDiv::DIV2;

static constexpr uint32_t PLLSAI1_M = 4UL;
static constexpr uint32_t PLLSAI1_N = 24UL;
static constexpr PLLxPDiv PLLSAI1_P = PLLxPDiv::DIV7;
static constexpr PLLxQDiv PLLSAI1_Q = PLLxQDiv::DIV2;
static constexpr PLLxRDiv PLLSAI1_R = PLLxRDiv::DIV2;

static constexpr uint32_t PLLSAI2_M = 1UL;
static constexpr uint32_t PLLSAI2_N = 8UL;
static constexpr PLLxPDiv PLLSAI2_P = PLLxPDiv::DIV7;

static constexpr HPREDiv  HPRE  = HPREDiv::DIV1;
static constexpr PPRExDiv PPRE1 = PPRExDiv::DIV1;
static constexpr PPRExDiv PPRE2 = PPRExDiv::DIV1;

static void _initPwr() {
	RCC->APB1ENR1 |= RCC_APB1ENR1_PWREN;
	PWR->CR1 &= ~PWR_CR1_VOS;
	PWR->CR1 |= (0UL << PWR_CR1_VOS_Pos);
	PWR->CR1 |= PWR_CR1_DBP;
}

static void _initClocks() {
	// Enable HSE.
	RCC->CR |= RCC_CR_HSEON;
	while (!(RCC->CR & RCC_CR_HSERDY)) {}

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
	// HSE as source.
	RCC->PLLSAI1CFGR |= (3UL << RCC_PLLSAI1CFGR_PLLSAI1SRC_Pos);
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
	// HSE as source.
	RCC->PLLSAI2CFGR |= (3UL << RCC_PLLSAI2CFGR_PLLSAI2SRC_Pos);
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
	// HSE as source.
	RCC->PLLCFGR |= (3UL << RCC_PLLCFGR_PLLSRC_Pos);
	RCC->PLLCFGR |=
	    (RCC_PLLCFGR_PLLPEN | RCC_PLLCFGR_PLLQEN | RCC_PLLCFGR_PLLREN);

	RCC->CR |= RCC_CR_PLLON;
	while (!(RCC->CR & RCC_CR_PLLRDY)) {}

	// Select PLL as system clock source.
	RCC->CFGR |= (3UL << RCC_CFGR_SW_Pos);
	while (!(RCC->CFGR & (3UL << RCC_CFGR_SWS_Pos))) {}
	uint32_t tmp = RCC->CFGR;
	tmp &= ~RCC_CFGR_HPRE;
	tmp |= static_cast<uint32_t>(HPRE);
	RCC->CFGR = tmp;

	// Enable LSI.
	RCC->CSR |= RCC_CSR_LSION;
	while (!(RCC->CSR & RCC_CSR_LSIRDY)) {}

	RCC->BDCR &= ~(RCC_BDCR_LSEDRV | RCC_BDCR_LSESYSEN);
	// Set high drive capability for LSE.
	RCC->BDCR |= (3UL << RCC_BDCR_LSEDRV_Pos);
	// Enable LSE.
	RCC->BDCR |= RCC_BDCR_LSEON;
	while (!(RCC->BDCR & RCC_BDCR_LSERDY)) {}
}

static void _initFlash() {
	FLASH->ACR &= ~FLASH_ACR_LATENCY;
	FLASH->ACR |= FLASH_ACR_LATENCY_4WS;
}

extern "C" {
	void SystemInit(void) {
		_initPwr();
		_initClocks();
		_initFlash();
#if (__FPU_PRESENT == 1) && (__FPU_USED == 1)
		// Set CP10 and CP11 full access.
		SCB->CPACR |= ((3UL << 20U) | (3UL << 22U));
#endif
	}

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
