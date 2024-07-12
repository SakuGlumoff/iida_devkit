#pragma once

#include <cstdint>

class Crc16 {
	public:
	void Update(uint8_t const byte) {
		uint32_t crc = _value;
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
		_value = crc & 0xFFFFU;
	}

	uint16_t Value() const {
		return _value;
	}

	private:
	uint16_t _value = 0;
};
