#pragma once

#include <cstdint>

static constexpr uint16_t
UpdateCrc16(uint16_t const crcIn, uint8_t const byte) {
	uint32_t crc = crcIn;
	uint32_t in  = byte | 0x100;
	do {
		crc <<= 1;
		in <<= 1;
		if (in & 0x100) {
			++crc;
		}
		if (crc & 0x10000) {
			crc ^= 0x1021;
		}
	} while (!(in & 0x10000));
	return crc & 0xFFFFU;
}
