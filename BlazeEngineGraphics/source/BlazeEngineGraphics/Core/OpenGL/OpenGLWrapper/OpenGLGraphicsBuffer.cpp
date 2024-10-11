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
		glNamedBufferStorage(GetHandle(), size, ptr, (clientStorage ? GL_CLIENT_STORAGE_BIT : 0));		
	}
	void ImmutableDynamicGraphicsBuffer::Allocate(const void* ptr, size_t size, bool clientStorage)
	{		
		glNamedBufferStorage(GetHandle(), size, ptr, GL_DYNAMIC_STORAGE_BIT | (clientStorage ? GL_CLIENT_STORAGE_BIT : 0));		
	}
	void ImmutableDynamicGraphicsBuffer::WriteData(const void* ptr, size_t size, size_t offset)
	{		
		glNamedBufferSubData(GetHandle(), offset, size, ptr);		
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

		glNamedBufferStorage(GetHandle(), size, ptr, GL_DYNAMIC_STORAGE_BIT | (clientStorage ? GL_CLIENT_STORAGE_BIT : 0) | _access | _mappingType);		
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

		void* map = glMapNamedBufferRange(GetHandle(), offset, size, _access | _type | _options);								

		return map;
	}
	void ImmutableMappedGraphicsBuffer::UnmapBuffer()
	{		
		glUnmapNamedBuffer(GetHandle());					
	}
	void ImmutableMappedGraphicsBuffer::FlushBufferRange(size_t offset, size_t size)
	{		
		glFlushMappedNamedBufferRange(GetHandle(), offset, size);		
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

		glNamedBufferData(GetHandle(), size, ptr, usage);		
	}
	void MutableDrawGraphicsBuffer::WriteData(const void* ptr, size_t size, size_t offset)
	{
		glNamedBufferSubData(GetHandle(), offset, size, ptr);		
	}
	void MutableReadGraphicsBuffer::Allocate(size_t size, MutableGraphicsBufferUseFrequency frequency)
	{
		Result result;
		GLenum usage = OpenGLMutableBufferUsage(frequency, MutableGraphicsBufferUseType::Draw, result);
		if (result) return;

		glNamedBufferData(GetHandle(), size, nullptr, usage);		
	}
	void MutableReadGraphicsBuffer::ReadData(void* ptr, size_t size, size_t offset) const
	{		
		glGetNamedBufferSubData(GetHandle(), offset, size, ptr);		
	}
	void MutableCopyGraphicsBuffer::Allocate(size_t size, MutableGraphicsBufferUseFrequency frequency)
	{
		Result result;
		GLenum usage = OpenGLMutableBufferUsage(frequency, MutableGraphicsBufferUseType::Draw, result);
		
		glNamedBufferData(GetHandle(), size, nullptr, usage);		
	}
	void MutableCopyGraphicsBuffer::CopyData(GraphicsBuffer& destination, size_t readOffset, size_t writeOffset, size_t size)
	{
		glCopyNamedBufferSubData(GetHandle(), destination.GetHandle(), readOffset, writeOffset, size);		
	}
}