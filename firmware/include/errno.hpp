#pragma once

#include <cstdint>

/**
 * @brief Common error code.
 *        Positive or zero if no error.
 *        Negative if there is an error.
 *
 * @note Errors are module specific and thus should be cast to module specific
 *       error codes.
 */
using error_code_t = int32_t;

constexpr error_code_t ERROR_NONE = 0;
