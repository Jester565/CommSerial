#include "PackManager.h"
#include "Packet.h"

namespace comser {
	PackManager::PackManager()
	{
	}

	void PackManager::RunCallback(uint8_t id, std::shared_ptr<ObjStream> in)
	{
		auto it = callbacks.find(id);
		if (it != callbacks.end()) {
			std::shared_ptr<Packet> packet = it->second.first->Create();
			packet->Unpack(in);
			it->second.second(packet);
		}
	}

	void PackManager::LinkCallback(uint8_t id, Packet * packet, PacketCallback callback)
	{
		callbacks.emplace(std::make_pair(id, std::make_pair(packet, callback)));
	}
}
