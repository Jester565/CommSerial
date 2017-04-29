#pragma once
#include "stdafx.h"
#include <cstdint>
#include <memory>
#include <vector>

namespace comser {
	class Serial;
	class Crc;
	class ObjStream;
	typedef uint8_t SizeInt;

	class Parser
	{
	public:

		static const uint8_t* START_KEY;
		static const uint8_t START_KEY_BYTES;
		static const uint8_t SIZE_BYTE_POS;
		static const uint8_t SIZE_BYTES;
		static const uint8_t CRC_BYTE_POS;
		static const uint8_t CRC_BYTES;
		static const uint8_t HEADER_BYTES;

		Parser();

		int Write(Serial*, uint8_t id, std::shared_ptr<ObjStream> data);

		int Read(Serial*, uint8_t& id, std::shared_ptr<ObjStream> data);

		void SetSendPrefix(const std::string& prefix) {
			this->prefix = new std::vector<uint8_t>(prefix.begin(), prefix.end());
		}

		void SetSendPostfix(const std::string& postfix) {
			this->postfix = new std::vector<uint8_t>(postfix.begin(), postfix.end());
		}

		~Parser();

	private:
		int CheckStartKey(Serial* serial);
		int ReadSize(Serial* serial);
		SizeInt GetDataSize();
		int ProcessPayload(std::vector<uint8_t>& data, SizeInt payloadSize);
		bool CheckCrc();
		void ResetSizes() {
			recvBufferSize = 0;
			startKeyI = 0;
		}
		SizeInt recvBufferSize;
		uint8_t* recvBuffer;
		uint8_t* sendHeaderBuffer;
		uint8_t startKeyI;
		Crc* crc;
		bool littleEndian;

		std::vector<uint8_t>* prefix;
		std::vector<uint8_t>* postfix;
	};
}
