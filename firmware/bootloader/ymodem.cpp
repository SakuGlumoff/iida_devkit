#include "ymodem.hpp"

#include <cstdint>

static void _uart_rx_cb(uint8_t* data, uint32_t len) {
	if (data == nullptr) {
		return;
	}
	if (len == 0UL) {
		return;
	}
}

bool YModem::ReceiveNewAppImage() {
	bool ret = false;

	while (true) {}

	return ret;
}
