#include "BlazeEngine/DataStructures/ByteStream.h"

namespace Blaze
{
	ByteStream::ByteStream()
		: offset(0)
	{
	}
	ByteStream::ByteStream(const ByteStream& bs)
		: offset(bs.offset), data(bs.data)
	{
	}
	ByteStream::ByteStream(ByteStream&& bs)
		: offset(bs.offset), data(std::move(bs.data))
	{
	}
	ByteStream::ByteStream(const std::vector<uint8>& data, uint offset)
		: offset(offset), data(data)
	{
	}
	ByteStream::ByteStream(std::vector<uint8>&& data, uint offset)
		: offset(offset), data(std::move(data))
	{
	}
	ByteStream::ByteStream(void* data, size_t size, uint offet)
		: offset(offset), data((uint8*)data, (uint8*)data + size)
	{
	}
	ByteStream::~ByteStream()
	{
	}
	ByteStream& ByteStream::Get(void* ptr, uint size)
	{
		if (data.size() < offset + size)
			data.resize(offset + size);
		memcpy(ptr, data.data() + offset, size);
		offset += size;
		return *this;
	}
	ByteStream& ByteStream::Set(const void* ptr, uint size)
	{
		if (data.size() < offset + size)
			data.resize(offset + size);
		memcpy(data.data() + offset, ptr, size);
		offset += size;
		return *this;
	}
	void ByteStream::operator=(const ByteStream& bs)
	{
		offset = bs.offset;
		data = bs.data;
	}
	void ByteStream::operator=(ByteStream&& bs)
	{
		offset = bs.offset;
		data = std::move(bs.data);
	}
}