#include <Packet.h>

class LargePack : public comser::Packet {
public:
	LargePack();

	void Pack(std::shared_ptr<comser::ObjStream> out) override;

	void Unpack(std::shared_ptr<comser::ObjStream> in) override;

	std::shared_ptr<comser::Packet> Create() override;

	uint32_t longitude;
	uint32_t latitude;
	uint32_t altitude;
};
