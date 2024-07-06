#include "ymodem.hpp"

#include "errno.hpp"
#include "pinmap.hpp"
#include "uart.hpp"

#include <cstdint>
#include <cstring>

enum ControlCharacter : uint8_t {
	CTRL_SOH    = 0x01, // Start of Header
	CTRL_STX    = 0x02, // Start of Text
	CTRL_EOT    = 0x04, // End of Transmission
	CTRL_ACK    = 0x06, // Acknowledge
	CTRL_NAK    = 0x15, // Negative Acknowledge
	CTRL_CANCEL = 0x18, // Cancel
	CTRL_ABORT  = 0x41, // Abort
	CTRL_CRC    = 0x43, // Cyclic Redundancy Check
};

static constexpr uint32_t PACKET_SIZE    = 128UL;
static constexpr uint32_t PACKET_1K_SIZE = 1'024UL;

enum ReceiveResult : int {
	RECEIVE_RESULT_OK = 0,
	RECEIVE_RESULT_ERROR,
	RECEIVE_RESULT_CANCELED,
	RECEIVE_RESULT_ABORTED,
};

static ReceiveResult
_ReceivePacket(Uart& uart, uint8_t* data, uint32_t* len, TickType timeout) {
	uint32_t packetSize = 0U;
	uint8_t  c          = 0U;
	*len                = 0UL;

	error_code_t ret = uart.Getc(c, timeout);
	if (ret != ERROR_NONE) {
		return RECEIVE_RESULT_ERROR;
	}

	switch (c) {
		case CTRL_SOH:
			packetSize = PACKET_SIZE;
			break;
		case CTRL_STX:
			packetSize = PACKET_1K_SIZE;
			break;
		case CTRL_EOT:
			return RECEIVE_RESULT_OK;
		case CTRL_CANCEL:
			ret = uart.Getc(c, timeout);
			if (ret != ERROR_NONE) {
				return RECEIVE_RESULT_ERROR;
			}
			if (c == CTRL_CANCEL) {
				return RECEIVE_RESULT_CANCELED;
			}
			*len = 0UL;
			return RECEIVE_RESULT_ERROR;
		case CTRL_ABORT:
			return RECEIVE_RESULT_ABORTED;
		default:
			return RECEIVE_RESULT_ERROR;
	}

	*data = c;
	for (uint32_t i = 1; i < packetSize; i++) {
		ret = uart.Getc(c, timeout);
		if (ret != ERROR_NONE) {
			return RECEIVE_RESULT_ERROR;
		}
		data[i] = c;
	}

	if (data[1] != static_cast<uint8_t>(~data[2])) {
		return RECEIVE_RESULT_ERROR;
	}

	*len = packetSize;

	return RECEIVE_RESULT_OK;
}

bool YModem::ReceiveNewAppImage() {
	bool ret = false;

	Uart dbg_uart(LPUART1, DBG_UART_TX, DBG_UART_RX);

	char const* const msg = "Upload image using YModem protocol.\n>";
	for (size_t i = 0; i < strlen(msg); i++) {
		dbg_uart.Putc(static_cast<uint8_t>(msg[i]), msToTicks(1'000UL));
	}

	uint8_t  packetData[PACKET_1K_SIZE] = {0};
	uint32_t packetsReceived            = 0UL;
	uint32_t len                        = 0UL;
	uint32_t errors                     = 0UL;

	while (true) {
		memset(packetData, 0, PACKET_1K_SIZE);
		DBG_PRINTF_DEBUG("Waiting for packet...");
		ReceiveResult result = _ReceivePacket(
		    dbg_uart,
		    packetData,
		    &len,
		    msToTicks(1'000UL)
		);
		if (result == RECEIVE_RESULT_OK) {
			DBG_PRINTF_DEBUG("Packet received: %u", len);
			errors = 0UL;
			dbg_uart.Putc(CTRL_ACK, msToTicks(1'000UL));
			if (len == 0UL) {
				ret = true;
				break;
			}
			if ((packetData[1] & 0xFFU)
			    != (packetsReceived & 0xFFU)) {
				dbg_uart.Putc(CTRL_NAK, msToTicks(1'000UL));
				continue;
			}
			// TODO: Process packetData
			// ProcessPacket(packetData, len);
			packetsReceived++;
		} else if (result == RECEIVE_RESULT_ABORTED) {
			DBG_PRINTF_DEBUG("ABORTED");
			dbg_uart.Putc(CTRL_ACK, msToTicks(1'000UL));
			dbg_uart.Putc(CTRL_ACK, msToTicks(1'000UL));
			ret = false;
			break;
		} else if (result == RECEIVE_RESULT_CANCELED) {
			DBG_PRINTF_DEBUG("CANCELED");
			dbg_uart.Putc(CTRL_CANCEL, msToTicks(1'000UL));
			dbg_uart.Putc(CTRL_CANCEL, msToTicks(1'000UL));
			ret = false;
			break;
		} else {
			errors++;
			if (errors > 10UL) {
				DBG_PRINTF_DEBUG("Too many errors. CANCELED");
				dbg_uart.Putc(CTRL_CANCEL, msToTicks(1'000UL));
				dbg_uart.Putc(CTRL_CANCEL, msToTicks(1'000UL));
				ret = false;
				break;
			}
			DBG_PRINTF_DEBUG("NAKED");
			dbg_uart.Putc(CTRL_NAK, msToTicks(1'000UL));
		}
	}

	return ret;
}
