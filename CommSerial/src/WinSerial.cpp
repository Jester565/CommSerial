#ifdef _WIN32
#include "Serial.h"
#include <Windows.h>
#include <iostream>

namespace comser {
	class WinSerial : public Serial {
	public:
		WinSerial()
			:Serial()
		{
		}

		bool Open(const std::string& portName, uint32_t baudrate) override
		{
			hComm = CreateFile(("\\\\.\\" + portName).c_str(), GENERIC_READ | GENERIC_WRITE,
				0, 0, OPEN_EXISTING, 0, 0);
			if (hComm == INVALID_HANDLE_VALUE) {
				std::cerr << "Could not create serial handle" << std::endl;
				return false;
			}


			DCB serialParams = { 0 };
			serialParams.DCBlength = sizeof(serialParams);
			GetCommState(hComm, &serialParams);
			serialParams.BaudRate = baudrate;
			serialParams.ByteSize = BYTE_SIZE;
			serialParams.StopBits = STOP_BIT;
			serialParams.Parity = PARITY;
			if (!SetCommState(hComm, &serialParams)) {
				std::cerr << "Could not set serial parameters" << std::endl;
				Close();
			}

			COMMTIMEOUTS serialTimeouts = { 0 };
			serialTimeouts.ReadIntervalTimeout = READ_INTERVAL_TIMEOUT;
			serialTimeouts.ReadTotalTimeoutConstant = READ_TOTAL_TIMEOUT;
			serialTimeouts.ReadTotalTimeoutMultiplier = READ_TOTAL_TIMEOUT_MULTIPLIER;
			serialTimeouts.WriteTotalTimeoutConstant = WRITE_TOTAL_TIMEOUT_CONSTANT;
			serialTimeouts.WriteTotalTimeoutMultiplier = WRITE_TOTAL_TIMEOUT_MULTIPLIER;
			if (!SetCommTimeouts(hComm, &serialTimeouts)) {
				std::cerr << "Could not set serial timeouts" << std::endl;
				Close();
			}

			open = true;
			return true;
		}

		int Write(const uint8_t * data, uint16_t size) override
		{
			DWORD dwWritten;
			if (!WriteFile(hComm, data, size, &dwWritten, NULL))
			{
				if (GetLastError() != ERROR_IO_PENDING) {
					return -1;
				}
			}
			return 1;
		}

		int Read(uint8_t * data, uint16_t maxSize) override
		{
			DWORD dwRead;
			//hComm is serial file handle, data is buffer to receive, maxSize is maximum size of buffer
			//dwRead is the amount of bytes received, NULL would be a event if this was in overlapped mode
			if (!ReadFile(hComm, data, maxSize, &dwRead, NULL))
			{
				if (GetLastError() != ERROR_IO_PENDING) {
					return -1;
				}
			}
			return dwRead;
		}

		bool Close() override
		{
			if (!CloseHandle(hComm)) {
				std::cerr << "Could not close serial connection" << std::endl;
				return false;
			}
			open = false;
			return true;
		}

		~WinSerial()
		{
			if (IsOpen()) {
				Close();
			}
		}

		private:
			void* hComm;
	};

	Serial* Serial::CreateSerial() {
		return new WinSerial();
	}
}
#endif
