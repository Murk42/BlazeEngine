#include "BlazeEngine/Graphics/OpenGL/Textures/OpenGLTextureBuffer.h"

#include "GL/glew.h"
#include "source/BlazeEngine/Internal/Conversions.h"
#include "BlazeEngine/Graphics/GraphicsCore.h"

namespace Blaze::OpenGL
{
	TextureBuffer::TextureBuffer()
		: id(-1)
	{
		glGenTextures(1, &id);		
		Graphics::Core::SelectTexture(this);
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
		CHECK_RESULT(result);

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