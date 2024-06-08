#pragma once

#include <cstdint>

/**
 * @brief Header for the application image.
 */
struct __attribute__((aligned(4))) AppHeader {
	uint32_t id;
	uint32_t size;
	uint32_t crc;
	uint32_t reserved;
};

/**
 * @brief Calculate the 32-bit CRC value of the application image.
 *
 * @param[in] appSize The size of the application image in bytes.
 *
 * @return The calculated 32-bit CRC value.
 */
uint32_t CalculateAppCrc32(uint32_t const appSize);

/**
 * @brief Start the application image.
 */
void StartApplication();
