#include "SerialConnection.h"
#include "ObjStream.h"
#include "Parser.h"
#include "Serial.h"
#include "Packet.h"
#include "PackManager.h"
#include <iostream>

namespace comser {
	const int SerialConnection::RECV_SLEEP_MILLIS = 50;

	SerialConnection::SerialConnection()
		:recvThread(nullptr), sendThread(nullptr), recvHandler(nullptr), errHandler(nullptr)
	{
		recvHandler = std::bind(&SerialConnection::DefaultRecvHandler, this, std::placeholders::_1, std::placeholders::_2);
		errHandler = std::bind(&SerialConnection::DefaultErrHandler, this, std::placeholders::_1);
		serial = Serial::CreateSerial();
		parser = new Parser();
		packManager = new PackManager();
	}

	bool SerialConnection::Start(const std::string & portName, uint32_t baudrate)
	{
		if (!serial->Open(portName, baudrate)) {
			std::cerr << "Failed to open serial" << std::endl;
			return false;
		}
		if (!recvHandler) {
			std::cerr << "recv handler is null" << std::endl;
			return false;
		}
		if (!errHandler) {
			std::cerr << "err handler is null" << std::endl;
			return false;
		}
		sendThread = new std::thread(std::bind(&SerialConnection::SendRun, this));
		recvThread = new std::thread(std::bind(&SerialConnection::RecvRun, this));
		sendThread->detach();
		recvThread->detach();
		return true;
	}

	SerialConnection::~SerialConnection()
	{
		delete recvThread;
		recvThread = nullptr;
		delete sendThread;
		sendThread = nullptr;
		delete serial;
		serial = nullptr;
		delete parser;
		parser = nullptr;
		delete packManager;
		packManager = nullptr;
	}

	void SerialConnection::Send(std::shared_ptr<Packet> data)
	{
		sendQueueMutex.lock();
		sendQueue.push(data);
		sendQueueMutex.unlock();
		sendCondVar.Set();
	}

	void SerialConnection::DefaultRecvHandler(uint8_t id, std::shared_ptr<ObjStream> stream)
	{
		packManager->RunCallback(id, stream);
	}

	void SerialConnection::DefaultErrHandler(int err)
	{
		std::cerr << "AN ERROR OCCURED WITH CODE: " << err << std::endl;
	}

	void SerialConnection::SendRun()
	{
		while (running) {
			std::shared_ptr<Packet> sendPack;
			bool send = false;
			bool empty = true;
			sendQueueMutex.lock();
			if (!sendQueue.empty()) {
				sendPack = sendQueue.front();
				sendQueue.pop();
				send = true;
				empty = sendQueue.empty();
			}
			sendQueueMutex.unlock();
			if (send) {
				int writeVal = parser->Write(serial, sendPack);
				if (writeVal < 0) {
					errHandler(writeVal);
				}
			}
			if (empty)
			{
				sendCondVar.Wait();
			}
		}
	}

	void SerialConnection::RecvRun()
	{
		while (running) {
			std::shared_ptr<ObjStream> packStream;
			uint8_t id = 0;
			int readVal = parser->Read(serial, id, packStream, packManager);
			if (readVal < 0) {
				errHandler(readVal);
			}
			else if (readVal > 0) {
				recvHandler(id, packStream);
			}
			recvCondVar.Wait(std::chrono::milliseconds(RECV_SLEEP_MILLIS));
		}
	}
}
