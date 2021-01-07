#include "BlazeEngine/Graphics/Buffer.h"
#include "BlazeEngine/Graphics/VertexLayout.h"
#include "BlazeEngine/Core/Logger.h"
#include "GL/glew.h"

namespace Blaze
{
	BufferUsageType operator| (const BufferUsageType& a, const BufferUsageType& b) { return (BufferUsageType)((int)a | (int)b); }	
	
	Buffer* Buffer::boundUniformBuffer = nullptr;

	Buffer::Buffer(BufferType type)	
		: id(0), type(type)
	{
		glGenBuffers(1, &id);
	}
	Buffer::Buffer(const Buffer& b)
		: id(0), type(b.type), size(b.size), usage(b.usage)
	{
		glGenVertexArrays(1, &id);

		Bind();
		glBufferData((int)type, b.size, nullptr, b.usage);

		glBindBuffer(GL_COPY_WRITE_BUFFER, id);
		glBindBuffer(GL_COPY_READ_BUFFER, b.id);
		glCopyBufferSubData(GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER, 0, 0, b.size);
	}
	Buffer::Buffer(Buffer&& b)
		: id(std::exchange(b.id, 0)), type(b.type), size(b.size), usage(b.usage)
	{

	}
	Buffer::Buffer(BufferType type, const void* ptr, unsigned size, BufferUsageType usage)		
		: type(type)
	{
		glGenBuffers(1, &id);
		AllocateData(ptr, size, usage);		
	}
	Buffer::~Buffer()
	{
		if (id != 0)
			glDeleteBuffers(1, &id);
	}

	void Buffer::AllocateData(const void* ptr, unsigned size, BufferUsageType usage)
	{
		this->size = size;		
		this->usage = usage;
		Bind();
		glBufferData((int)type, size, ptr, usage);
	}

	void Buffer::ChangeData(const void* ptr, unsigned size, unsigned offset)
	{		
		Bind();
		glBufferSubData((int)type, offset, size, ptr);
	}

	void Buffer::Bind() const
	{
		glBindBuffer((unsigned)type, id);

		switch (type)
		{
		case BufferType::ArrayBuffer: VertexLayout::boundVertexLayout->vertexBuffer = (VertexBuffer*)this; break;
		case BufferType::IndexBuffer: VertexLayout::boundVertexLayout->indexBuffer = (IndexBuffer*)this; break;
		case BufferType::UniformBuffer: boundUniformBuffer = (Buffer*)this; break;
		}
	}
	void Buffer::Unbind(BufferType type)
	{
		glBindBuffer((unsigned)type, 0);

		switch (type)
		{
		case BufferType::ArrayBuffer: VertexLayout::boundVertexLayout->vertexBuffer = nullptr; break;
		case BufferType::IndexBuffer: VertexLayout::boundVertexLayout->indexBuffer = nullptr; break;
		case BufferType::UniformBuffer: boundUniformBuffer = nullptr; break;
		}
	}
	Buffer* Buffer::GetBound(BufferType type)
	{
		switch (type)
		{
		case BufferType::ArrayBuffer: return VertexLayout::boundVertexLayout->vertexBuffer;
		case BufferType::IndexBuffer: return VertexLayout::boundVertexLayout->indexBuffer;
		case BufferType::UniformBuffer: return boundUniformBuffer;		
		}
	}

	void Buffer::operator=(const Buffer& b)
	{
		size = b.size;
		type = b.type;
		usage = b.usage;

		Bind();
		glBufferData((int)type, b.size, nullptr, b.usage);

		glBindBuffer(GL_COPY_WRITE_BUFFER, id);
		glBindBuffer(GL_COPY_READ_BUFFER, b.id);
		glCopyBufferSubData(GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER, 0, 0, b.size);
	}

	void Buffer::operator=(Buffer&& b)		
	{
		id = std::exchange(b.id, 0);
		size = b.size;
		type = b.type;
		usage = b.usage;
	}

	VertexBuffer::VertexBuffer()
		: Buffer(BufferType::ArrayBuffer)
	{
	}
	VertexBuffer::VertexBuffer(const VertexBuffer& vb)
		: Buffer(vb)
	{
	}
	VertexBuffer::VertexBuffer(VertexBuffer&& vb)
		: Buffer(std::move(vb))
	{
	}
	void VertexBuffer::operator=(const VertexBuffer& vb)
	{
		Buffer::operator=(vb);
	}
	void VertexBuffer::operator=(VertexBuffer&& vb)
	{
		Buffer::operator=(std::move(vb));
	}

	IndexBuffer::IndexBuffer()
		: Buffer(BufferType::IndexBuffer)
	{

	}
	IndexBuffer::IndexBuffer(const IndexBuffer& ib)
		: Buffer(ib), indexType(ib.indexType)
	{
	}
	IndexBuffer::IndexBuffer(IndexBuffer&& ib)
		: Buffer(std::move(ib)), indexType(ib.indexType)
	{
	}
	IndexBuffer::IndexBuffer(const void* ptr, unsigned count, BufferUsageType usage, Type indexType)
		: Buffer(BufferType::IndexBuffer, ptr, count* SizeOf(indexType), usage), indexType(indexType)
	{
	}

	void IndexBuffer::AllocateData(const void* ptr, unsigned size, BufferUsageType usage, Type indexType)
	{		
		this->indexType = indexType;
		Buffer::AllocateData(ptr, size, usage);
	}	

	void IndexBuffer::operator=(const IndexBuffer& ib)
	{
		indexType = ib.indexType;
		Buffer::operator=(ib);
	}

	void IndexBuffer::operator=(IndexBuffer&& ib)
	{
		indexType = ib.indexType;
		Buffer::operator=(std::move(ib));
	}	
}