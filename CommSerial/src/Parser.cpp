#include "Parser.h"
#include "Serial.h"
#include <iostream>

namespace comser {
	Parser::Parser()
		:recvBufferStart(0), recvBufferEnd(0)
	{
		recvBuffer = new uint8_t[MAX_RECV_SIZE];
	}

	int Parser::Write(Serial* serial, const std::string& data)
	{
		return serial->Write((const uint8_t*)data.c_str(), data.size());
	}

	int Parser::Read(Serial* serial, std::string& data)
	{
		if (recvQueue.empty())
		{
			uint16_t recvSize = 0;
			int rVal = serial->Read(&recvBuffer[recvBufferEnd], MAX_RECV_SIZE - recvBufferEnd, recvSize);
			if (rVal < 0) {
				std::cerr << "READ ERROR" << std::endl;
				return rVal;
			}
			if (recvSize > 0) {
				recvBufferEnd += recvSize;
				for (int i = recvBufferStart; i < recvBufferEnd; i++) {
					if (recvBuffer[i] == TERMINUS) {
						recvQueue.emplace(&recvBuffer[recvBufferStart], &recvBuffer[i] + 1);
						recvBufferStart = i + 1;
					}
				}
				if (recvBufferStart >= recvBufferEnd) {
					recvBufferStart = 0;
					recvBufferEnd = 0;
				}
				else if (recvBufferEnd >= BUFFER_MOVE_SIZE && recvBufferStart > 0)
				{
					std::copy(&recvBuffer[recvBufferStart], &recvBuffer[recvBufferEnd], &recvBuffer[0]);
					recvBufferEnd -= recvBufferStart;
					recvBufferStart = 0;
				}
			}
		}
		if (!recvQueue.empty()) {
			data = recvQueue.front();
			recvQueue.pop();
			return 1;
		}
		return 0;
	}

	Parser::~Parser()
	{
	}
}