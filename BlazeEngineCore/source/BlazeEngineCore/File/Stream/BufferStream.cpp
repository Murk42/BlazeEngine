#include "pch.h"
#include "BlazeEngineCore/File/Stream/BufferStream.h"

namespace Blaze
{
	Blaze::BufferStreamBase::BufferStreamBase()
		: buffer(nullptr), size(0), position(0)
	{
	}
	BufferStreamBase::BufferStreamBase(uintMem size)
		: size(size), position(0)
	{
		buffer = malloc(size);		
	}
	BufferStreamBase::BufferStreamBase(void* buffer, uintMem size)
		: buffer(buffer), size(size), position(0)
	{
	}
	BufferStreamBase::~BufferStreamBase()
	{
		Clear();
	}
	Result BufferStreamBase::SetBuffer(void* buffer, uintMem size)
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
	bool BufferStreamBase::MovePosition(intMem offset)
	{
		uintMem newPos = position + offset;

		if (std::cmp_greater_equal(offset, size) || newPos >= size)
			return false;
		
		position = newPos;

		return true;
	}
	bool BufferStreamBase::SetPosition(uintMem offset)
	{
		if (offset >= size)
			return false;

		position = offset;

		return true;
	}
	uintMem BufferStreamBase::GetPosition() const
	{
		return position;
	}
	uintMem BufferStreamBase::GetSize() const
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
	BufferWriteStream::BufferWriteStream(uintMem size)
		: BufferStreamBase(size)
	{

	}
	BufferWriteStream::BufferWriteStream(void* buffer, uintMem size)
		: BufferStreamBase(buffer, size)
	{

	}
	BufferWriteStream::~BufferWriteStream()
	{
	}
	uintMem BufferWriteStream::Write(const void* ptr, uintMem byteCount)
	{
		uintMem newSize = size;
		uintMem leftBytes = newSize - position;

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
	BufferReadStream::BufferReadStream(uintMem size)
		: BufferStreamBase(size)
	{

	}
	BufferReadStream::BufferReadStream(void* buffer, uintMem size)
		: BufferStreamBase(buffer, size)
	{

	}
	BufferReadStream::~BufferReadStream()
	{
	}
	uintMem BufferReadStream::Read(void* ptr, uintMem byteCount)
	{
		uintMem position = GetPosition();
		uintMem leftBytes = GetSize() - position;

		if (leftBytes < byteCount)
			byteCount = leftBytes;

		memcpy(ptr, (char*)GetBuffer() + position, byteCount);
		MovePosition(byteCount);

		return byteCount;
	}
}