#ifndef _WIN32
#include "Serial.h"

namespace comser {
	class LinSerial : public Serial {
		bool Open(const std::string & portName) override
		{
			return false;
		}

		int Write(const uint8_t * data, uint16_t size) override
		{
			return 0;
		}
		int Read(uint8_t * data, uint16_t maxSize) override
		{
			return 0;
		}

		bool Close() override
		{
			
		}

		~LinSerial() {
			if (IsOpen()) {
				Close();
			}
		}
	};
}
#endif
