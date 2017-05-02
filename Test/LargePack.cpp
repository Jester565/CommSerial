#include "LargePack.h"
#include "ObjStream.h"

LargePack::LargePack()
	:Packet(1, 12)
{

}

void LargePack::Pack(std::shared_ptr<comser::ObjStream> out)
{
	*out << longitude << latitude << altitude;
}

void LargePack::Unpack(std::shared_ptr<comser::ObjStream> in)
{
	*in >> longitude >> latitude >> altitude;
}

std::shared_ptr<comser::Packet> LargePack::Create()
{
	return std::make_shared<LargePack>();
}
