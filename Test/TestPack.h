#include <Packet.h>

class TestPack : public comser::Packet{
public:
	TestPack();

	void Pack(std::shared_ptr<comser::ObjStream> out) override;

	void Unpack(std::shared_ptr<comser::ObjStream> in) override;

	std::shared_ptr<comser::Packet> Create() override;

	int num1;
	float num2;
};