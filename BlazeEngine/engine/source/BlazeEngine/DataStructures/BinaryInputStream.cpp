#include "BlazeEngine/DataStructures/BinaryInputStream.h"
#include <utility>
#include <cstring>

namespace Blaze
{
	BinaryInputStream::BinaryInputStream()
		: offset(0)
	{

	}

	void BinaryInputStream::SetOffset(size_t offset)
	{
		this->offset = offset;
	}
	void BinaryInputStream::TakeBuffer(Buffer& buffer)
	{
		this->buffer = std::move(buffer);
	}

	void BinaryInputStream::ReturnBuffer(Buffer& buffer)
	{
		buffer = std::move(this->buffer);
	}

	void BinaryInputStream::ReadString(String& string)
	{
		size_t size;
		Read(size);

		memcpy(string.Ptr(), (char*)buffer.Ptr() + offset, size);
		offset += size;
	}

	template<typename T> 
	void Read(T& value, size_t& offset, Buffer& buffer)
	{
		value = *(T*)((char*)buffer.Ptr() + offset);
		offset += sizeof(value);
	}


	void BinaryInputStream::Read(bool& out)		{ Blaze::Read(out, offset, buffer); }
	void BinaryInputStream::Read(int8& out)		{ Blaze::Read(out, offset, buffer); }
	void BinaryInputStream::Read(int16& out)	{ Blaze::Read(out, offset, buffer); }
	void BinaryInputStream::Read(int32& out)	{ Blaze::Read(out, offset, buffer); }
	void BinaryInputStream::Read(int64& out)	{ Blaze::Read(out, offset, buffer); }
	void BinaryInputStream::Read(uint8& out)	{ Blaze::Read(out, offset, buffer); }
	void BinaryInputStream::Read(uint16& out)	{ Blaze::Read(out, offset, buffer); }
	void BinaryInputStream::Read(uint32& out)	{ Blaze::Read(out, offset, buffer); }
	void BinaryInputStream::Read(uint64& out)	{ Blaze::Read(out, offset, buffer); }
	void BinaryInputStream::Read(float& out)	{ Blaze::Read(out, offset, buffer); }
	void BinaryInputStream::Read(double& out)	{ Blaze::Read(out, offset, buffer); }
}