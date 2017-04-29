#pragma once
#include <vector>
#include <iostream>

namespace comser {
	class Base32
	{
	public:
		Base32();

		void encode(std::vector<uint8_t>& in, std::vector<uint8_t>& out);

		void decode(std::vector<uint8_t>& in, std::vector<uint8_t>& out);

		~Base32();
	};
}
