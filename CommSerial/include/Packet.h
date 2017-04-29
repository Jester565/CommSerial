#pragma once
#include "stdafx.h"
#include <memory>

namespace comser {
	class ObjStream;

	class Packet {
	public:
		Packet(uint8_t id, uint8_t size)
			:id(id), size(size)
		{

		}

		virtual void Pack(std::shared_ptr<ObjStream> out) = 0;

		virtual void Unpack(std::shared_ptr<ObjStream> in) = 0;

		virtual std::shared_ptr<Packet> Create() = 0;

		uint8_t GetSize() {
			return size;
		}

		uint8_t GetID() {
			return id;
		}

		virtual ~Packet() {

		}

	protected:
		uint8_t id;
		uint8_t size;
	};
}
