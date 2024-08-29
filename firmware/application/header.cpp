#include "common.hpp"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-variable"
/**
 * @brief Application header used by the bootloader to verify the image.
 *
 * @note The header is populated and modified in post-build steps.
 */
__attribute__((section(".app_header"))) static AppHeader appHeader = {};
#pragma GCC diagnostic pop
