#include "Parser.h"
#include "Serial.h"
#include "Crc.h"
#include "ObjStream.h"
#include "Packet.h"
#include "PackManager.h"
#include "Base32.h"
#include <iostream>

namespace comser {

	const uint8_t Parser::START_KEY[] = { 'C', 'P', 0x44 };
	const uint8_t Parser::START_KEY_MASK[] = { 0xFF, 0xFF, 0xFC };
	const uint8_t Parser::START_KEY_BYTES = 3;

	const uint8_t Parser::SIZE_BYTE_POS = 0;
	const uint8_t Parser::SIZE_BYTES = 1;

	const uint8_t Parser::CRC_BYTE_POS = SIZE_BYTES;
	const uint8_t Parser::CRC_BYTES = 1;

	const uint8_t Parser::HEADER_BYTES = SIZE_BYTES + CRC_BYTES;

	Parser::Parser()
		:recvBufferSize(0), startKeyI(0), prefix(nullptr), postfix(nullptr)
	{
		recvBuffer = new uint8_t[MAX_MSG_SIZE];
		int num = 1;
		littleEndian = (*(char *)&num == 1);
		crc = new Crc();
		base32Converter = new Base32();
	}

	int Parser::Write(Serial* serial, std::shared_ptr<Packet> pack)
	{
		uint8_t hID = pack->GetID();
		bool maxSize = pack->GetSize() == MAX_PACKET_SIZE;
		if (maxSize) {
			hID <<= 4;
			hID |= 0x40;
		}

		std::shared_ptr<ObjStream> packStream = std::make_shared<ObjStream>();
		*packStream << (uint8_t)pack->GetID();  //will be used to calculate crc, placeholder for CRC
		pack->Pack(packStream);
		CrcInt payloadCRC = crc->GenCRC(packStream->buffer.data(), packStream->buffer.size());
		packStream->buffer.at(0) = payloadCRC;

		std::vector<uint8_t> data;
		data.reserve(START_KEY_BYTES + ((!maxSize) ? SIZE_BYTES : 0) + 
			((prefix != nullptr) ? prefix->size() : 0) + 
			base32Converter->Base256To32Size(packStream->buffer.size()) +
			((postfix != nullptr) ? postfix->size() : 0));
		if (prefix != nullptr) {
			data.insert(data.end(), prefix->begin(), prefix->end());
		}
		for (int i = 0; i < START_KEY_BYTES; i++) {
			data.push_back(START_KEY[i]);
		}
		*data.rbegin() |= ((hID & 0x60) >> 5);
		if (!maxSize) {
			data.push_back(base32Converter->EncodeByte(hID & 0x1F));
		}
		base32Converter->Encode(packStream->buffer, data);
		if (maxSize) {
			*data.rbegin() = base32Converter->EncodeByte((hID & 0x10) | base32Converter->DecodeByte(*data.rbegin()));
		}
		if (postfix != nullptr) {
			data.insert(data.end(), postfix->begin(), postfix->end());
		}
		return serial->Write(data.data(), data.size());
	}

	int Parser::Read(Serial* serial, uint8_t& hID, std::shared_ptr<ObjStream>& stream, PackManager* packManager)
	{
		if (startKeyI < START_KEY_BYTES) {
			int readStatus = CheckStartKey(serial);
			if (readStatus <= 0) {
				return readStatus;
			}
		}
		if (startKeyI >= START_KEY_BYTES) {
			hID = (lastKeyByte & 0x03) << 5;
			bool maxSize = (hID >= 0x40);
			uint8_t dataSize = 0;
			uint8_t payloadStartI = 0;
			if (maxSize) {
				dataSize = base32Converter->Base256To32Size(MAX_PACKET_SIZE + CRC_BYTES);
			}
			else
			{
				payloadStartI = 1;
				if (recvBufferSize < SIZE_BYTES) {
					int readStatus = ReadSize(serial);
					if (readStatus <= 0) {
						return readStatus;
					}
				}
				if (recvBufferSize >= SIZE_BYTES) {
					hID |= base32Converter->DecodeByte(recvBuffer[0]) & 0x1F;
					if (packManager->GetPackSize(hID, dataSize)) {
						dataSize = base32Converter->Base256To32Size(dataSize + CRC_BYTES) + SIZE_BYTES;
					}
					else
					{
						std::cerr << "Could not find size of received packet with id " << (int)hID << std::endl;
						startKeyI = 0;
						recvBufferSize = 0;
						return 0;
					}
				}
			}
			if (dataSize > 0) {
				if (recvBufferSize < dataSize) {
					uint16_t recvSize = 0;
					int readStatus = serial->Read(&recvBuffer[recvBufferSize], dataSize - recvBufferSize, recvSize);
					if (readStatus <= 0) {
						return readStatus;
					}
					recvBufferSize += recvSize;
				}
				if (recvBufferSize >= dataSize) {
					if (maxSize) {
						hID |= (base32Converter->DecodeByte(recvBuffer[recvBufferSize - 1]) & 0x10);
						hID &= 0x3F;
						hID >>= 4;
						uint8_t packSize = 0;
						if (packManager->GetPackSize(hID, packSize)) {
							if (base32Converter->Base256To32Size(packSize + CRC_BYTES) != dataSize) {
								std::cerr << "Size of id: " << (int)hID << " did not match with the size that was received" << std::endl;
								std::cerr << "Received size: " << base32Converter->Base32To256Size(dataSize) - 1 << std::endl;
								std::cerr << "Local size: " << packSize << std::endl;
								startKeyI = 0;
								recvBufferSize = 0;
								return 0;
							}
						}
						else
						{
							std::cerr << "Could not find the received packet with id " << (int)hID << std::endl;
							startKeyI = 0;
							recvBufferSize = 0;
							return 0;
						}
					}
					std::vector<uint8_t> encodedData;
					std::vector<uint8_t> decodedData;
					encodedData.assign(recvBuffer + payloadStartI, recvBuffer + recvBufferSize);
					
					startKeyI = 0;
					recvBufferSize = 0;
					
					base32Converter->Decode(encodedData, decodedData);
					CrcInt crcInt = decodedData.at(0);
					decodedData.at(0) = hID;
					CrcInt genCrc = crc->GenCRC(decodedData.data(), decodedData.size());
					if (crcInt != genCrc) {
						std::cerr << "Crc did not match" << std::endl;
						return 0;
					}
					stream = std::make_shared<ObjStream>(decodedData.begin() + 1, decodedData.end());
					return stream->buffer.size();
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
		delete base32Converter;
		base32Converter = nullptr;
	}
	int Parser::CheckStartKey(Serial* serial)
	{
		int readStatus;
		while (true) {
			readStatus = serial->Read(recvBuffer, 1);
			if (readStatus > 0) {
				if ((recvBuffer[0] & START_KEY_MASK[startKeyI]) == (START_KEY[startKeyI] & START_KEY_MASK[startKeyI])) {
					startKeyI++;
					if (startKeyI == START_KEY_BYTES) {
						lastKeyByte = recvBuffer[0];
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
		return readStatus;
	}
	SizeInt Parser::GetDataSize()
	{
		SizeInt totalDataSize = 0;
		if (littleEndian) {
			for (int i = 0; i < SIZE_BYTES; i++) {
				totalDataSize |= (recvBuffer[SIZE_BYTE_POS + SIZE_BYTES - i - 1] & 0xff) << (8 * i) - 'A';
			}
		}
		else
		{
			for (int i = 0; i < SIZE_BYTES; i++) {
				totalDataSize |= (recvBuffer[SIZE_BYTE_POS + i] & 0xff) << (8 * i) - 'A';
			}
		}
		return totalDataSize;
	}

	/*
	int Parser::ProcessPayload(uint8_t hID, std::shared_ptr<Packet>& pack)
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
	bool Parser::CheckCrc(uint8_t hID)
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
	*/
}

