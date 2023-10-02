#pragma once
#include "BlazeEngine/Graphics/OpenGL/Textures/OpenGLTexture1D.h"
#include "BlazeEngine/Internal/Conversions.h"

#include "GL/glew.h"
#include "IL/il.h"

#include "BlazeEngine/Graphics/GraphicsCore.h"

namespace Blaze::OpenGL
{
	void CalculateAlignmentAndStride(uint width, uint byteStride, uint pixelSize, uint& align, uint& pixelStride);

	Texture1D::Texture1D()
		: id(-1), size(0)
	{
		glGenTextures(1, &id);
	}
	Texture1D::Texture1D(Texture1D&& tex) noexcept
		: id(tex.id), size(tex.size)	
	{
		tex.id = -1;
	}
	Texture1D::~Texture1D()
	{
		if (id != -1)
			glDeleteTextures(1, &id);
	}

	Result Texture1D::SetSettings(Texture1DSettings settings)
	{
		Result result;
		GLenum _min = OpenGLTextureMinSampling(settings.min, settings.mip, settings.mipmaps, result);
		CHECK_RESULT(result);
		GLenum _mag = OpenGLTextureMagSampling(settings.mag, result);
		CHECK_RESULT(result);
		GLenum _xWrap = OpenGLTextureWrapping(settings.xWrap, result);
		CHECK_RESULT(result);
		
		glTextureParameteri(id, GL_TEXTURE_WRAP_S, _xWrap);		
		glTextureParameteri(id, GL_TEXTURE_MIN_FILTER, _min);
		glTextureParameteri(id, GL_TEXTURE_MAG_FILTER, _mag);

		Graphics::Renderer::SelectTexture(this);
		if (settings.mipmaps)
			glGenerateMipmap(GL_TEXTURE_2D);

		return result;
	}		
	Result Texture1D::Create(uint size, TextureInternalPixelFormat internalFormat)
	{
		Result result;
		auto internalPixelFormat = OpenGLInternalPixelFormat(internalFormat, result);
		CHECK_RESULT(result);
		auto format = OpenGLFormatByInternalPixelFormat(internalFormat, result);
		CHECK_RESULT(result);

		this->size = size;		
		Graphics::Renderer::SelectTexture(this);				
		glTexImage1D(GL_TEXTURE_1D, 0, internalPixelFormat, static_cast<GLsizei>(size), 0, format, GL_UNSIGNED_BYTE, nullptr);

		return result;
	}	

	Result Texture1D::SetPixels(uint offset, uint size, BitmapColorFormat format, BitmapColorComponentType type, void* pixels)
	{
		Result result;

		GLenum _format = OpenGLPixelFormat(format, result);
		CHECK_RESULT(result);
		GLenum _type = OpenGLPixelType(type, result);
		CHECK_RESULT(result);
						
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);		
		glTextureSubImage1D(id, 0, static_cast<GLint>(offset), static_cast<GLsizei>(size), _format, _type, pixels);

		return result;
	}

	void Texture1D::GenerateMipmaps()
	{
		Graphics::Renderer::SelectTexture(this);
		glGenerateMipmap(GL_TEXTURE_1D);
	}

	Texture1D& Texture1D::operator=(Texture1D&& tex) noexcept
	{
		if (id != -1)
			glDeleteTextures(1, &id);
		id = tex.id;
		tex.id = -1;
		size = tex.size;
		return *this;
	}
}