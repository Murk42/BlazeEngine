#include "pch.h"
#include "BlazeEngine/Graphics/Core/OpenGL/OpenGLWrapper/OpenGLGraphicsBuffer.h"
#include "BlazeEngine/Graphics/Core/OpenGL/OpenGLWrapper/OpenGLConversions.h"

namespace Blaze::Graphics::OpenGL
{
	GraphicsBuffer::GraphicsBuffer()
		: id(0)
	{
		glCreateBuffers(1, &id);
	}
	GraphicsBuffer::GraphicsBuffer(uint id)
		: id(id)
	{
	}
	GraphicsBuffer::GraphicsBuffer(GraphicsBuffer&& buffer) noexcept
		: id(0)
	{
		id = buffer.id;
		buffer.id = 0;
	}
	GraphicsBuffer::~GraphicsBuffer()
	{
		if (id != 0)
			glDeleteBuffers(1, &id);
	}

	void GraphicsBuffer::Invalidate() const
	{
		glInvalidateBufferData(id);
	}
	uint GraphicsBuffer::ReleaseHandleOwnership()
	{
		uint id = this->id;
		this->id = 0;
		return id;
	}
	void GraphicsBuffer::Release()
	{
		if (id != 0)
		{
			glDeleteBuffers(1, &id);
			id = 0;
		}
	}
	GraphicsBuffer& GraphicsBuffer::operator=(GraphicsBuffer&& buffer) noexcept
	{
		if (id != 0)
			glDeleteBuffers(1, &id);

		id = buffer.id;
		buffer.id = 0;

		return *this;
	}
	void ImmutableStaticGraphicsBuffer::Allocate(const void* ptr, size_t size, bool clientStorage)
	{
		glNamedBufferStorage(id, size, ptr, (clientStorage ? GL_CLIENT_STORAGE_BIT : 0));
	}
	void ImmutableStaticGraphicsBuffer::Swap(ImmutableStaticGraphicsBuffer& other)
	{
		uint temp = id;
		id = other.id;
		other.id = temp;
	}
	void ImmutableDynamicGraphicsBuffer::Allocate(const void* ptr, size_t size, bool clientStorage)
	{
		glNamedBufferStorage(id, size, ptr, GL_DYNAMIC_STORAGE_BIT | (clientStorage ? GL_CLIENT_STORAGE_BIT : 0));
	}
	void ImmutableDynamicGraphicsBuffer::WriteData(const void* ptr, size_t size, size_t offset)
	{
		glNamedBufferSubData(id, offset, size, ptr);
	}
	void ImmutableDynamicGraphicsBuffer::Swap(ImmutableDynamicGraphicsBuffer& other)
	{
		uint temp = id;
		id = other.id;
		other.id = temp;
	}
	ImmutableMappedGraphicsBuffer::ImmutableMappedGraphicsBuffer()
		: mapAccess((GraphicsBufferMapAccessFlags)0), mappingType((GraphicsBufferMapType::None)), GraphicsBuffer()
	{
	}
	ImmutableMappedGraphicsBuffer::ImmutableMappedGraphicsBuffer(ImmutableMappedGraphicsBuffer&& other) noexcept
		: GraphicsBuffer(std::move(other)), mapAccess(other.mapAccess), mappingType(other.mappingType)
	{
		other.mapAccess = (GraphicsBufferMapAccessFlags)0;
		other.mappingType = GraphicsBufferMapType::None;
	}
	void ImmutableMappedGraphicsBuffer::Allocate(const void* ptr, size_t size, GraphicsBufferMapAccessFlags access, GraphicsBufferMapType mappingType, bool clientStorage)
	{
		this->mapAccess = access;
		this->mappingType = mappingType;
		
		GLenum _access, _mappingType;
		if (!OpenGLBufferMappingAccess(access, _access))
		{
			BLAZE_LOG_ERROR("Invalid GraphicsBufferMapAccessFlags enum value");
			return;
		}
		if (!OpenGLBufferMappingType(mappingType, _mappingType))
		{
			BLAZE_LOG_ERROR("Invalid GraphicsBufferMapType enum value");
			return;
		}

		glNamedBufferStorage(id, size, ptr, GL_DYNAMIC_STORAGE_BIT | (clientStorage ? GL_CLIENT_STORAGE_BIT : 0) | _access | _mappingType);
	}
	void* ImmutableMappedGraphicsBuffer::MapBufferRange(size_t offset, size_t size, GraphicsBufferMapOptions options)
	{
		GLenum _access, _type, _options;
		if (!OpenGLBufferMappingAccess(mapAccess, _access))
		{
			BLAZE_LOG_ERROR("Invalid GraphicsBufferMapAccessFlags enum value");
			return nullptr;
		}
		if (!OpenGLBufferMappingType(mappingType, _type))
		{
			BLAZE_LOG_ERROR("Invalid GraphicsBufferMapType enum value");
			return nullptr;
		}
		if (!OpenGLBufferMappingOptions(options, _options))
		{
			BLAZE_LOG_ERROR("Invalid GraphicsBufferMapOptions enum value");
			return nullptr;
		}

		void* map = glMapNamedBufferRange(id, offset, size, _access | _type | _options);

		return map;
	}
	void ImmutableMappedGraphicsBuffer::UnmapBuffer()
	{
		glUnmapNamedBuffer(id);
	}
	void ImmutableMappedGraphicsBuffer::FlushBufferRange(size_t offset, size_t size)
	{
		glFlushMappedNamedBufferRange(id, offset, size);
	}
	void ImmutableMappedGraphicsBuffer::Swap(ImmutableMappedGraphicsBuffer& other)
	{
		uint temp = id;
		id = other.id;
		other.id = temp;
	}
	ImmutableMappedGraphicsBuffer& ImmutableMappedGraphicsBuffer::operator=(ImmutableMappedGraphicsBuffer&& other) noexcept
	{
		GraphicsBuffer::operator=(std::move(other));

		mapAccess = other.mapAccess;
		mappingType = other.mappingType;
		other.mapAccess = (GraphicsBufferMapAccessFlags)0;
		other.mappingType = GraphicsBufferMapType::None;

		return *this;
	}
	void MutableDrawGraphicsBuffer::Allocate(const void* ptr, size_t size, GraphicsBufferUseFrequency frequency)
	{		
		GLenum usage;
		if (!OpenGLMutableBufferUsage(frequency, GraphicsBufferUseType::Draw, usage))
		{
			BLAZE_LOG_ERROR("Invalid GraphicsBufferUseType enum value");
			return;
		}

		glNamedBufferData(id, size, ptr, usage);
	}
	void MutableDrawGraphicsBuffer::WriteData(const void* ptr, size_t size, size_t offset)
	{
		glNamedBufferSubData(id, offset, size, ptr);
	}
	void MutableDrawGraphicsBuffer::Swap(MutableDrawGraphicsBuffer& other)
	{
		uint temp = id;
		id = other.id;
		other.id = temp;
	}
	void MutableReadGraphicsBuffer::Allocate(size_t size, GraphicsBufferUseFrequency frequency)
	{
		GLenum usage;
		if (!OpenGLMutableBufferUsage(frequency, GraphicsBufferUseType::Read, usage))
		{
			BLAZE_LOG_ERROR("Invalid GraphicsBufferUseFrequency enum value");
			return;
		}

		glNamedBufferData(id, size, nullptr, usage);
	}
	void MutableReadGraphicsBuffer::ReadData(void* ptr, size_t size, size_t offset) const
	{
		glGetNamedBufferSubData(id, offset, size, ptr);
	}
	void MutableReadGraphicsBuffer::Swap(MutableReadGraphicsBuffer& other)
	{
		uint temp = id;
		id = other.id;
		other.id = temp;
	}
	void MutableCopyGraphicsBuffer::Allocate(size_t size, GraphicsBufferUseFrequency frequency)
	{
		GLenum usage;
		if (!OpenGLMutableBufferUsage(frequency, GraphicsBufferUseType::Copy, usage))
		{
			BLAZE_LOG_ERROR("Invalid GraphicsBufferUseType enum value");
			return;
		}

		glNamedBufferData(id, size, nullptr, usage);
	}
	void MutableCopyGraphicsBuffer::CopyData(GraphicsBuffer& destination, size_t readOffset, size_t writeOffset, size_t size)
	{
		glCopyNamedBufferSubData(id, destination.GetHandle(), readOffset, writeOffset, size);
	}
	void MutableCopyGraphicsBuffer::Swap(MutableCopyGraphicsBuffer& other)
	{
		uint temp = id;
		id = other.id;
		other.id = temp;
	}
}