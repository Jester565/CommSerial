#include "Parser.h"
#include "Serial.h"
#include <iostream>

namespace comser {
	Parser::Parser()
		:recvBufferSize(0)
	{
		recvBuffer = new uint8_t[MAX_RECV_SIZE];
		sendSizeBuffer = new uint8_t[SIZE_BYTES];
		int num = 1;
		littleEndian = (*(char *)&num == 1);
	}

	int Parser::Write(Serial* serial, const std::vector<uint8_t>& data)
	{
		uint16_t totalDataSize = data.size() + SIZE_BYTES;
		if (littleEndian) {
			for (int i = 0; i < SIZE_BYTES; i++) {
				sendSizeBuffer[SIZE_BYTES - i - 1] = (totalDataSize >> (8 * i)) & 0xff;
			}
		}
		else
		{
			for (int i = 0; i < SIZE_BYTES; i++) {
				sendSizeBuffer[i] = (totalDataSize >> (8 * i)) & 0xff;
			}
		}
		int writeStatus = serial->Write(sendSizeBuffer, SIZE_BYTES);
		if (writeStatus <= 0) {
			return writeStatus;
		}
		writeStatus = serial->Write(data.data(), data.size());
		return writeStatus;
	}

	int Parser::Read(Serial* serial, std::vector<uint8_t>& data)
	{
		if (recvBufferSize < SIZE_BYTES) {
			uint16_t recvSize = 0;
			int readStatus = serial->Read(&recvBuffer[recvBufferSize], SIZE_BYTES - recvBufferSize, recvSize);
			recvBufferSize += recvSize;
			if (readStatus <= 0) {
				return readStatus;
			}
		}
		if (recvBufferSize >= SIZE_BYTES) {
			uint16_t totalDataSize = 0;
			if (littleEndian) {
				for (int i = 0; i < SIZE_BYTES; i++) {
					totalDataSize |= (recvBuffer[SIZE_BYTES - i - 1] & 0xff) << (8 * i);
				}
			}
			else
			{
				for (int i = 0; i < SIZE_BYTES; i++) {
					totalDataSize |= (recvBuffer[i] & 0xff) << (8 * i);
				}
			}
			uint16_t recvSize = 0;
			
			int readStatus = serial->Read(&recvBuffer[recvBufferSize], totalDataSize - recvBufferSize, recvSize);
			if (readStatus <= 0) {
				return readStatus;
			}
			recvBufferSize += recvSize;
			if (recvBufferSize == totalDataSize) {
				data.insert(data.end(), &recvBuffer[SIZE_BYTES], &recvBuffer[recvBufferSize]);
				recvBufferSize = 0;
				return totalDataSize - SIZE_BYTES;
			}
			
		}
		return 0;
	}

	Parser::~Parser()
	{
	}
}