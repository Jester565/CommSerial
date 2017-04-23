#include "ObjStream.h"


namespace comser {
	ObjStream::ObjStream()
		:idx(0)
	{
		int num = 1;
		littleEndian = (*(char *)&num == 1);
	}

	ObjStream & ObjStream::operator<<(int8_t data)
	{
		Pack(data);
		return *this;
	}

	ObjStream & ObjStream::operator<<(uint8_t data)
	{
		Pack(data);
		return *this;
	}

	ObjStream & ObjStream::operator<<(int16_t data)
	{
		Pack(data);
		return *this;
	}

	ObjStream & ObjStream::operator<<(uint16_t data)
	{
		Pack(data);
		return *this;
	}

	ObjStream & ObjStream::operator<<(int32_t data)
	{
		Pack(data);
		return *this;
	}

	ObjStream & ObjStream::operator<<(uint32_t data)
	{
		Pack(data);
		return *this;
	}

	ObjStream & ObjStream::operator<<(int64_t data)
	{
		Pack(data);
		return *this;
	}

	ObjStream & ObjStream::operator<<(uint64_t data)
	{
		Pack(data);
		return *this;
	}

	ObjStream & ObjStream::operator<<(float data)
	{
		Pack(data);
		return *this;
	}

	ObjStream & ObjStream::operator<<(double data)
	{
		Pack(data);
		return *this;
	}

	ObjStream & ObjStream::operator<<(const std::string & str)
	{
		if (idx + str.size() > MAX_MSG_SIZE) {
			std::cerr << "Buffer size exceeded" << std::endl;
			return *this;
		}
		buffer.insert(buffer.end(), str.begin(), str.end());
		buffer.push_back('\0');
		idx += str.size() + 1;
		return *this;
	}

	ObjStream & ObjStream::operator>>(int8_t & data)
	{
		Unpack(data);
		return *this;
	}

	ObjStream & ObjStream::operator>>(uint8_t & data)
	{
		Unpack(data);
		return *this;
	}

	ObjStream & ObjStream::operator>>(int16_t & data)
	{
		Unpack(data);
		return *this;
	}

	ObjStream & ObjStream::operator>>(uint16_t & data)
	{
		Unpack(data);
		return *this;
	}

	ObjStream & ObjStream::operator>>(int32_t & data)
	{
		Unpack(data);
		return *this;
	}

	ObjStream & ObjStream::operator>>(uint32_t & data)
	{
		Unpack(data);
		return *this;
	}

	ObjStream & ObjStream::operator>>(int64_t & data)
	{
		Unpack(data);
		return *this;
	}

	ObjStream & ObjStream::operator>>(uint64_t & data)
	{
		Unpack(data);
		return *this;
	}

	ObjStream & ObjStream::operator>>(float& data)
	{
		Unpack(data);
		return *this;
	}

	ObjStream & ObjStream::operator>>(double& data)
	{
		Unpack(data);
		return *this;
	}

	ObjStream & ObjStream::operator>>(std::string & data)
	{
		while (idx < buffer.size() && buffer.at(idx) != '\0') {
			data.push_back(buffer.at(idx));
			idx++;
		}
		idx++;
		return *this;
	}
}