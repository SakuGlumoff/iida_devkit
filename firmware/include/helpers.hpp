#pragma once

/**
 * @brief Get the largest of two parameters.
 *
 * @param[in] a Option A.
 * @param[in] b Option B.
 */
#define MAX(a, b) ((a) > (b) ? (a) : (b))

/**
 * @brief Get the smallest of two parameters.
 *
 * @param[in] a Option A.
 * @param[in] b Option B.
 */
#define MIN(a, b) ((a) < (b) ? (a) : (b))

/**
 * @brief Calculate a number where a bit is set.
 *
 * @param[in] index Index of the bit.
 */
#define BIT(index) (1 << index)

/**
 * @brief Get the length of a C-style array.
 *
 * @param[in] arr The array you wish to get the length of.
 */
#define ARRAY_LEN(arr) (sizeof(arr) / sizeof(arr[0]))
