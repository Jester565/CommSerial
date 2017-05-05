#include <iostream>
#include <string>
#include <thread>
#include "Serial.h"
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
	if (!serCon1.Start("COM7", 38400)) {
		return 1;
	}

	serCon1.GetSerial()->Write("TT1;", 4);
	serCon1.GetParser()->SetSendPrefix("KYW");
	serCon1.GetParser()->SetSendPostfix(";");
	serCon1.GetPackManager()->LinkCallback(new TestPack(), &TestPackHandler);
	serCon1.GetPackManager()->LinkCallback(new LargePack(), &LargePackHandler);

	while (true) {
		std::cout << "Press enter to send" << std::endl;
		char c[100];
		std::cin.getline(c, 100);
		std::shared_ptr<LargePack> largePack = std::make_shared<LargePack>();
		largePack->longitude = 20;
		largePack->latitude = 100;
		largePack->altitude = 10342;
		serCon1.Send(largePack);
		std::cout << "SENT" << std::endl;
		std::this_thread::sleep_for(std::chrono::milliseconds(500));
	}
}
