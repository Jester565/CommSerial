#pragma once
#include <vector>
#include <cstdint>
#include <memory>
#include "Crc.h"

namespace comser {
	class Serial;
	class Crc;

	class Parser
	{
	public:
		static const uint8_t SIZE_BYTES = 2;
		typedef uint16_t SizeInt;
		static const SizeInt MAX_RECV_SIZE = 1024;
		static const uint8_t CRC_SIZE_BYTES = 1;
		static const uint8_t HEADER_BYTES = SIZE_BYTES + CRC_WIDTH;

		Parser();

		int Write(Serial*, const std::vector<uint8_t>& data);

		int Read(Serial*, std::vector<uint8_t>& data);

		~Parser();

	private:
		bool CheckCrc();
		SizeInt recvBufferSize;
		uint8_t* recvBuffer;
		uint8_t* sendHeaderBuffer;
		Crc* crc;
		bool littleEndian;
	};
}
