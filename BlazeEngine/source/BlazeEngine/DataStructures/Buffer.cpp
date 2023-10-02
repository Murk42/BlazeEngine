#include "pch.h"
#include "BlazeEngine/DataStructures/Buffer.h"

namespace Blaze
{
	Buffer::Buffer()
		: ptr(nullptr), size(0)
	{
	}
	Buffer::Buffer(const Buffer& buffer)
		: size(buffer.size)
	{
		ptr = malloc(size);
		memcpy(ptr, buffer.ptr, size);
	}
	Buffer::Buffer(Buffer&& buffer) noexcept
		: ptr(buffer.ptr), size(buffer.size)
	{
		buffer.ptr = nullptr;
		buffer.size = 0;
	}
	Buffer::Buffer(size_t size)
		: size(size)
	{
		ptr = malloc(size);
	}
	Buffer::Buffer(const BufferView& bv)
		: size(bv.Size())
	{
		ptr = malloc(size);
		memcpy(ptr, bv.Ptr(), size);
	}
	Buffer::~Buffer()
	{
		free(ptr);		
	}
	void Buffer::Allocate(size_t size)
	{		
		void* newPtr = realloc(ptr, size);

		if (newPtr != nullptr)
			ptr = newPtr;
		else
			free(ptr);

		this->size = size;
	}
	void Buffer::Free()
	{
		free(ptr);
		ptr = nullptr;
		size = 0;
	}
	Buffer& Buffer::operator=(const Buffer& buffer)
	{
		free(ptr);
		ptr = malloc(buffer.size);
		memcpy(ptr, buffer.ptr, buffer.size);
		size = buffer.size;
		return *this;
	}
	Buffer& Buffer::operator=(Buffer&& buffer) noexcept
	{		
		free(ptr);		
		ptr = buffer.ptr;
		size = buffer.size;
		buffer.ptr = nullptr;
		buffer.size = 0;
		return *this;
	}

	Buffer& Buffer::operator=(const BufferView& bv)
	{
		free(ptr);
		size = bv.Size();
		ptr = malloc(size);
		memcpy(ptr, bv.Ptr(), size);
		return *this;
	}

	BufferView::BufferView()
		: ptr(nullptr), size(0)
	{
	}
	BufferView::BufferView(const BufferView& bv)
		: ptr(bv.ptr), size(bv.size)
	{
	}
	BufferView::BufferView(const void* ptr, size_t size)
		: ptr(ptr), size(size)
	{
	}
	BufferView::BufferView(const Buffer& buffer)
		: ptr(buffer.Ptr()), size(buffer.Size())
	{
	}
	BufferView& BufferView::operator=(const BufferView& bv)
	{
		ptr = bv.ptr;
		size = bv.size;
		return *this;		
	}
	BufferView& BufferView::operator=(const Buffer& buffer)
	{
		ptr = buffer.Ptr();
		size = buffer.Size();
		return *this;
	}
}