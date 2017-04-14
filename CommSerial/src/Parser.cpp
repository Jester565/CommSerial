#include "Parser.h"
#include "Serial.h"
#include <iostream>

namespace comser {
	Parser::Parser()
		:recvBufferSize(0)
	{
		recvBuffer = new uint8_t[MAX_RECV_SIZE];
		sendHeaderBuffer = new uint8_t[HEADER_BYTES];
		int num = 1;
		littleEndian = (*(char *)&num == 1);
		crc = new Crc();
	}

	int Parser::Write(Serial* serial, const std::vector<uint8_t>& data)
	{
		CrcInt dataCRC = crc->GenCRC(data.data(), data.size());
		uint16_t totalDataSize = data.size() + SIZE_BYTES + CRC_WIDTH;
		if (littleEndian) {
			for (int i = 0; i < SIZE_BYTES; i++) {
				sendHeaderBuffer[SIZE_BYTES - i - 1] = (totalDataSize >> (8 * i)) & 0xff;
			}
			for (int i = 0; i < CRC_WIDTH; i++) {
				sendHeaderBuffer[HEADER_BYTES - i - 1] = ((dataCRC) >> (8 * i)) & 0xff;
			}
		}
		else
		{
			for (int i = 0; i < SIZE_BYTES; i++) {
				sendHeaderBuffer[i] = (totalDataSize >> (8 * i)) & 0xff;
			}
			for (int i = 0; i < CRC_WIDTH; i++) {
				sendHeaderBuffer[i + SIZE_BYTES] = ((dataCRC) >> (8 * i)) & 0xff;
			}
		}
		int writeStatus = serial->Write(sendHeaderBuffer, HEADER_BYTES);
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
				if (CheckCrc()) {
					data.insert(data.end(), &recvBuffer[HEADER_BYTES], &recvBuffer[recvBufferSize]);
					recvBufferSize = 0;
					return totalDataSize - SIZE_BYTES;
				}
				else
				{
					std::cerr << "INVALID CRC!" << std::endl;
					recvBufferSize = 0;
				}
			}
		}
		return 0;
	}

	Parser::~Parser()
	{
	}
	bool Parser::CheckCrc()
	{
		if (recvBufferSize < HEADER_BYTES) {
			return false;
		}
		else
		{
			CrcInt dataCRC = crc->GenCRC(&recvBuffer[HEADER_BYTES], recvBufferSize - HEADER_BYTES);
			CrcInt sentCRC = 0;
			if (littleEndian) {
				for (int i = 0; i < CRC_WIDTH; i++) {
					sentCRC |= (recvBuffer[HEADER_BYTES - i - 1] & 0xff) << (8 * i);
				}
			}
			else
			{
				for (int i = 0; i < CRC_WIDTH; i++) {
					sentCRC |= (recvBuffer[i] & 0xff) << (8 * i);
				}
			}
			return dataCRC == sentCRC;
		}
	}
}