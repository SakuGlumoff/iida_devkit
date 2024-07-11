#pragma once

namespace SelfTest {
	/**
	 * @brief Run a self-test on the flash memory.
	 *
	 * @return True if successful, false otherwise.
	 */
	bool FlashTest();

	/**
	 * @brief Run all self-tests.
	 *
	 * @return True if all tests pass, false otherwise.
	 */
	bool RunAll();
}; // namespace SelfTest
