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

	void PackManager::LinkCallback(Packet * packet, PacketCallback callback)
	{
		callbacks.emplace(std::make_pair(packet->GetID(), std::make_pair(packet, callback)));
	}
	bool PackManager::GetPackSize(uint8_t id, uint8_t & size)
	{
		auto it = callbacks.find(id);
		if (it != callbacks.end()) {
			size = it->second.first->GetSize();
			return true;
		}
		return false;
	}
}
