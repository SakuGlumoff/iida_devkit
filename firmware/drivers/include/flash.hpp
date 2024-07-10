#pragma once

#include "errno.hpp"

#include <cstdint>

namespace Flash {
	enum Error {
		ERROR_UNSPECIFIED = 1,
		ERROR_BAD_PARAMETER,
	};

	/**
	 * @brief Erase a region of flash memory.
	 *
	 * @param[in] address The start address of the region to erase.
	 * @param[in] size    The size of the region to erase.
	 *
	 * @return ERROR_NONE if succesful, an error code of Flash::Error
	 *         otherwise.
	 */
	error_code_t Erase(uint32_t const address, uint32_t size);

	/**
	 * @brief Read data from a region of flash memory.
	 *
	 * @param[in]  address The start address of the region to read.
	 * @param[out] data    The buffer to store the read data.
	 * @param[in]  size    The size of data to read.
	 *
	 * @return ERROR_NONE if succesful, an error code of Flash::Error
	 *         otherwise.
	 */
	error_code_t Read(uint32_t const address, uint8_t* data, uint32_t size);

	/**
	 * @brief Write data to a region of flash memory.
	 *
	 * @param[in] address The start address of the region to write to.
	 * @param[in] data    The data to write to the region.
	 * @param[in] size    The size of data to write.
	 *
	 * @return ERROR_NONE if succesful, an error code of Flash::Error
	 *         otherwise.
	 */
	error_code_t
	Write(uint32_t const address, uint8_t const* data, uint32_t size);
}; // namespace Flash
