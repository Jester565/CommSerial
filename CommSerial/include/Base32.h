#pragma once
#include <vector>
#include <iostream>

namespace comser {
	class Base32
	{
	public:
		Base32();

		void Encode(std::vector<uint8_t>& in, std::vector<uint8_t>& out);

		void Decode(std::vector<uint8_t>& in, std::vector<uint8_t>& out);

		uint8_t EncodeByte(uint8_t byte);

		uint8_t DecodeByte(uint8_t byte);

		uint8_t Base256To32Size(uint8_t size) {
			double outSize = size * (8.0 / 5.0);
			if (outSize > (int)outSize) {
				outSize = (int)outSize + 1;
			}
			return (int)outSize;
		}

		uint8_t Base32To256Size(uint8_t size) {
			return (int)(size * (5.0 / 8.0));
		}

		~Base32();
	};
}
