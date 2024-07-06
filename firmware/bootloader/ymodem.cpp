#include "ymodem.hpp"

#include "errno.hpp"
#include "pinmap.hpp"
#include "uart.hpp"

#include <cstdint>
#include <cstring>

enum ControlCharacter : uint8_t {
	CTRL_SOH    = 1,  // Start of Header
	CTRL_EOT    = 4,  // End of Transmission
	CTRL_ACK    = 6,  // Acknowledge
	CTRL_NAK    = 21, // Negative Acknowledge
	CTRL_CANCEL = 24, // Cancel
	CTRL_ABORT  = 65, // Abort
	CTRL_CRC    = 67, // Cyclic Redundancy Check
};

static constexpr uint32_t CONTROL_SIZE  = 1UL;
static constexpr uint32_t BLOCK_ID_SIZE = 2UL;
static constexpr uint32_t DATA_SIZE     = 128UL;
static constexpr uint32_t DATA_1K_SIZE  = 1'024UL;
static constexpr uint32_t CRC_SIZE      = 2UL;
static constexpr uint32_t PACKET_SIZE =
    CONTROL_SIZE + BLOCK_ID_SIZE + DATA_SIZE + CRC_SIZE;
static constexpr uint32_t PACKET_1K_SIZE =
    CONTROL_SIZE + BLOCK_ID_SIZE + DATA_1K_SIZE + CRC_SIZE;
static constexpr TickType TIMEOUT = msToTicks(1'000UL * 10UL);

static constexpr uint16_t
_UpdateCrc16(uint16_t const crcIn, uint8_t const byte) {
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
		ret = uart.Getc(c, 1'000UL);
		if (ret != ERROR_NONE) {
			return RECEIVE_RESULT_ERROR;
		}
		data[i] = c;
	}

	if (data[1] != static_cast<uint8_t>(~data[2])) {
		return RECEIVE_RESULT_ERROR;
	}

	*len = packetSize;

	uint16_t crcValue = (data[packetSize - 2] << 8) | data[packetSize - 1];
	uint32_t crc      = 0U;
	uint32_t i        = (CONTROL_SIZE + BLOCK_ID_SIZE);
	for (; i < (packetSize - CRC_SIZE); i++) {
		crc = _UpdateCrc16(crc, data[i]);
	}
	crc = _UpdateCrc16(crc, 0U);
	crc = _UpdateCrc16(crc, 0U);
	crc &= 0xFFFFU;
	if (static_cast<uint16_t>(crc) != crcValue) {
		return RECEIVE_RESULT_ERROR;
	}

	return RECEIVE_RESULT_OK;
}

bool YModem::ReceiveNewAppImage() {
	bool ret = false;

	Uart dbg_uart(LPUART1, DBG_UART_TX, DBG_UART_RX);

	char const* const greetingMsg =
	    "\r\nUpload image using YModem protocol.\r\n>";
	for (size_t i = 0; i < strlen(greetingMsg); i++) {
		dbg_uart.Putc(
		    static_cast<uint8_t>(greetingMsg[i]),
		    msToTicks(TIMEOUT)
		);
	}

	uint8_t  packetData[PACKET_1K_SIZE] = {0};
	uint32_t packetsReceived            = 0UL;
	uint32_t len                        = 0UL;
	uint32_t errors                     = 0UL;

	while (true) {
		memset(packetData, 0, PACKET_1K_SIZE);
		ReceiveResult result = _ReceivePacket(
		    dbg_uart,
		    (uint8_t*)&packetData[0],
		    &len,
		    msToTicks(TIMEOUT)
		);
		if (result == RECEIVE_RESULT_OK) {
			errors = 0UL;
			if ((packetsReceived + 1UL) >= 256UL) {
				packetsReceived = 0UL;
			}
			packetsReceived++;
			if (len == 0UL) {
				dbg_uart.Putc(CTRL_ACK, msToTicks(TIMEOUT));
				ret = true;
				break;
			}
			if ((packetData[1] & 0xFFU)
			    != (packetsReceived & 0xFFU)) {
				DBG_PRINTF_DEBUG("Packet number mismatch");
				dbg_uart.Putc(CTRL_NAK, msToTicks(TIMEOUT));
				continue;
			}
			dbg_uart.Putc(CTRL_ACK, msToTicks(TIMEOUT));
			dbg_uart.Putc(CTRL_CRC, msToTicks(TIMEOUT));
			// TODO: Process packetData
			// ProcessPacket(packetData, len);
		} else if (result == RECEIVE_RESULT_ABORTED) {
			dbg_uart.Putc(CTRL_ACK, msToTicks(TIMEOUT));
			dbg_uart.Putc(CTRL_ACK, msToTicks(TIMEOUT));
		} else if (result == RECEIVE_RESULT_CANCELED) {
			dbg_uart.Putc(CTRL_CANCEL, msToTicks(TIMEOUT));
			dbg_uart.Putc(CTRL_CANCEL, msToTicks(TIMEOUT));
		} else {
			errors++;
			if (errors > 10UL) {
				dbg_uart.Putc(CTRL_CANCEL, msToTicks(TIMEOUT));
				dbg_uart.Putc(CTRL_CANCEL, msToTicks(TIMEOUT));
				errors = 0UL;
			}
			dbg_uart.Putc(CTRL_CRC, msToTicks(TIMEOUT));
		}
	}

	if (ret) {
		char const* const goodbyeMsg = "\r\nImage uploaded.\r\n";
		for (size_t i = 0; i < strlen(goodbyeMsg); i++) {
			dbg_uart.Putc(
			    static_cast<uint8_t>(goodbyeMsg[i]),
			    msToTicks(TIMEOUT)
			);
		}
	} else {
		char const* const goodbyeMsg =
		    "\r\nCould not upload the image.\r\n";
		for (size_t i = 0; i < strlen(goodbyeMsg); i++) {
			dbg_uart.Putc(
			    static_cast<uint8_t>(goodbyeMsg[i]),
			    msToTicks(TIMEOUT)
			);
		}
	}

	return ret;
}
