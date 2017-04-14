#pragma once
#include "Serial.h"
#include <Windows.h>
#include <string>
#include <cstdint>

namespace comser {

	class WinSerial : public Serial
	{
	public:
		WinSerial();

		bool Open(const std::string& portName) override;

		int Write(const uint8_t* data, uint16_t size) override;

		int Read(uint8_t* data, uint16_t maxSize) override;

		int Read(uint8_t* data, uint16_t maxSize, uint16_t& size) override;

		bool Close() override;

		~WinSerial();

	private:
		HANDLE hComm;
	};
}
