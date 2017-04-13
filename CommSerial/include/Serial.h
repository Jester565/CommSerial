#pragma once
#include <string>

class Serial
{
public:
	Serial()
		:open(false)
	{

	}
	virtual bool Open(const std::string& portName) = 0;

	virtual int Write(const uint8_t* data, uint16_t size) = 0;

	virtual int Read(uint8_t* data, uint16_t maxSize) = 0;

	virtual int Read(uint8_t* data, uint16_t maxSize, uint16_t& size) = 0;

	virtual bool IsOpen() {
		return open;
	}

	virtual bool Close() = 0;

	virtual ~Serial() {

	}

protected:
	bool open;
};