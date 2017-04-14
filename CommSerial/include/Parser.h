#pragma once
#include <vector>
#include <cstdint>
#include <memory>

namespace comser {
	class Serial;

	class Parser
	{
	public:
		static const uint8_t SIZE_BYTES = 2;
		typedef uint16_t SizeInt;
		static const SizeInt MAX_RECV_SIZE = 1024;

		Parser();

		int Write(Serial*, const std::vector<uint8_t>& data);

		int Read(Serial*, std::vector<uint8_t>& data);

		~Parser();

	private:
		SizeInt recvBufferSize;
		uint8_t* recvBuffer;
		uint8_t* sendSizeBuffer;
		bool littleEndian;
	};
}
