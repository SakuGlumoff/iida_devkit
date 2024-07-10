#include "flash.hpp"

#include "memorymap.hpp"
#include "stm32l552xx.h"

#include <cstdint>
#include <cstring>

namespace Flash {
	constexpr uint32_t _FLASH_KEY1      = 0x45670123UL;
	constexpr uint32_t _FLASH_KEY2      = 0xCDEF89ABUL;
	constexpr uint32_t _FLASH_PAGE_SIZE = 1'024UL * 4UL;

	inline void _Unlock() {
		FLASH->NSKEYR = _FLASH_KEY1;
		FLASH->NSKEYR = _FLASH_KEY2;
	}

	inline void _Lock() {
		FLASH->NSCR |= FLASH_NSCR_NSLOCK;
	}

	inline void _WaitBusy() {
		while (FLASH->NSSR & FLASH_NSSR_NSBSY) {
			// Wait for the flash to not be busy.
			__NOP();
		}
	}

	inline void _ClearFlags() {
		FLASH->NSSR |=
		    (FLASH_NSSR_OPTWERR | FLASH_NSSR_NSPGSERR
		     | FLASH_NSSR_NSSIZERR | FLASH_NSSR_NSPGAERR
		     | FLASH_NSSR_NSWRPERR | FLASH_NSSR_NSPROGERR
		     | FLASH_NSSR_NSOPERR | FLASH_NSSR_NSEOP);
	}

	inline constexpr uint32_t _GetPage(uint32_t const address) {
		return address / _FLASH_PAGE_SIZE;
	}

	inline void _WriteDword(uint32_t const address, uint64_t const data) {
		_Unlock();

		_WaitBusy();
		_ClearFlags();
		FLASH->NSCR |= FLASH_NSCR_NSPG;
		*reinterpret_cast<uint64_t*>(address) = data;
		_WaitBusy();
		while (FLASH->NSSR & FLASH_NSSR_NSEOP) {
			__NOP();
		}
		FLASH->NSCR &= ~FLASH_NSCR_NSPG;

		_Lock();
	}

	error_code_t Erase(uint32_t const address, uint32_t size) {
		_Unlock();

		uint32_t const startPage = _GetPage(address);
		uint32_t const endPage   = _GetPage(address + size);
		for (uint32_t i = startPage; i < endPage; i++) {
			_WaitBusy();
			_ClearFlags();
			// Set to page erase mode.
			FLASH->NSCR |= FLASH_NSCR_NSPER;
			// Set the correct page number.
			FLASH->NSCR &= ~FLASH_NSCR_NSPNB;
			FLASH->NSCR |= (i << FLASH_NSCR_NSPNB_Pos);
			// Start erase operation.
			FLASH->NSCR |= FLASH_NSCR_NSSTRT;
			_WaitBusy();
			// Clear the PER bit to prevent accidents.
			FLASH->NSCR &= ~FLASH_NSCR_NSPER;
		}

		_Lock();

		return ERROR_NONE;
	}

	error_code_t
	Read(uint32_t const address, uint8_t* data, uint32_t size) {
		if (data == nullptr) {
			return -ERROR_BAD_PARAMETER;
		}
		for (uint32_t i = 0; i < size; i++) {
			data[i] = *reinterpret_cast<uint8_t*>(address + i);
		}
		return ERROR_NONE;
	}

	error_code_t
	Write(uint32_t const address, uint8_t const* data, uint32_t size) {
		if (data == nullptr) {
			return -ERROR_BAD_PARAMETER;
		}
		error_code_t err       = ERROR_NONE;
		uint32_t     startPage = _GetPage(address);
		uint32_t     endPage   = _GetPage(address + size);
		uint32_t     written   = 0UL;
		for (uint32_t i = startPage; i < endPage; i++) {
			uint32_t currentPageAddress = i * _FLASH_PAGE_SIZE;

			// Read the current page into a buffer.
			uint8_t buffer[_FLASH_PAGE_SIZE];
			err =
			    Read(currentPageAddress, buffer, _FLASH_PAGE_SIZE);
			if (err != ERROR_NONE) {
				return err;
			}

			// Erase that page
			err = Erase(currentPageAddress, _FLASH_PAGE_SIZE);
			if (err != ERROR_NONE) {
				return err;
			}

			// Modify that page

			// Write that page
		}
	}
}; // namespace Flash
