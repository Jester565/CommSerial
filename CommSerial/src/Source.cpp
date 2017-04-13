#include "WinSerial.h"
#include <iostream>
#include <string>
#include <thread>

int main() {
	WinSerial serial1;
	if (!serial1.Open("COM4")) {
		std::cerr << "failed to open com4" << std::endl;
	}
	WinSerial serial2;
	if (!serial2.Open("COM5")) {
		std::cerr << "failed to open com5" << std::endl;
	}
	if (serial1.Write((const uint8_t*)"speed", 5) < 0) {
		std::cerr << "failed to send" << std::endl;
	}
	std::this_thread::sleep_for(std::chrono::milliseconds(800));
	uint16_t size;
	uint8_t data[20];
	if (serial2.Read(data, 20, size) > 0) {
		for (int i = 0; i < size; i++) {
			std::cout << data[i];
		}
		std::cout << std::endl;
	}
	system("pause");
}