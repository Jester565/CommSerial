#include "SerialConnection.h"
#include "ObjStream.h"
#include "Parser.h"
#include "Serial.h"
#include <iostream>

namespace comser {
	const int SerialConnection::RECV_SLEEP_MILLIS = 50;

	SerialConnection::SerialConnection()
		:recvThread(nullptr), sendThread(nullptr), recvHandler(nullptr), errHandler(nullptr)
	{
		errHandler = std::bind(&SerialConnection::DefaultErrHandler, this, std::placeholders::_1);
		serial = Serial::CreateSerial();
		parser = new Parser();
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
	}

	void SerialConnection::Send(std::shared_ptr<ObjStream> data)
	{
		sendQueueMutex.lock();
		sendQueue.push(data);
		sendQueueMutex.unlock();
		sendCondVar.Set();
	}

	void SerialConnection::DefaultErrHandler(int err)
	{
		std::cerr << "AN ERROR OCCURED WITH CODE: " << err << std::endl;
	}

	void SerialConnection::SendRun()
	{
		while (running) {
			std::shared_ptr<ObjStream> sendData;
			bool send = false;
			bool empty = true;
			sendQueueMutex.lock();
			if (!sendQueue.empty()) {
				sendData = sendQueue.front();
				sendQueue.pop();
				send = true;
				empty = sendQueue.empty();
			}
			sendQueueMutex.unlock();
			if (send) {
				int writeVal = parser->Write(serial, sendData);
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
			std::shared_ptr<ObjStream> recvData = std::make_shared<ObjStream>();
			int readVal = parser->Read(serial, recvData);
			if (readVal < 0) {
				errHandler(readVal);
			}
			else if (readVal > 0) {
				recvHandler(recvData);
			}
			recvCondVar.Wait(std::chrono::milliseconds(RECV_SLEEP_MILLIS));
		}
	}
}
