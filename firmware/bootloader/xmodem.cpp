#include "xmodem.hpp"

#include "crc16.hpp"
#include "errno.hpp"
#include "pinmap.hpp"
#include "uart.hpp"

#include <cstdint>
#include <cstring>

static XModem::Callback _newPacketCb = nullptr;

enum ReceiveResult : uint32_t {
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
		case XModem::CTRL_SOH:
			packetSize = XModem::PACKET_SIZE;
			break;
		case XModem::CTRL_STX:
			packetSize = XModem::PACKET_1K_SIZE;
			break;
		case XModem::CTRL_EOT:
			return RECEIVE_RESULT_OK;
		case XModem::CTRL_CANCEL:
			ret = uart.Getc(c, timeout);
			if (ret != ERROR_NONE) {
				return RECEIVE_RESULT_ERROR;
			}
			if (c == XModem::CTRL_CANCEL) {
				return RECEIVE_RESULT_CANCELED;
			}
			*len = 0UL;
			return RECEIVE_RESULT_ERROR;
		case XModem::CTRL_ABORT:
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
	Crc16    crc;
	uint32_t i = (XModem::DATA_INDEX);
	for (; i < (packetSize - XModem::CRC_SIZE); i++) {
		crc.Update(data[i]);
	}
	crc.Update(0U);
	crc.Update(0U);
	if (crc.Value() != crcValue) {
		return RECEIVE_RESULT_ERROR;
	}

	return RECEIVE_RESULT_OK;
}

bool XModem::DownloadImage(XModem::Callback newPacketCb) {
	bool ret     = false;
	_newPacketCb = newPacketCb;
	Uart dbg_uart(LPUART1, DBG_UART_TX, DBG_UART_RX);

	char const* const greetingMsg =
		"\r\nUpload image using the XModem protocol.\r\n>";
	for (size_t i = 0; i < strlen(greetingMsg); i++) {
		dbg_uart.Putc(static_cast<uint8_t>(greetingMsg[i]), msToTicks(TIMEOUT));
	}

	uint8_t  packetData[PACKET_1K_SIZE] = {0};
	uint32_t packetsReceived            = 0UL;
	uint32_t len                        = 0UL;
	uint32_t errors                     = 0UL;

	while (true) {
		memset(packetData, 0, PACKET_1K_SIZE);
		ReceiveResult result = _ReceivePacket(
			dbg_uart, (uint8_t*)&packetData[0], &len, msToTicks(TIMEOUT)
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
			if ((packetData[1] & 0xFFU) != (packetsReceived & 0xFFU)) {
				DBG_PRINTF_DEBUG("Packet number mismatch");
				dbg_uart.Putc(CTRL_NAK, msToTicks(TIMEOUT));
				continue;
			}
			if (_newPacketCb != nullptr) {
				int err = _newPacketCb(
					&packetData[XModem::DATA_INDEX], len - DATA_INDEX - CRC_SIZE
				);
				if (err) {
					dbg_uart.Putc(CTRL_NAK, msToTicks(TIMEOUT));
				} else {
					dbg_uart.Putc(CTRL_ACK, msToTicks(TIMEOUT));
				}
			}
			dbg_uart.Putc(CTRL_CRC, msToTicks(TIMEOUT));
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
				static_cast<uint8_t>(goodbyeMsg[i]), msToTicks(TIMEOUT)
			);
		}
	} else {
		char const* const goodbyeMsg = "\r\nCould not upload the image.\r\n";
		for (size_t i = 0; i < strlen(goodbyeMsg); i++) {
			dbg_uart.Putc(
				static_cast<uint8_t>(goodbyeMsg[i]), msToTicks(TIMEOUT)
			);
		}
	}

	return ret;
}
