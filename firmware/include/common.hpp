#pragma once

#include "memorymap.hpp"

#include <cstdint>

constexpr uint32_t TIMESTAMP_SIZE      = 25UL;
constexpr uint32_t COMMIT_ID_SIZE      = 8UL;
constexpr uint32_t TOOLCHAIN_INFO_SIZE = 100UL;

/**
 * @brief Header for metadata.
 */
struct __attribute__((aligned(4))) __attribute__((packed)) MetadataHeader {
	char const timestamp[TIMESTAMP_SIZE];
	char const commit[COMMIT_ID_SIZE];
	char const toolchain[TOOLCHAIN_INFO_SIZE];
};

/**
 * @brief Header for the application image.
 */
struct __attribute__((aligned(4))) __attribute__((packed)) AppHeader {
	uint32_t       id;
	uint32_t       size;
	uint32_t       crc;
	MetadataHeader metadata;
};
