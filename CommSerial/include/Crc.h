#pragma once
#include "stdafx.h"
#include <stdint.h>

namespace comser {
	typedef uint8_t CrcInt;
	static const uint8_t CRC_WIDTH = 8;
	static const CrcInt CRC_TOP = (1 << (CRC_WIDTH - 1));
	static const CrcInt CRC_POLYNOMIAL = 0x39;
	class Crc {
	public:
		Crc() {
			InitCRCTable();
		}

		CrcInt GenCRC(const uint8_t* data, uint32_t size) {
			CrcInt remainder = 0;
			uint8_t curByte = 0;
			for (int i = 0; i < size; i++) {
				curByte = data[i] ^ (remainder >> (CRC_WIDTH - 8));
				remainder = precalcDivs[curByte] ^ (remainder << 8);
			}
			return remainder;
		}

	protected:
		void InitCRCTable() {
			CrcInt remainder;

			for (int dividend = 0; dividend < 256; dividend++) {
				remainder = dividend << (CRC_WIDTH - 8);
				for (uint8_t bit = 0; bit < 8; bit++) {
					if (remainder & CRC_TOP) {
						remainder = (remainder << 1) ^ CRC_POLYNOMIAL;
					}
					else
					{
						remainder = (remainder << 1);
					}
				}
				precalcDivs[dividend] = remainder;
			}
		}

		CrcInt precalcDivs[256];
	};
}