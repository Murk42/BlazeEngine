#include "BlazeEngine/Graphics/OpenGL/OpenGLGraphicsBuffer.h"
#include "source/BlazeEngine/Internal/Conversions.h"

#include "GL/glew.h"

namespace Blaze::OpenGL
{
	GraphicsBuffer::GraphicsBuffer()
		: id(-1)
	{
		glCreateBuffers(1, &id);
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
			glDeleteBuffers(1, &id);
	}

	void GraphicsBuffer::AllocateDynamicStorage(BufferView buffer, GraphicsBufferDynamicStorageHint hint)
	{
		GLenum _hint = OpenGLBufferDynamicStorageHint(hint);
		glNamedBufferData(id, buffer.Size(), buffer.Ptr(), _hint);
	}
	void GraphicsBuffer::AllocateStaticStorage(BufferView buffer, GraphicsBufferStaticStorageHint hint)
	{
		GLenum _hint = OpenGLBufferStaticStorageHint(hint);
		glNamedBufferStorage(id, buffer.Size(), buffer.Ptr(), _hint);
	}
	void GraphicsBuffer::ChangeData(BufferView buffer, size_t offset)
	{
		glNamedBufferSubData(id, offset, buffer.Size(), buffer.Ptr());
	}

	GraphicsBuffer& GraphicsBuffer::operator=(GraphicsBuffer&& buffer) noexcept
	{
		if (id != -1)
			glDeleteBuffers(1, &id);
		id = buffer.id;
		buffer.id = -1;
		return *this;
	}
}