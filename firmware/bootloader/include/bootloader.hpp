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
 * @brief Calculate the 16-bit CRC value of the application image.
 *
 * @param[in] startAddress The start address of the application image.
 * @param[in] size         The size of the application image.
 *
 * @return The calculated 16-bit CRC value.
 */
uint16_t CalculateAppCrc16(uint32_t startAddress, uint32_t size);

/**
 * @brief Start the application image.
 */
void StartApplication();
