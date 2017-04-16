#pragma once
#include <vector>
#include <cstdint>
#include <memory>
#include <queue>

namespace comser {
	class Serial;
	class Crc;

	class Parser
	{
	public:
		static const char TERMINUS = ';';
		static const uint16_t MAX_RECV_SIZE = 200;
		static const uint16_t BUFFER_MOVE_SIZE = 150;

		Parser();

		int Write(Serial*, const std::string& data);

		int Read(Serial*, std::string& data);

		~Parser();

	private:
		uint16_t recvBufferStart;
		uint16_t recvBufferEnd;
		uint8_t* recvBuffer;
		std::queue<std::string> recvQueue;
	};
}
