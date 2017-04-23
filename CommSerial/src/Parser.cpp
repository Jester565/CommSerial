#include "Parser.h"
#include "Serial.h"
#include "Crc.h"
#include "ObjStream.h"
#include <iostream>

namespace comser {

	const uint8_t* Parser::START_KEY = (const uint8_t*)"CPP";
	const uint8_t Parser::START_KEY_BYTES = 3;

	const uint8_t Parser::SIZE_BYTE_POS = 0;
	const uint8_t Parser::SIZE_BYTES = 1;

	const uint8_t Parser::CRC_BYTE_POS = SIZE_BYTES;
	const uint8_t Parser::CRC_BYTES = 1;

	const uint8_t Parser::HEADER_BYTES = SIZE_BYTES + CRC_BYTES;

	Parser::Parser()
		:recvBufferSize(0), startKeyI(0), prefix(nullptr), postfix(nullptr)
	{
		recvBuffer = new uint8_t[HEADER_BYTES + MAX_MSG_SIZE];
		sendHeaderBuffer = new uint8_t[HEADER_BYTES];
		int num = 1;
		littleEndian = (*(char *)&num == 1);
		crc = new Crc();
	}

	int Parser::Write(Serial* serial, std::shared_ptr<ObjStream> data)
	{
		uint16_t payloadSize = data->buffer.size();
		CrcInt dataCRC = crc->GenCRC(data->buffer.data(), payloadSize);		
		if (littleEndian) {
			for (int i = 0; i < SIZE_BYTES; i++) {
				sendHeaderBuffer[SIZE_BYTES - i - 1] = (payloadSize >> (8 * i)) & 0xff;
			}
			for (int i = 0; i < CRC_BYTES; i++) {
				sendHeaderBuffer[HEADER_BYTES - i - 1] = ((dataCRC) >> (8 * i)) & 0xff;
			}
		}
		else
		{
			for (int i = 0; i < SIZE_BYTES; i++) {
				sendHeaderBuffer[i] = (payloadSize >> (8 * i)) & 0xff;
			}
			for (int i = 0; i < CRC_WIDTH; i++) {
				sendHeaderBuffer[i + SIZE_BYTES] = ((dataCRC) >> (8 * i)) & 0xff;
			}
		}
		int writeStatus;
		if (prefix != nullptr) {
			writeStatus = serial->Write(prefix->data(), prefix->size());
			if (writeStatus <= 0) {
				return writeStatus;
			}
		}
		writeStatus = serial->Write(START_KEY, START_KEY_BYTES);
		if (writeStatus <= 0) {
			return writeStatus;
		}
		writeStatus = serial->Write(sendHeaderBuffer, HEADER_BYTES);
		if (writeStatus <= 0) {
			return writeStatus;
		}
		writeStatus = serial->Write(data->buffer.data(), payloadSize);
		if (writeStatus <= 0) {
			return writeStatus;
		}
		if (postfix != nullptr) {
			writeStatus = serial->Write(postfix->data(), postfix->size());
			if (writeStatus <= 0) {
				return writeStatus;
			}
		}
		return 1;
	}

	int Parser::Read(Serial* serial, std::shared_ptr<ObjStream> data)
	{
		if (startKeyI < START_KEY_BYTES) {
			int readStatus = CheckStartKey(serial);
			if (readStatus <= 0) {
				return readStatus;
			}
		}
		if (startKeyI >= START_KEY_BYTES) {
			if (recvBufferSize < SIZE_BYTES) {
				int readStatus = ReadSize(serial);
				if (readStatus <= 0) {
					return readStatus;
				}
			}
			if (recvBufferSize >= SIZE_BYTES) {
				SizeInt payloadSize = GetDataSize();
				uint16_t recvSize = 0;
				int readStatus = serial->Read(&recvBuffer[recvBufferSize], CRC_BYTES + payloadSize, recvSize);
				if (readStatus <= 0) {
					return readStatus;
				}
				recvBufferSize += recvSize;
				if (recvBufferSize >= payloadSize + HEADER_BYTES) {
					return ProcessPayload(data->buffer, payloadSize);
				}
			}
		}
		return 0;
	}

	Parser::~Parser()
	{
		delete crc;
		crc = nullptr;
		delete[] recvBuffer;
		recvBuffer = nullptr;
		delete[] sendHeaderBuffer;
		sendHeaderBuffer = nullptr;
	}
	int Parser::CheckStartKey(Serial* serial)
	{
		int readStatus;
		while (true) {
			readStatus = serial->Read(recvBuffer, 1);
			if (readStatus > 0) {
				if (recvBuffer[0] == START_KEY[startKeyI]) {
					startKeyI++;
					if (startKeyI == START_KEY_BYTES) {
						return START_KEY_BYTES;
					}
				}
				else
				{
					startKeyI = 0;
				}
			}
			else
			{
				return readStatus;
			}
		}
		return 0;
	}
	int Parser::ReadSize(Serial * serial)
	{
		uint16_t recvSize = 0;
		int readStatus = serial->Read(&recvBuffer[recvBufferSize], SIZE_BYTES - recvBufferSize, recvSize);
		recvBufferSize += recvSize;
		if (readStatus <= 0) {
			return readStatus;
		}
	}
	SizeInt Parser::GetDataSize()
	{
		SizeInt totalDataSize = 0;
		if (littleEndian) {
			for (int i = 0; i < SIZE_BYTES; i++) {
				totalDataSize |= (recvBuffer[SIZE_BYTE_POS + SIZE_BYTES - i - 1] & 0xff) << (8 * i);
			}
		}
		else
		{
			for (int i = 0; i < SIZE_BYTES; i++) {
				totalDataSize |= (recvBuffer[SIZE_BYTE_POS + i] & 0xff) << (8 * i);
			}
		}
		return totalDataSize;
	}

	int Parser::ProcessPayload(std::vector<uint8_t>& data, SizeInt payloadSize)
	{
		if (CheckCrc()) {
			data.insert(data.end(), &recvBuffer[HEADER_BYTES], &recvBuffer[HEADER_BYTES + payloadSize]);
			ResetSizes();
			return payloadSize;
		}
		else
		{
			ResetSizes();
			std::cerr << "INVALID CRC!" << std::endl;
		}
		return 0;
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
				for (int i = 0; i < CRC_BYTES; i++) {
					sentCRC |= (recvBuffer[CRC_BYTE_POS + CRC_BYTES - i - 1] & 0xff) << (8 * i);
				}
			}
			else
			{
				for (int i = 0; i < CRC_BYTES; i++) {
					sentCRC |= (recvBuffer[CRC_BYTE_POS + i] & 0xff) << (8 * i);
				}
			}
			return dataCRC == sentCRC;
		}
	}
}
