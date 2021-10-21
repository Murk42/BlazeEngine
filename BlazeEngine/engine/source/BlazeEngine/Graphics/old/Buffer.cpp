#include "BlazeEngine/Graphics/Buffer.h"
#include "BlazeEngine/Graphics/VertexLayout.h"
#include "BlazeEngine/Core/Logger.h"
#include "GL/glew.h"

#include "source/BlazeEngine/Internal/Engine.h"

namespace Blaze
{
	BufferUsage operator| (const BufferUsage& a, const BufferUsage& b) { return (BufferUsage)((int)a | (int)b); }		

	Buffer::Buffer(GraphicsBufferType type)	
		: id(0), type(type), size(0), usage((BufferUsage)0)
	{
		glGenBuffers(1, &id);
	}
	Buffer::Buffer(const Buffer& b)
		: id(0), type(b.type), size(b.size), usage(b.usage)
	{
		glGenVertexArrays(1, &id);

		Bind();
		glBufferData((int)type, b.size, nullptr, (uint)b.usage);

		glBindBuffer(GL_COPY_WRITE_BUFFER, id);
		glBindBuffer(GL_COPY_READ_BUFFER, b.id);
		glCopyBufferSubData(GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER, 0, 0, b.size);
	}
	Buffer::Buffer(Buffer&& b) noexcept
		: id(std::exchange(b.id, 0)), type(b.type), size(b.size), usage(b.usage)
	{

	}
	Buffer::Buffer(GraphicsBufferType type, const void* ptr, unsigned size, BufferUsage usage)		
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

	void Buffer::AllocateData(const void* ptr, unsigned size, BufferUsage usage)
	{
		this->size = size;		
		this->usage = usage;
		Bind();
		glBufferData((int)type, size, ptr, (uint)usage);
	}

	void Buffer::ChangeData(const void* ptr, unsigned size, size_t offset)
	{		
		Bind();
		glBufferSubData((int)type, offset, size, ptr);
	}

	void Buffer::Bind() const
	{
		glBindBuffer((unsigned)type, id);

		switch (type)
		{
		case GraphicsBufferType::ArrayBuffer: engine->Renderer.boundVertexLayout->vertexBuffer = (VertexBuffer*)this; break;
		case GraphicsBufferType::IndexBuffer: engine->Renderer.boundVertexLayout->indexBuffer = (IndexBuffer*)this; break;
		case GraphicsBufferType::UniformBuffer: engine->Renderer.boundUniformBuffer = (Buffer*)this; break;
		}
	}
	void Buffer::Unbind(GraphicsBufferType type)
	{
		glBindBuffer((unsigned)type, 0);

		switch (type)
		{
		case GraphicsBufferType::ArrayBuffer: engine->Renderer.boundVertexLayout->vertexBuffer = nullptr; break;
		case GraphicsBufferType::IndexBuffer: engine->Renderer.boundVertexLayout->indexBuffer = nullptr; break;
		case GraphicsBufferType::UniformBuffer: engine->Renderer.boundUniformBuffer = nullptr; break;
		}
	}
	Buffer* Buffer::GetBound(GraphicsBufferType type)
	{
		switch (type)
		{
		case GraphicsBufferType::ArrayBuffer: return engine->Renderer.boundVertexLayout->vertexBuffer;
		case GraphicsBufferType::IndexBuffer: return engine->Renderer.boundVertexLayout->indexBuffer;
		case GraphicsBufferType::UniformBuffer: return engine->Renderer.boundUniformBuffer;
		}
		return nullptr;
	}

	Buffer& Buffer::operator=(const Buffer& b)
	{
		size = b.size;
		type = b.type;
		usage = b.usage;

		Bind();
		glBufferData((int)type, b.size, nullptr, (uint)b.usage);

		glBindBuffer(GL_COPY_WRITE_BUFFER, id);
		glBindBuffer(GL_COPY_READ_BUFFER, b.id);
		glCopyBufferSubData(GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER, 0, 0, b.size);

		return *this;
	}

	Buffer& Buffer::operator=(Buffer&& b) noexcept
	{
		id = std::exchange(b.id, 0);
		size = b.size;
		type = b.type;
		usage = b.usage;

		return *this;
	}

	VertexBuffer::VertexBuffer()
		: Buffer(GraphicsBufferType::ArrayBuffer)
	{
	}
	VertexBuffer::VertexBuffer(const VertexBuffer& vb)
		: Buffer(vb)
	{
	}
	VertexBuffer::VertexBuffer(VertexBuffer&& vb) noexcept
		: Buffer(std::move(vb))
	{
	}
	VertexBuffer& VertexBuffer::operator=(const VertexBuffer& vb)
	{
		Buffer::operator=(vb);
		return *this;
	}
	VertexBuffer& VertexBuffer::operator=(VertexBuffer&& vb) noexcept
	{
		Buffer::operator=(std::move(vb));
		return *this;
	}

	IndexBuffer::IndexBuffer()
		: Buffer(GraphicsBufferType::IndexBuffer), indexType((Type)0)
	{

	}
	IndexBuffer::IndexBuffer(const IndexBuffer& ib)
		: Buffer(ib), indexType(ib.indexType)
	{
	}
	IndexBuffer::IndexBuffer(IndexBuffer&& ib) noexcept
		: Buffer(std::move(ib)), indexType(ib.indexType)
	{
	}
	IndexBuffer::IndexBuffer(const void* ptr, unsigned count, BufferUsage usage, Type indexType)
		: Buffer(GraphicsBufferType::IndexBuffer, ptr, count* SizeOf(indexType), usage), indexType(indexType)
	{
	}

	void IndexBuffer::AllocateData(const void* ptr, unsigned size, BufferUsage usage, Type indexType)
	{		
		this->indexType = indexType;
		Buffer::AllocateData(ptr, size, usage);
	}	

	IndexBuffer& IndexBuffer::operator=(const IndexBuffer& ib)
	{
		indexType = ib.indexType;
		Buffer::operator=(ib);
		return *this;
	}

	IndexBuffer& IndexBuffer::operator=(IndexBuffer&& ib) noexcept
	{
		indexType = ib.indexType;
		Buffer::operator=(std::move(ib));
		return *this;
	}	
}