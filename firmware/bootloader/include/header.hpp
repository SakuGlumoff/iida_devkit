#pragma once

#include "memorymap.hpp"

#include <cstdint>

constexpr uint32_t TIMESTAMP_SIZE      = 25UL;
constexpr uint32_t COMMIT_ID_SIZE      = 8UL;
constexpr uint32_t TOOLCHAIN_INFO_SIZE = 100UL;

/**
 * @brief Header for the bootloader image.
 */
struct __attribute__((aligned(8))) __attribute__((packed)) BootloaderHeader {
	char const timestamp[TIMESTAMP_SIZE];
	char const commit[COMMIT_ID_SIZE];
	char const toolchain[TOOLCHAIN_INFO_SIZE];
};
