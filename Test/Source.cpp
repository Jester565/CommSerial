#include <iostream>
#include <string>
#include <thread>
#include "SerialConnection.h"

using namespace comser;

static const int MAX_IN_SIZE = 50;

void RHandler(std::string data) {
	for (int i = 0; i < data.size(); i++) {
		std::cout << data.at(i);
	}
	std::cout << std::endl;
}

int main() {
	SerialConnection serCon1;
	serCon1.SetRecvHandler(&RHandler);
	if (!serCon1.Start("COM5")) {
		return 1;
	}
	SerialConnection serCon2;
	serCon2.SetRecvHandler(&RHandler);
	if (!serCon2.Start("COM6")) {
		return 1;
	}
	
	while (true) {
		std::cout << "Input: ";
		std::string userInput;
		if (std::getline(std::cin, userInput)) {
			serCon1.Send(userInput);
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
