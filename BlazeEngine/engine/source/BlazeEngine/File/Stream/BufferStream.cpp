#include "BlazeEngine/File/Stream/BufferStream.h"

namespace Blaze
{
	Blaze::BufferStreamBase::BufferStreamBase()
		: buffer(nullptr), size(0), position(0)
	{
	}
	BufferStreamBase::BufferStreamBase(uint64 size)
		: size(size), position(0)
	{
		buffer = malloc(size);		
	}
	BufferStreamBase::BufferStreamBase(void* buffer, uint64 size)
		: buffer(buffer), size(size), position(0)
	{
	}
	BufferStreamBase::~BufferStreamBase()
	{
		Clear();
	}
	Result BufferStreamBase::SetBuffer(void* buffer, uint64 size)
	{
		CHECK_RESULT(Clear());
		this->buffer = buffer;
		this->size = size;
		this->position = 0;

		return Result();
	}
	Result BufferStreamBase::Clear()
	{
		free(this->buffer);
		this->size = 0;
		this->position = 0;

		return Result();
	}
	bool BufferStreamBase::MovePosition(int64 offset)
	{
		uint64 newPos = position + offset;

		if (offset >= size || newPos >= size)
			return false;
		
		position = newPos;

		return true;
	}
	bool BufferStreamBase::SetPosition(uint64 offset)
	{
		if (offset >= size)
			return false;

		position = offset;

		return true;
	}
	uint64 BufferStreamBase::GetPosition() const
	{
		return position;
	}
	uint64 BufferStreamBase::GetSize() const
	{
		return size;
	}
	const void* BufferStreamBase::GetBuffer() const
	{
		return buffer;
	}	

	BufferWriteStream::BufferWriteStream()
	{
	}
	BufferWriteStream::BufferWriteStream(uint64 size)
		: BufferStreamBase(size)
	{

	}
	BufferWriteStream::BufferWriteStream(void* buffer, uint64 size)
		: BufferStreamBase(buffer, size)
	{

	}
	BufferWriteStream::~BufferWriteStream()
	{
	}
	uint64 BufferWriteStream::Write(const void* ptr, uint64 byteCount)
	{
		uint64 newSize = size;
		uint leftBytes = newSize - position;

		while (leftBytes < byteCount)
		{
			newSize *= 2;
			leftBytes = size - position;
		}

		if (newSize != size)
		{
			void* old = buffer;
			buffer = malloc(newSize);
			memcpy(buffer, old, size);
			size = newSize;
		}

		memcpy((char*)buffer + position, ptr, byteCount);

		position += byteCount;

		return byteCount;
	}

	BufferReadStream::BufferReadStream()
	{
	}
	BufferReadStream::BufferReadStream(uint64 size)
		: BufferStreamBase(size)
	{

	}
	BufferReadStream::BufferReadStream(void* buffer, uint64 size)
		: BufferStreamBase(buffer, size)
	{

	}
	BufferReadStream::~BufferReadStream()
	{
	}
	uint64 BufferReadStream::Read(void* ptr, uint64 byteCount)
	{
		uint position = GetPosition();
		uint leftBytes = GetSize() - position;

		if (leftBytes < byteCount)
			byteCount = leftBytes;

		memcpy(ptr, (char*)GetBuffer() + position, byteCount);
		MovePosition(byteCount);

		return byteCount;
	}
}