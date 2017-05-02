#pragma once
#include "stdafx.h"
#include <vector>
#include <algorithm>
#include <cstring>
#include <iostream>

namespace comser {
	class Parser;

	class ObjStream {
	public:
		friend Parser;

		ObjStream();
		ObjStream(std::vector<uint8_t>::iterator begin, std::vector<uint8_t>::iterator end);

		ObjStream& operator << (int8_t data);
		ObjStream& operator << (uint8_t data);
		ObjStream& operator << (int16_t data);
		ObjStream& operator << (uint16_t data);
		ObjStream& operator << (int32_t data);
		ObjStream& operator << (uint32_t data);
		ObjStream& operator << (int64_t data);
		ObjStream& operator << (uint64_t data);
		ObjStream& operator << (float data);
		ObjStream& operator << (double data);
		ObjStream& operator << (const std::string& str);

		ObjStream& operator >> (int8_t& data);
		ObjStream& operator >> (uint8_t& data);
		ObjStream& operator >> (int16_t& data);
		ObjStream& operator >> (uint16_t& data);
		ObjStream& operator >> (int32_t& data);
		ObjStream& operator >> (uint32_t& data);
		ObjStream& operator >> (int64_t& data);
		ObjStream& operator >> (uint64_t& data);
		ObjStream& operator >> (float& data);
		ObjStream& operator >> (double& data);
		ObjStream& operator >> (std::string& data);

	private:
		bool littleEndian;
		std::vector<uint8_t> buffer;
		uint16_t idx;

		template <typename T>
		void SwapEndian(T& data)
		{
			if (littleEndian) {
				char& raw = reinterpret_cast<char&>(data);
				std::reverse(&raw, &raw + sizeof(T));
			}
		}

		template <typename T>
		void Pack(T& data) {
			if (idx + sizeof(data) > MAX_MSG_SIZE) {
				std::cerr << "Buffer size exceeded" << std::endl;
				return;
			}
			SwapEndian(data);
			buffer.resize(idx + sizeof(data));
			memcpy(&buffer[idx], &data, sizeof(data));
			idx += sizeof(data);
		}

		template <typename T>
		void Unpack(T& data) {
			if (idx + sizeof(data) > buffer.size()) {
				std::cerr << "Buffer size exceeded: No more data" << std::endl;
				return;
			}
			memcpy(&data, &buffer[idx], sizeof(data));
			SwapEndian(data);
			idx += sizeof(data);
		}
	};
}
