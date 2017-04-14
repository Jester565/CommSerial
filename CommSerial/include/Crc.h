#pragma once
#include <stdint.h>

namespace comser {
	typedef uint32_t CrcInt;
	static const uint8_t CRC_WIDTH = 32;
	static const CrcInt CRC_TOP = (1 << (CRC_WIDTH - 1));
	static const CrcInt CRC_POLYNOMIAL = 0x04C11DB7;
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

		uint8_t GenCRC8(const uint16_t data) {
			uint8_t remainder = 0;
			for (int byte = 0; byte < 2; byte++) {
				remainder ^= (data >> (8 * byte)) & 0xff;
				for (uint8_t bit = 0; bit < 8; bit++) {
					if (remainder & 0x80) {
						remainder = (remainder << 1) ^ 0x39;
					}
					else
					{
						remainder = (remainder << 1);
					}
				}
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