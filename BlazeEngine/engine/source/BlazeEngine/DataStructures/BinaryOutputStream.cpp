#include "BlazeEngine/DataStructures/BinaryOutputStream.h"
#include <utility>
#include <cstring>

namespace Blaze
{
	BinaryOutputStream::BinaryOutputStream()
		: offset(0)
	{

	}

	void BinaryOutputStream::SetOffset(size_t offset)
	{
		this->offset = offset;
	}

	const Buffer& BinaryOutputStream::GetBuffer()
	{		
		return buffer;		
	}

	void BinaryOutputStream::WriteString(StringView string)
	{
		Write(string.Size());
		
		size_t newOffset = offset + string.Size();
		if (newOffset > buffer.Size())
		{
			Buffer oldBuffer = std::move(buffer);
			buffer.Allocate(oldBuffer.Size() + 64);

			memcpy(buffer.Ptr(), oldBuffer.Ptr(), buffer.Size());
		}

		memcpy((char*)buffer.Ptr() + offset, string.Ptr(), string.Size());

		offset = newOffset;
	}

	template<typename T>
	void Write(const T& value, size_t& offset, Buffer& buffer)
	{
		size_t newOffset = offset + sizeof(value);
		if (newOffset > buffer.Size())
		{
			Buffer oldBuffer = std::move(buffer);
			buffer.Allocate(oldBuffer.Size() + 64);

			memcpy(buffer.Ptr(), oldBuffer.Ptr(), buffer.Size());
		}

		*(T*)((char*)buffer.Ptr() + offset) = value;

		offset = newOffset;
	}

	void BinaryOutputStream::Write(const bool& out) { Blaze::Write(out, offset, buffer); }
	void BinaryOutputStream::Write(const int8& out) { Blaze::Write(out, offset, buffer); }
	void BinaryOutputStream::Write(const int16& out) { Blaze::Write(out, offset, buffer); }
	void BinaryOutputStream::Write(const int32& out) { Blaze::Write(out, offset, buffer); }
	void BinaryOutputStream::Write(const int64& out) { Blaze::Write(out, offset, buffer); }
	void BinaryOutputStream::Write(const uint8& out) { Blaze::Write(out, offset, buffer); }
	void BinaryOutputStream::Write(const uint16& out) { Blaze::Write(out, offset, buffer); }
	void BinaryOutputStream::Write(const uint32& out) { Blaze::Write(out, offset, buffer); }
	void BinaryOutputStream::Write(const uint64& out) { Blaze::Write(out, offset, buffer); }
	void BinaryOutputStream::Write(const float& out) { Blaze::Write(out, offset, buffer); }
	void BinaryOutputStream::Write(const double& out) { Blaze::Write(out, offset, buffer); }
}