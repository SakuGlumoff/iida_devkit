#include "common.hpp"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-variable"
/**
 * @brief Application header used by the bootloader to verify the image.
 *
 * @note The header is populated and modified in post-build steps.
 */
__attribute__((section(".app_header"))) static AppHeader appHeader = {
	// ID
	0xAABBCCDDUL,
	// Size
	0UL,
	// CRC
	0UL,
	// Reserved
	0UL,
};
#pragma GCC diagnostic pop
