#include <iostream>
#include <string>
#include <thread>
#include "SerialConnection.h"

using namespace comser;

void RHandler(std::vector<uint8_t>& data) {
	for (int i = 0; i < data.size(); i++) {
		std::cout << data.at(i);
	}
	std::cout << std::endl;
}

int main() {
	SerialConnection serCon1;
	serCon1.SetRecvHandler(&RHandler);
	if (!serCon1.Start("COM4")) {
		return 1;
	}
	SerialConnection serCon2;
	serCon2.SetRecvHandler(&RHandler);
	if (!serCon2.Start("COM5")) {
		return 1;
	}
	while (true) {
		std::cout << "Input: ";
		std::string userInput;
		if (std::cin >> userInput) {
			std::vector<uint8_t> vec(userInput.begin(), userInput.end());
			serCon1.Send(vec);
			std::this_thread::sleep_for(std::chrono::milliseconds(500));
		}
		else
		{
			std::cin.clear();
			std::cin.ignore(std::numeric_limits<std::streamsize>::max());
		}
	}
	system("pause");
}

/*
Parser parser1;
WinSerial serial1;
if (!serial1.Open("COM4")) {
	std::cerr << "failed to open com4" << std::endl;
}
Parser parser2;
WinSerial serial2;
if (!serial2.Open("COM5")) {
	std::cerr << "failed to open com5" << std::endl;
}
std::string str = "speed";
std::vector<uint8_t> data(str.begin(), str.end());
if (parser1.Write(&serial1, data) < 0) {
	std::cerr << "failed to send" << std::endl;
}
std::this_thread::sleep_for(std::chrono::milliseconds(800));
std::vector<uint8_t> recvData;
if (parser2.Read(&serial2, recvData) > 0) {
	for (int i = 0; i < recvData.size(); i++) {
		std::cout << data[i];
	}
	std::cout << std::endl;
}
*/