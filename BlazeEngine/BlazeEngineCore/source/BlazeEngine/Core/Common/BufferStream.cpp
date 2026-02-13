#include "pch.h"
#include "BlazeEngine/Core/Common/BufferStream.h"
#include "BlazeEngine/Core/Container/Array.h"

namespace Blaze
{
	Blaze::BufferStreamBase::BufferStreamBase()
		: position(0)
	{
	}
	BufferStreamBase::BufferStreamBase(BufferStreamBase&& other) noexcept
		: buffer(std::move(other.buffer)), position(other.position)
	{
		other.position = 0;
	}
	BufferStreamBase::~BufferStreamBase()
	{
		Clear();
	}
	bool BufferStreamBase::Empty() const { return buffer.Empty(); }
	void BufferStreamBase::Clear()
	{
		buffer.Clear();
		position = 0;
	}
	bool BufferStreamBase::MovePosition(intMem offset)
	{
		uintMem newPos = position + offset;

		if (offset >= buffer.Count() || newPos >= buffer.Count())
			return false;

		position = newPos;

		return true;
	}
	bool BufferStreamBase::SetPosition(uintMem offset)
	{
		if (offset >= buffer.Count())
			return false;

		position = offset;

		return true;
	}
	bool BufferStreamBase::SetPositionFromEnd(intMem offset)
	{
		if (offset < 0 || offset >= (intMem)buffer.Count())
			return false;

		position = buffer.Count() - offset;

		return true;
	}
	uintMem BufferStreamBase::GetPosition() const
	{
		return position;
	}
	uintMem BufferStreamBase::GetSize() const
	{
		return buffer.Count();
	}
	void* BufferStreamBase::GetPtr()
	{
		return buffer.Ptr();
	}
	const void* BufferStreamBase::GetPtr() const
	{
		return buffer.Ptr();
	}
	BufferStreamBase& BufferStreamBase::operator=(BufferStreamBase&& other) noexcept
	{
		buffer = std::move(other.buffer);
		position = other.position;

		other.position = 0;

		return *this;
	}

	BufferWriteStream::BufferWriteStream()
	{
	}
	BufferWriteStream::~BufferWriteStream()
	{
	}
	uintMem BufferWriteStream::Write(const void* ptr, uintMem byteCount)
	{
		if (buffer.Count() < position + byteCount)
			buffer.Resize(position + byteCount);

		memcpy((char*)buffer.Ptr() + position, ptr, byteCount);

		position += byteCount;

		return byteCount;
	}

	BufferReadStream::BufferReadStream()
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

		memcpy(ptr, (char*)buffer.Ptr() + position, byteCount);
		MovePosition(byteCount);

		return byteCount;
	}
	BufferViewStream::BufferViewStream()
		: ptr(nullptr), size(0), position(0)
	{
	}
	BufferViewStream::BufferViewStream(BufferViewStream&& other) noexcept
		: ptr(other.ptr), size(other.size), position(other.position)
	{
		other.ptr = nullptr;
		other.size = 0;
		other.position = 0;
	}
	BufferViewStream::BufferViewStream(const void* ptr, uintMem size)
		: ptr(nullptr), size(0), position(0)
	{
		SetBuffer(ptr, size);
	}
	BufferViewStream::~BufferViewStream()
	{
	}

	void BufferViewStream::Clear()
	{
		ptr = nullptr;
		size = 0;
		position = 0;
	}
	bool BufferViewStream::MovePosition(intMem offset)
	{
		uintMem newPos = position + offset;

		if (offset >= size || newPos >= size)
			return false;

		position = newPos;

		return true;
	}
	bool BufferViewStream::SetPosition(uintMem offset)
	{
		if (offset >= size)
			return false;

		position = offset;

		return true;
	}
	bool BufferViewStream::SetPositionFromEnd(intMem offset)
	{
		if (offset < 0 || offset >= (intMem)size)
			return false;

		position = size - offset;

		return true;
	}
	uintMem BufferViewStream::GetPosition() const
	{
		return position;
	}
	uintMem BufferViewStream::GetSize() const
	{
		return size;
	}
	const void* BufferViewStream::GetPtr() const
	{
		return ptr;
	}
	uintMem BufferViewStream::Read(void* ptr, uintMem byteCount)
	{
		uintMem position = GetPosition();
		uintMem leftBytes = GetSize() - position;

		if (leftBytes < byteCount)
			byteCount = leftBytes;

		memcpy(ptr, (char*)this->ptr + position, byteCount);
		MovePosition(byteCount);

		return byteCount;
	}
	void BufferViewStream::SetBuffer(const void* ptr, uintMem size)
	{
		if (ptr == nullptr || size == 0)
		{
			this->ptr = nullptr;
			this->size = 0;
			position = 0;
		}
		else
		{
			this->ptr = ptr;
			this->size = size;
			position = 0;
		}
	}
	BufferViewStream& BufferViewStream::operator=(BufferViewStream&& other) noexcept
	{
		ptr = other.ptr;
		size = other.size;
		position = other.position;

		other.ptr = nullptr;
		other.size = 0;
		other.position = 0;
		return *this;
	}
}