#include <iostream>
#include <string>
#include <thread>
#include "ObjStream.h"
#include "SerialConnection.h"

using namespace comser;

static const int MAX_IN_SIZE = 50;

void RHandler(std::shared_ptr<ObjStream> data) {
	std::string str;
	*data >> str;
	std::cout << str << std::endl;
}

int main() {
	SerialConnection serCon1;
	serCon1.SetRecvHandler(&RHandler);
	if (!serCon1.Start("/dev/ttyS10", 38400)) {
		return 1;
	}
	SerialConnection serCon2;
	serCon2.SetRecvHandler(&RHandler);
	if (!serCon2.Start("/dev/ttyS11", 38400)) {
		return 1;
	}
	
	while (true) {
		std::cout << "Input: ";
		std::string userInput;
		if (std::getline(std::cin, userInput)) {
			std::shared_ptr<ObjStream> sendData = std::make_shared<ObjStream>();
			*sendData << userInput;
			serCon1.Send(sendData);
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
