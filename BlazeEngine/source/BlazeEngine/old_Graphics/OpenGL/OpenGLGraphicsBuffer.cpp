#include "BlazeEngine/Graphics/OpenGL/OpenGLGraphicsBuffer.h"
#include "source/BlazeEngine/Internal/Conversions.h"

#include "GL/glew.h"

namespace Blaze::OpenGL
{
	GraphicsBuffer::GraphicsBuffer()
		: id(-1)
	{		
		glCreateBuffers(1, &id);			

		FlushOpenGLResult();
	}
	GraphicsBuffer::GraphicsBuffer(GraphicsBuffer&& buffer) noexcept
		: id(-1)
	{
		id = buffer.id;
		buffer.id = -1;
	}
	GraphicsBuffer::~GraphicsBuffer()
	{
		if (id != -1)
		{
			glDeleteBuffers(1, &id);
			FlushOpenGLResult();
		}
	}

	Result GraphicsBuffer::Invalidate() const
	{
		glInvalidateBufferData(id);
		CHECK_OPENGL_RESULT();

		return Result();		
	}
	GraphicsBuffer& GraphicsBuffer::operator=(GraphicsBuffer&& buffer) noexcept
	{
		if (id != -1)
		{
			glDeleteBuffers(1, &id);
			if (FlushOpenGLResult())
				return *this;
		}
		id = buffer.id;
		buffer.id = -1;

		return *this;
	}
	Result ImmutableStaticGraphicsBuffer::Allocate(void* ptr, size_t size, bool clientStorage)
	{		
		glNamedBufferStorage(GetHandle(), size, ptr, (clientStorage ? GL_CLIENT_STORAGE_BIT : 0));
		CHECK_OPENGL_RESULT();

		return Result();
	}
	Result ImmutableDynamicGraphicsBuffer::Allocate(void* ptr, size_t size, bool clientStorage)
	{		
		glNamedBufferStorage(GetHandle(), size, ptr, GL_DYNAMIC_STORAGE_BIT | (clientStorage ? GL_CLIENT_STORAGE_BIT : 0));
		CHECK_OPENGL_RESULT();

		return Result();
	}
	Result ImmutableDynamicGraphicsBuffer::WriteData(const void* ptr, size_t size, size_t offset)
	{		
		glNamedBufferSubData(GetHandle(), offset, size, ptr);
		CHECK_OPENGL_RESULT();

		return Result();
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
	Result ImmutableMappedGraphicsBuffer::Allocate(void* ptr, size_t size, ImmutableGraphicsBufferMapAccess access, ImmutableGraphicsBufferMapType type, bool clientStorage)
	{		
		this->mapAccess = access;
		this->mappingType = mappingType;

		Result result;
		GLenum _access = OpenGLBufferMappingAccess(access, result);
		CHECK_RESULT(result);		
		GLenum _type = OpenGLBufferMappingType(type, result);
		CHECK_RESULT(result);		

		glNamedBufferStorage(GetHandle(), size, ptr, GL_DYNAMIC_STORAGE_BIT | (clientStorage ? GL_CLIENT_STORAGE_BIT : 0) | _access | _type);
		CHECK_OPENGL_RESULT();

		return Result();
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
		FlushOpenGLResult();				

		return map;
	}
	Result ImmutableMappedGraphicsBuffer::UnmapBuffer()
	{		
		glUnmapNamedBuffer(GetHandle());			
		CHECK_OPENGL_RESULT();

		return Result();
	}
	Result ImmutableMappedGraphicsBuffer::FlushBufferRange(size_t offset, size_t size)
	{		
		glFlushMappedNamedBufferRange(GetHandle(), offset, size);
		CHECK_OPENGL_RESULT();

		return Result();
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
	Result MutableDrawGraphicsBuffer::Allocate(void* ptr, size_t size, MutableGraphicsBufferUseFrequency frequency)
	{
		Result result;
		GLenum usage = OpenGLMutableBufferUsage(frequency, MutableGraphicsBufferUseType::Draw, result);
		CHECK_RESULT(result);

		glNamedBufferData(GetHandle(), size, ptr, usage);
		CHECK_OPENGL_RESULT();

		return Result();
	}
	Result MutableDrawGraphicsBuffer::WriteData(const void* ptr, size_t size, size_t offset)
	{
		glNamedBufferSubData(GetHandle(), offset, size, ptr);
		CHECK_OPENGL_RESULT();

		return Result();
	}
	Result MutableReadGraphicsBuffer::Allocate(size_t size, MutableGraphicsBufferUseFrequency frequency)
	{
		Result result;
		GLenum usage = OpenGLMutableBufferUsage(frequency, MutableGraphicsBufferUseType::Draw, result);
		CHECK_RESULT(result);		

		glNamedBufferData(GetHandle(), size, nullptr, usage);
		CHECK_OPENGL_RESULT();

		return Result();
	}
	Result MutableReadGraphicsBuffer::ReadData(void* ptr, size_t size, size_t offset) const
	{		
		glGetNamedBufferSubData(GetHandle(), offset, size, ptr);
		CHECK_OPENGL_RESULT();

		return Result();
	}
	Result MutableCopyGraphicsBuffer::Allocate(size_t size, MutableGraphicsBufferUseFrequency frequency)
	{
		Result result;
		GLenum usage = OpenGLMutableBufferUsage(frequency, MutableGraphicsBufferUseType::Draw, result);
		CHECK_RESULT(result);

		glNamedBufferData(GetHandle(), size, nullptr, usage);
		CHECK_OPENGL_RESULT();

		return Result();
	}
	Result MutableCopyGraphicsBuffer::CopyData(GraphicsBuffer& destination, size_t readOffset, size_t writeOffset, size_t size)
	{
		glCopyNamedBufferSubData(GetHandle(), destination.GetHandle(), readOffset, writeOffset, size);
		CHECK_OPENGL_RESULT();

		return Result();
	}
}