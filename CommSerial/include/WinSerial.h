#pragma once
#include "Serial.h"
#include <Windows.h>
#include <string>
#include <cstdint>

static const uint32_t BAUDRATE = 57600;
static const uint32_t BYTE_SIZE = 8;
static const uint32_t STOP_BIT = 0;
static const uint32_t PARITY = 0;

static const uint32_t READ_INTERVAL_TIMEOUT = 50;
static const uint32_t READ_TOTAL_TIMEOUT = 50;
static const uint32_t READ_TOTAL_TIMEOUT_MULTIPLIER = 50;
static const uint32_t WRITE_TOTAL_TIMEOUT_CONSTANT = 50;
static const uint32_t WRITE_TOTAL_TIMEOUT_MULTIPLIER = 10;

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

