#pragma once
#include "BlazeEngine/Graphics/OpenGL/Textures/OpenGLTexture1D.h"
#include "source/BlazeEngine/Internal/Conversions.h"
#include "BlazeEngine/Logging/Logger.h"

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

	void Texture1D::SetSettings(Texture1DSettings settings)
	{
		GLenum _min = OpenGLTextureMinSampling(settings.min, settings.mip, settings.mipmaps);		
		GLenum _mag = OpenGLTextureMagSampling(settings.mag);		
		GLenum _xWrap = OpenGLTextureWrapping(settings.xWrap);		
		
		glTextureParameteri(id, GL_TEXTURE_WRAP_S, _xWrap);		
		glTextureParameteri(id, GL_TEXTURE_MIN_FILTER, _min);
		glTextureParameteri(id, GL_TEXTURE_MAG_FILTER, _mag);

		Graphics::Core::SelectTexture(this);
		if (settings.mipmaps)
			glGenerateMipmap(GL_TEXTURE_2D);
	}		
	void Texture1D::Create(size_t size, TextureInternalPixelFormat internalFormat)
	{
		this->size = size;		
		Graphics::Core::SelectTexture(this);				
		glTexImage1D(GL_TEXTURE_1D, 0, OpenGLInternalPixelFormat(internalFormat), size, 0, OpenGLFormatByInternalPixelFormat(internalFormat), GL_UNSIGNED_BYTE, nullptr);
	}	

	void Texture1D::SetPixels(size_t offset, size_t size, BitmapPixelFormat format, BitmapPixelType type, void* pixels)
	{
		GLenum _format = OpenGLPixelFormat(format);
		GLenum _type = OpenGLPixelType(type);
		
		uint align;
		uint str;		
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);		
		glTextureSubImage1D(id, 0, offset, size, _format, _type, pixels);
	}

	void Texture1D::GenerateMipmaps()
	{
		Graphics::Core::SelectTexture(this);
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