#pragma once
#include <string>
#include <vector>

namespace comser {

	static const uint32_t BYTE_SIZE = 8;
	static const uint32_t STOP_BIT = 0;
	static const uint32_t PARITY = 0;

	static const uint32_t READ_INTERVAL_TIMEOUT = 1;
	static const uint32_t READ_TOTAL_TIMEOUT = 1;
	static const uint32_t READ_TOTAL_TIMEOUT_MULTIPLIER = 1;
	static const uint32_t WRITE_TOTAL_TIMEOUT_CONSTANT = 1;
	static const uint32_t WRITE_TOTAL_TIMEOUT_MULTIPLIER = 1;

	class Serial
	{
	public:
		static Serial* CreateSerial();

		Serial()
			:open(false)
		{

		}
		virtual bool Open(const std::string& portName, uint32_t baudrate) = 0;

		virtual int Write(const uint8_t* data, uint16_t size) = 0;

		virtual int Read(uint8_t* data, uint16_t maxSize) = 0;

		virtual int Read(uint8_t* data, uint16_t maxSize, uint16_t& size) {
			int rVal = Read(data, maxSize);
			if (rVal <= 0) {
				return rVal;
			}
			else
			{
				size = rVal;
				return 1;
			}
		}

		virtual bool IsOpen() {
			return open;

		}

		virtual bool Close() = 0;

		virtual ~Serial() { 

		}

	protected:
		bool open;
	};
}
