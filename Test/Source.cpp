#include <iostream>
#include <string>
#include <thread>
#include "TestPack.h"
#include "ObjStream.h"
#include "Parser.h"
#include "SerialConnection.h"

using namespace comser;

static const int MAX_IN_SIZE = 50;

void RHandler(uint8_t id, std::shared_ptr<ObjStream> data) {
	std::cout << "Receive called" << std::endl;
}

int main() {
	SerialConnection serCon1;
	serCon1.SetRecvHandler(&RHandler);
	if (!serCon1.Start("COM3", 38400)) {
		return 1;
	}
	serCon1.GetParser()->SetSendPrefix("KYW");
	serCon1.GetParser()->SetSendPostfix(";");
	
	SerialConnection serCon2;
	serCon2.SetRecvHandler(&RHandler);
	if (!serCon2.Start("COM6", 38400)) {
		return 1;
	}
	
	while (true) {
		std::cout << "Input: ";
		std::string userInput;
		if (std::getline(std::cin, userInput)) {
			std::shared_ptr<TestPack> sendData = std::make_shared<TestPack>();
			sendData->num1 = 100;
			sendData->num2 = 12.14;
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

/*
SerialConnection serCon1;
serCon1.SetRecvHandler(&RHandler);
if (!serCon1.Start("COM3", 38400)) {
return 1;
}
serCon1.GetParser()->SetSendPrefix("KYW");
serCon1.GetParser()->SetSendPostfix(";");
serCon1.GetSerial()->Write((const uint8_t*)"TT1;", 4);
*/