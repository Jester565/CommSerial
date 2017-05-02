#pragma once
#include "stdafx.h"
#include <iostream>
#include <memory>

namespace comser {
	class ObjStream;

	class Packet {
	public:
		Packet(uint8_t id, uint8_t size)
			:id(id), size(size)
		{
			if (size > 12) {
				std::cerr << "Packet size cannot be greater than 12" << std::endl;
			}
			else if (size == 12 && id > 4) {
				std::cerr << "Packet size of 12 must have an id between 0 and 3" << std::endl;
			}
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
