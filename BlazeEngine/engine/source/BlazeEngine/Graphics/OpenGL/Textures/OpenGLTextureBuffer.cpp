#include "BlazeEngine/Graphics/OpenGL/Textures/OpenGLTextureBuffer.h"

#include "GL/glew.h"
#include "source/BlazeEngine/Internal/Conversions.h"
#include "BlazeEngine/Graphics/Renderer.h"

namespace Blaze::OpenGL
{
	TextureBuffer::TextureBuffer()
		: id(-1)
	{
		glGenTextures(1, &id);		
		Renderer::SelectTexture(this);
	}

	TextureBuffer::TextureBuffer(TextureBuffer&& tex) noexcept
		: id(tex.id)
	{
		tex.id = -1;
	}
	TextureBuffer::~TextureBuffer()
	{
		if (id != -1)
			glDeleteTextures(1, &id);
	}

	void TextureBuffer::Associate(const GraphicsBuffer& buffer, TextureBufferInternalPixelFormat format)
	{
		glTextureBuffer(id, OpenGLBufferInternalPixelFormat(format), buffer.GetHandle());
	}	
	
	TextureBuffer& TextureBuffer::operator=(TextureBuffer&& tex) noexcept
	{
		if (id != -1)
			glDeleteTextures(1, &id);
		id = tex.id;
		tex.id = -1;		
		return *this;
	}
}