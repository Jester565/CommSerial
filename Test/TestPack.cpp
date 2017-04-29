#include "TestPack.h"
#include "ObjStream.h"

TestPack::TestPack()
	:Packet(0, 8)
{

}

void TestPack::Pack(std::shared_ptr<comser::ObjStream> out)
{
	*out << num1 << num2;
}

void TestPack::Unpack(std::shared_ptr<comser::ObjStream> in)
{
	*in >> num1 >> num2;
}

std::shared_ptr<comser::Packet> TestPack::Create()
{
	return std::shared_ptr<TestPack>();
}
