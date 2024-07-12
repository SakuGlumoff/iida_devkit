#pragma once

#include <cstdint>

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
