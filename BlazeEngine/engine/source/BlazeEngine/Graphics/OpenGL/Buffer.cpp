#include "BlazeEngine/Graphics/OpenGL/Buffer.h"
#include "GL/glew.h"
#include "source/BlazeEngine/Internal/Engine.h"

namespace Blaze
{
	namespace OpenGL
	{
		BaseBuffer::BaseBuffer()
			: id(0), size(0)
		{
			glCreateBuffers(1, &id);
		}		
		BaseBuffer::BaseBuffer(BaseBuffer&& buffer) noexcept
			: id(0), size(0)
		{
			id = buffer.id; 
			buffer.id = 0;
		}
		BaseBuffer::~BaseBuffer()
		{
			if (id != 0)
				glDeleteBuffers(1, &id);
		}				

		inline size_t BaseBuffer::GetSize() const
		{
			return size;
		}

		inline void BaseBuffer::Bind(BufferType type, BaseBuffer& buffer)
		{
			glBindBuffer((GLenum)type, buffer.id);
			switch (type)
			{
			case Blaze::OpenGL::BufferType::ArrayBuffer: engine->Renderer.boundVertexArray->arrayBuffer = &buffer; break;
			case Blaze::OpenGL::BufferType::IndexBuffer: engine->Renderer.boundVertexArray->indexBuffer = &buffer; break;				
			case Blaze::OpenGL::BufferType::UniformBuffer: engine->Renderer.boundUniformBuffer = &buffer; break;			
			}
		}
		inline void BaseBuffer::Unbind(BufferType type)
		{
			glBindBuffer((GLenum)type, 0);
			switch (type)
			{
			case Blaze::OpenGL::BufferType::ArrayBuffer: engine->Renderer.boundVertexArray->arrayBuffer = nullptr; break;
			case Blaze::OpenGL::BufferType::IndexBuffer: engine->Renderer.boundVertexArray->indexBuffer = nullptr; break;
			case Blaze::OpenGL::BufferType::UniformBuffer: engine->Renderer.boundUniformBuffer = nullptr; break;
			}
		}
		inline BaseBuffer* BaseBuffer::GetBound(BufferType type)
		{
			switch (type)
			{
			case Blaze::OpenGL::BufferType::ArrayBuffer: return engine->Renderer.boundVertexArray->arrayBuffer;
			case Blaze::OpenGL::BufferType::IndexBuffer: return engine->Renderer.boundVertexArray->indexBuffer;
			case Blaze::OpenGL::BufferType::UniformBuffer: return engine->Renderer.boundUniformBuffer;
			}
		}

		BaseBuffer& BaseBuffer::operator=(BaseBuffer&& buffer) noexcept
		{
			id = buffer.id;
			buffer.id = 0;
			return *this;
		}


		inline void MutableBuffer::AllocateData(size_t size, const void* ptr, BufferUsageHint usage)
		{
			this->size = size;
			glNamedBufferData(id, size, ptr, (GLenum)usage);
		}
		inline void MutableBuffer::ChangeData(size_t size, const void* ptr, size_t offset)
		{
			glNamedBufferSubData(id, offset, size, ptr);
		}
		
		inline void ImmutableBuffer<false, false>::AllocateData(size_t size, const void* ptr)
		{
			this->size = size;
			glNamedBufferStorage(id, size, ptr, 0);			
		}
		inline void ImmutableBuffer<false, false>::ChangeData(size_t size, const void* ptr, size_t offset)
		{
			glNamedBufferSubData(id, offset, size, ptr);
		}
		
		inline void ImmutableBuffer<true, false>::AllocateData(size_t size, const void* ptr)
		{
			bool IsBuffer = glIsBuffer(id);
			this->size = size; 
			glNamedBufferStorage(id, size, ptr, 0);
		}		
	}
}