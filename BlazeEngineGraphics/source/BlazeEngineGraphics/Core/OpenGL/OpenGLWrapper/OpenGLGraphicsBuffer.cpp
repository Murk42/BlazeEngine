#include "pch.h"
#include "BlazeEngineGraphics/Core/OpenGL/OpenGLWrapper/OpenGLGraphicsBuffer.h"
#include "BlazeEngineGraphics/Core/OpenGL/OpenGLWrapper/OpenGLConversions.h"

namespace Blaze::Graphics::OpenGLWrapper
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
		: mapAccess((ImmutableGraphicsBufferMapAccess)0), mappingType((ImmutableGraphicsBufferMapType::None)), GraphicsBuffer()
	{
	}
	ImmutableMappedGraphicsBuffer::ImmutableMappedGraphicsBuffer(ImmutableMappedGraphicsBuffer&& other) noexcept
		: GraphicsBuffer(std::move(other)), mapAccess(other.mapAccess), mappingType(other.mappingType)
	{
		other.mapAccess = (ImmutableGraphicsBufferMapAccess)0;
		other.mappingType = ImmutableGraphicsBufferMapType::None;
	}
	void ImmutableMappedGraphicsBuffer::Allocate(const void* ptr, size_t size, ImmutableGraphicsBufferMapAccess access, ImmutableGraphicsBufferMapType mappingType, bool clientStorage)
	{		
		this->mapAccess = access;
		this->mappingType = mappingType;

		Result result;
		GLenum _access = OpenGLBufferMappingAccess(access, result);
		if (result) return;
		GLenum _mappingType = OpenGLBufferMappingType(mappingType, result);
		if (result) return;

		glNamedBufferStorage(id, size, ptr, GL_DYNAMIC_STORAGE_BIT | (clientStorage ? GL_CLIENT_STORAGE_BIT : 0) | _access | _mappingType);		
	}
	void* ImmutableMappedGraphicsBuffer::MapBufferRange(size_t offset, size_t size, ImmutableGraphicsBufferMapOptions options)
	{		
		Result result;
		GLenum _access = OpenGLBufferMappingAccess(mapAccess, result);
		if (result) return nullptr;
		GLenum _type = OpenGLBufferMappingType(mappingType, result);
		if (result) return nullptr;
		GLenum _options = OpenGLBufferMappingOptions(options, result);
		if (result) return nullptr;

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
		other.mapAccess = (ImmutableGraphicsBufferMapAccess)0;
		other.mappingType = ImmutableGraphicsBufferMapType::None;

		return *this;
	}
	void MutableDrawGraphicsBuffer::Allocate(const void* ptr, size_t size, MutableGraphicsBufferUseFrequency frequency)
	{
		Result result;
		GLenum usage = OpenGLMutableBufferUsage(frequency, MutableGraphicsBufferUseType::Draw, result);
		if (result) return;

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
	void MutableReadGraphicsBuffer::Allocate(size_t size, MutableGraphicsBufferUseFrequency frequency)
	{
		Result result;
		GLenum usage = OpenGLMutableBufferUsage(frequency, MutableGraphicsBufferUseType::Draw, result);
		if (result) return;

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
	void MutableCopyGraphicsBuffer::Allocate(size_t size, MutableGraphicsBufferUseFrequency frequency)
	{
		Result result;
		GLenum usage = OpenGLMutableBufferUsage(frequency, MutableGraphicsBufferUseType::Draw, result);
		
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