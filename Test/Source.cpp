#include <iostream>
#include <string>
#include <thread>
#include "TestPack.h"
#include "LargePack.h"
#include "ObjStream.h"
#include "Parser.h"
#include "SerialConnection.h"
#include "PackManager.h"

using namespace comser;

static const int MAX_IN_SIZE = 50;

void TestPackHandler(std::shared_ptr<Packet> pack) {
	std::shared_ptr<TestPack> testPack = std::static_pointer_cast<TestPack>(pack);
	std::cout << "Num1: " << testPack->num1 << std::endl;
	std::cout << "Num2: " << testPack->num2 << std::endl;
}

void LargePackHandler(std::shared_ptr<Packet> pack) {
	std::shared_ptr<LargePack> largePack = std::static_pointer_cast<LargePack>(pack);
	std::cout << "Longitude: " << largePack->longitude << std::endl;
	std::cout << "Latitutde: " << largePack->latitude << std::endl;
	std::cout << "Altitude: " << largePack->altitude << std::endl;
}

int main() {
	SerialConnection serCon1;
	if (!serCon1.Start("COM5", 38400)) {
		return 1;
	}
	serCon1.GetPackManager()->LinkCallback(new TestPack(), &TestPackHandler);
	serCon1.GetPackManager()->LinkCallback(new LargePack(), &LargePackHandler);

	SerialConnection serCon2;
	if (!serCon2.Start("COM6", 38400)) {
		return 1;
	}
	serCon2.GetPackManager()->LinkCallback(new TestPack(), &TestPackHandler);
	serCon2.GetPackManager()->LinkCallback(new LargePack(), &LargePackHandler);

	while (true) {
		std::cout << "Input: ";
		std::string userInput;
		if (std::getline(std::cin, userInput)) {
			std::shared_ptr<TestPack> testPack = std::make_shared<TestPack>();
			testPack->num1 = 100;
			testPack->num2 = 12.14;
			//serCon1.Send(testPack);
			std::shared_ptr<LargePack> largePack = std::make_shared<LargePack>();
			largePack->longitude = UINT32_MAX - 1;
			largePack->latitude = UINT32_MAX - 2;
			largePack->altitude = UINT32_MAX - 1000;
			serCon1.Send(largePack);
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