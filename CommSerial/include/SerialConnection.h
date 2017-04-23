#pragma once
#include "stdafx.h"
#include "ConditionVariable.h"
#include <string>
#include <functional>
#include <thread>
#include <memory>
#include <vector>
#include <mutex>
#include <queue>

namespace comser {
	class Serial;
	class Parser;
	class ObjStream;

	static const uint32_t BAUDRATE = 57600;

	typedef std::function<void(std::shared_ptr<ObjStream>)> RecvHandler;
	typedef std::function<void(int)> ErrHandler;
	class SerialConnection {
	public:
		static const int RECV_SLEEP_MILLIS;
		SerialConnection();

		virtual bool Start(const std::string& portName, uint32_t baudrate = BAUDRATE);

		void SetRecvHandler(const RecvHandler& recvHandler) {
			this->recvHandler = recvHandler;
		}

		void SetErrorHandler(const ErrHandler& errHandler) {
			this->errHandler = errHandler;
		}

		void Send(std::shared_ptr<ObjStream> data);

		void DefaultErrHandler(int err);

		Parser* GetParser() {
			return parser;
		}

		Serial* GetSerial() {
			return serial;
		}

		virtual ~SerialConnection();
	protected:
		Serial* serial;
		Parser* parser;
		std::queue<std::shared_ptr<ObjStream>> sendQueue;
		std::mutex sendQueueMutex;
		virtual void SendRun();
		virtual void RecvRun();
		std::thread* sendThread;
		std::thread* recvThread;
		RecvHandler recvHandler;
		ErrHandler errHandler;
		std::atomic<bool> running;
		CommConditionVariable sendCondVar;
		CommConditionVariable recvCondVar;
	};
}