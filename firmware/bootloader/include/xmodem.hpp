#pragma once

#include "system.hpp"

#include <cstdint>

namespace XModem {
	constexpr uint32_t CONTROL_SIZE  = 1UL;
	constexpr uint32_t BLOCK_ID_SIZE = 2UL;
	constexpr uint32_t DATA_INDEX    = CONTROL_SIZE + BLOCK_ID_SIZE;
	constexpr uint32_t DATA_SIZE     = 128UL;
	constexpr uint32_t DATA_1K_SIZE  = 1'024UL;
	constexpr uint32_t CRC_SIZE      = 2UL;
	constexpr uint32_t PACKET_SIZE =
	    CONTROL_SIZE + BLOCK_ID_SIZE + DATA_SIZE + CRC_SIZE;
	constexpr uint32_t PACKET_1K_SIZE =
	    CONTROL_SIZE + BLOCK_ID_SIZE + DATA_1K_SIZE + CRC_SIZE;
	constexpr TickType TIMEOUT = msToTicks(1'000UL * 10UL);

	enum ControlCharacter : uint8_t {
		CTRL_SOH    = 1,  // Start of Header
		CTRL_STX    = 2,  // Start of Text (1K packet)
		CTRL_EOT    = 4,  // End of Transmission
		CTRL_ACK    = 6,  // Acknowledge
		CTRL_NAK    = 21, // Negative Acknowledge
		CTRL_CANCEL = 24, // Cancel
		CTRL_ABORT  = 65, // Abort
		CTRL_CRC    = 67, // Cyclic Redundancy Check
	};

	using Callback = void (*)(uint8_t* data, uint32_t len);

	bool DownloadImage(Callback newPacketCb);
} // namespace XModem
