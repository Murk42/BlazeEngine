#include "pch.h"
#include "BlazeEngineGraphics/Core/OpenGL/OpenGLWrapper/OpenGLContext.h"
#include "BlazeEngineGraphics/Core/OpenGL/OpenGLWrapper/OpenGLConversions.h"

namespace Blaze::Graphics::OpenGLWrapper
{
	TextureBuffer::TextureBuffer()
		: id(-1)
	{
		glGenTextures(1, &id);		
		SelectTexture(this);
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

	Result TextureBuffer::Associate(const GraphicsBuffer& buffer, TextureBufferInternalPixelFormat format)
	{
		Result result;

		auto internalPixelFormat = OpenGLBufferInternalPixelFormat(format, result);
		if (result) return;

		glTextureBuffer(id, internalPixelFormat, buffer.GetHandle());

		return result;
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