#ifndef _WIN32
#include "Serial.h"
#include <cstring>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <iostream>

namespace comser {
	class LinSerial : public Serial {
		bool Open(const std::string & portName, uint32_t baudrate) override
		{
			//open the file to the specified port
			fileDescriptor = ::open(portName.c_str(), O_RDWR | O_NOCTTY);
			if (fileDescriptor == -1) {
				std::cerr << "Unable to open port " << portName << std::endl;
				return false;
			}
			//checks if the fileDescriptor points to a tty (text telepohone / teletype terminal / teletypewriter) devices
			if (!isatty(fileDescriptor)) 
			{
				std::cerr << "Device " << portName << " is not a serial" << std::endl;
				return false;
				Close();
			}
			struct termios config;
			memset (&config, 0, sizeof(termios)); //sets all bytes in config to 0
			if (tcgetattr(fileDescriptor, &config) < 0) //get configuration for the socket
			{
				std::cerr << "could not aquire serial attributes" << std::endl;
				Close();
				return false;
			}
			
			config.c_iflag &= ~(IGNBRK | BRKINT | ICRNL | INLCR | PARMRK | INPCK | ISTRIP | IXON);  //iflag: input modes - disables any sort of input checking
			config.c_oflag = 0;  //oflag: output modes - disables output checking
			config.c_lflag &= ~(ECHO | ECHONL | ICANON | IEXTEN | ISIG);  //lflag: local modes - disables printing status of buffer
			config.c_cflag &= ~(CSIZE | PARENB);  //cflag: control modes - clears the current byte size so that when we or the 8 bit version, we don't get leftover bits
			config.c_cflag |= CS8;  //sets byte size to 8 bits
			config.c_cc[VMIN] = 1;  //cc: special characters - The minimum amount of bytes to return from read, set to 1 (read does not block)
			config.c_cc[VTIME] = 5;  //.5 seconds before read timesout
			
			switch (baudrate) {
			case 0:      { baudrate = B0;      break; }
			case 50:     { baudrate = B50;     break; }
			case 75:     { baudrate = B75;     break; } 
			case 110:    { baudrate = B110;    break; }
			case 134:    { baudrate = B134;    break; }
			case 150:    { baudrate = B150;    break; }
			case 200:    { baudrate = B200;    break; }
			case 300:    { baudrate = B300;    break; }
			case 600:    { baudrate = B600;    break; }
			case 1200:   { baudrate = B1200;   break; }
			case 1800:   { baudrate = B1800;   break; }
			case 2400:   { baudrate = B2400;   break; }
			case 4800:   { baudrate = B4800;   break; }
			case 9600:   { baudrate = B9600;   break; }
			case 19200:  { baudrate = B19200;  break; }
			case 38400:  { baudrate = B38400;  break; }
			case 57600:  { baudrate = B57600;  break; }
			case 115200: { baudrate = B115200; break; }
			default: {
				std::cerr << "Baudrate was not standard... using 115200";
				baudrate = B115200;
			}
    		}
			
			//set input and output baudrate
			if (cfsetispeed(&config, (speed_t)baudrate) < 0 || cfsetospeed(&config, (speed_t)baudrate) < 0) {
				std::cerr << "Could not set baudrate" << std::endl;
				Close();
				return false;
			}

			cfmakeraw(&config);
			//clear previous configurations
			tcflush(fileDescriptor, TCIFLUSH);
			//apply configurations to serial
			if (tcsetattr(fileDescriptor, TCSANOW, &config) != 0) {
				std::cerr << "Could not apply attributes to serial" << std::endl;
				Close();
				return false;
			}
			open = true;
			return true;
		}

		int Write(const uint8_t * data, uint16_t size) override
		{
			return write(fileDescriptor, data, size);
		}

		int Read(uint8_t * data, uint16_t maxSize) override
		{
			int rVal = read(fileDescriptor, data, maxSize);
			if (rVal < 0) {
				fputs("read failed! - ", stderr);
				perror("perr - ");
			}
			return rVal;
		}

		bool Close() override
		{
			close(fileDescriptor);
			open = false;
			return true;
		}

		~LinSerial() {
			if (IsOpen()) {
				Close();
			}
		}
		protected:
		int fileDescriptor;
	};

	Serial* Serial::CreateSerial() {
		return new LinSerial();
	}
}
#endif
