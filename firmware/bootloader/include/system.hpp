#pragma once

#include <cstdint>

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

void SystemInit();
void SystemDeinit();

using TickType = uint64_t;

static constexpr TickType usToTicks(uint32_t us) {
	return static_cast<TickType>((us) * (SYSCLK_HZ / 1'000'000UL));
}

static constexpr TickType msToTicks(uint32_t ms) {
	return static_cast<TickType>((ms) * (SYSCLK_HZ / 1'000UL));
}

void Sleep(TickType ticks);
