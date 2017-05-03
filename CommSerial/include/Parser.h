#pragma once
#include "stdafx.h"
#include <cstdint>
#include <memory>
#include <vector>

namespace comser {
	class Serial;
	class Crc;
	class ObjStream;
	class Packet;
	class Base32;
	class PackManager;
	typedef uint8_t SizeInt;

	class Parser
	{
	public:

		static const uint8_t START_KEY[];
		static const uint8_t START_KEY_MASK[];
		static const uint8_t START_KEY_BYTES;
		static const uint8_t SIZE_BYTE_POS;
		static const uint8_t SIZE_BYTES;
		static const uint8_t CRC_BYTE_POS;
		static const uint8_t CRC_BYTES;
		static const uint8_t HEADER_BYTES;

		Parser();

		int Write(Serial*, std::shared_ptr<Packet> pack);

		int Read(Serial* serial, uint8_t& hID, std::shared_ptr<ObjStream>& stream, PackManager* packManager);

		void SetSendPrefix(const std::string& prefix) {
			this->prefix = new std::vector<uint8_t>(prefix.begin(), prefix.end());
		}

		void SetSendPostfix(const std::string& postfix) {
			this->postfix = new std::vector<uint8_t>(postfix.begin(), postfix.end());
		}

		~Parser();

	private:
		int CheckStartKey(Serial* serial);
		int ReadHeader(Serial* serial, PackManager* packManager, bool isMaxSize, uint8_t& hID, uint8_t& dataSize, uint8_t& payloadStartI);
		int ReadSize(Serial* serial);
		int ReadMaxSizeHIDEnd(PackManager* packManager, uint8_t dataSize, uint8_t& hID);
		void ResetSizes() {
			recvBufferSize = 0;
			startKeyI = 0;
		}
		uint8_t* recvBuffer;
		SizeInt recvBufferSize;

		uint8_t startKeyI;
		uint8_t lastKeyByte;

		Base32* base32Converter;
		Crc* crc;

		bool littleEndian;

		std::vector<uint8_t>* prefix;
		std::vector<uint8_t>* postfix;
	};
}
