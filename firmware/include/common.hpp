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
