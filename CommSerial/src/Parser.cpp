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
		int num = 1;
		littleEndian = (*(char *)&num == 1);

		recvBuffer = new uint8_t[MAX_MSG_SIZE];
		crc = new Crc();
		base32Converter = new Base32();
	}

	int Parser::Write(Serial* serial, std::shared_ptr<Packet> pack)
	{
		uint8_t hID = pack->GetID();  //hID represents the id of the packet in a format that can be serialized in data
		bool maxSize = pack->GetSize() == MAX_PACKET_SIZE;  //we will use different serialization if at maxSize as we try to save some bits
		if (maxSize) {
			hID <<= 4;  //shift the packet id left by 2 bits, the maximum value is now 00110000
			hID |= 0x40;  //set the 7th bit to 1 indicating this packet is at max size, the maximum value is now 01110000
		}

		std::shared_ptr<ObjStream> packStream = std::make_shared<ObjStream>();
		*packStream << (uint8_t)pack->GetID();  //will be used to calculate crc and serves as a placeholder for the crc
		pack->Pack(packStream);
		CrcInt payloadCRC = crc->GenCRC(packStream->buffer.data(), packStream->buffer.size());
		packStream->buffer.at(0) = payloadCRC;  //put crc in data now that it is calculated

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
		/*
		The first bits of the hID are set to the last bits of the last byte in START_KEY
		If the hID is 01011111, and the last byte of the start key is 01000000 then it becomes 01000010
		*/
		*data.rbegin() |= ((hID & 0x60) >> 5);
		if (!maxSize) {
			//if we are not at the maximum size, then we can add a byte to data that stores the id
			data.push_back(base32Converter->EncodeByte(hID & 0x1F));
		}
		//append the base32 conversion of buffer to data
		base32Converter->Encode(packStream->buffer, data);
		if (maxSize) {
			/*
			if at max size then we will store the last bit of the id at the one free bit at the end of the data. 
			We know this bit is unused due to the nature of base32 encoding
			*/
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
			//the 7th and 6th bits of hID is stored in the last byte of the key
			hID = (lastKeyByte & 0x03) << 5;
			bool maxSize = (hID >= 0x40);  //the 7th bit of hID indicates if the packet is at max size
			uint8_t dataSize = 0;
			uint8_t payloadStartI = 0;
			{
				int ec = ReadHeader(serial, packManager, maxSize, hID, dataSize, payloadStartI);
				if (ec <= 0) {
					if (ec < 0) {
						ResetSizes();
					}
					return ec;
				}
			}
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
					int ec = ReadMaxSizeHIDEnd(packManager, dataSize, hID);
					if (ec <= 0) {
						if (ec < 0) {
							ResetSizes();
						}
						return ec;
					}
				}

				std::vector<uint8_t> encodedData;
				std::vector<uint8_t> decodedData;
				encodedData.assign(recvBuffer + payloadStartI, recvBuffer + recvBufferSize);
					
				ResetSizes();

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
		delete prefix;
		prefix = nullptr;
		delete postfix;
		postfix = nullptr;
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

	int Parser::ReadHeader(Serial * serial, PackManager* packManager, bool maxSize, uint8_t & hID, uint8_t& dataSize, uint8_t& payloadStartI)
	{
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
					return -1;
				}
			}
		}
		return 1;
	}

	int Parser::ReadSize(Serial * serial)
	{
		uint16_t recvSize = 0;
		int readStatus = serial->Read(&recvBuffer[recvBufferSize], SIZE_BYTES - recvBufferSize, recvSize);
		recvBufferSize += recvSize;
		return readStatus;
	}

	int Parser::ReadMaxSizeHIDEnd(PackManager* packManager, uint8_t dataSize, uint8_t & hID)
	{
		hID |= (base32Converter->DecodeByte(recvBuffer[recvBufferSize - 1]) & 0x10);
		hID &= 0x3F;
		hID >>= 4;
		uint8_t packSize = 0;
		if (packManager->GetPackSize(hID, packSize)) {
			if (base32Converter->Base256To32Size(packSize + CRC_BYTES) != dataSize) {
				std::cerr << "Size of id: " << (int)hID << " did not match with the size that was received" << std::endl;
				std::cerr << "Received size: " << base32Converter->Base32To256Size(dataSize) - 1 << std::endl;
				std::cerr << "Local size: " << packSize << std::endl;
				return -1;
			}
		}
		else
		{
			std::cerr << "Could not find the received packet with id " << (int)hID << std::endl;
			return -1;
		}
		return 1;
	}
}

