#include "pch.h"
#include "BlazeEngine/Graphics/Core/OpenGL/OpenGLWrapper/Textures/OpenGLTexture1D.h"
#include "BlazeEngine/Graphics/Core/OpenGL/OpenGLWrapper/OpenGLConversions.h"
#include <GL/glew.h>

namespace Blaze::Graphics::OpenGL
{
	Texture1D::Texture1D()
		: id(0), size(0)
	{
		glGenTextures(1, &id);

		//It is important to bind the texture to set it up as a GL_TEXTURE_2D, otherwise glTextureStorage2D wont work
		glBindTexture(GL_TEXTURE_1D, id);
	}
	Texture1D::Texture1D(Texture1D&& tex) noexcept
		: id(tex.id), size(tex.size)	
	{
		tex.id = 0;
	}
	Texture1D::~Texture1D()
	{
		if (id != 0)
			glDeleteTextures(1, &id);
	}
	
	void Texture1D::Create(uint size, TextureInternalPixelFormat internalFormat, const Texture1DSettings& settings)
	{
		GLenum internalPixelFormat;
		if (!OpenGLInternalPixelFormat(internalFormat, internalPixelFormat))
		{
			BLAZE_LOG_ERROR("Invalid TextureInternalPixelFormat enum value");
			return;
		}

		this->size = size;						
		glTextureStorage1D(id, settings.textureLevelCount, internalPixelFormat, size);				

		SetSettings(settings);
	}
	void Texture1D::Create(uint size, TextureInternalPixelFormat internalFormat, BitmapColorFormat format, BitmapColorComponentType type, void* pixels, const Texture1DSettings& settings)
	{
		Create(size, internalFormat, settings);
		SetPixels(0, size, format, type, pixels, 0);
	}
	void Texture1D::SetPixels(uint offset, uint size, BitmapColorFormat format, BitmapColorComponentType type, void* pixels, uint textureLevel)
	{				
		GLenum _format, _type;
		if (!OpenGLPixelFormat(format, _format))
		{
			BLAZE_LOG_ERROR("Invalid BitmapColorFormat enum value");
			return;
		}
		if (!OpenGLPixelType(type, _type))
		{
			BLAZE_LOG_ERROR("Invalid BitmapColorComponentType enum value");
			return;
		}

		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);		
		glTextureSubImage1D(id, textureLevel, static_cast<GLint>(offset), static_cast<GLsizei>(size), _format, _type, pixels);				
	}
	void Texture1D::AutoGenerateMipmaps()
	{
		glGenerateTextureMipmap(id);
	}	

	Texture1D& Texture1D::operator=(Texture1D&& tex) noexcept
	{
		if (id != 0)
			glDeleteTextures(1, &id);
		id = tex.id;
		tex.id = 0;
		size = tex.size;
		return *this;
	}

	void Texture1D::SetSettings(const Texture1DSettings& settings)
	{
		GLenum _min;
		if (!OpenGLTextureMinSampling(settings.min, settings.mip, settings.textureLevelCount > 1, _min))
		{
			BLAZE_LOG_ERROR("Invalid TextureSampling enum value");
			return;
		}
		GLenum _mag;
		if (!OpenGLTextureMagSampling(settings.mag, _mag))
		{
			BLAZE_LOG_ERROR("Invalid TextureSampling enum value");
			return;
		}
		GLenum _xWrap;
		if (!OpenGLTextureWrapping(settings.xWrap, _xWrap))
		{
			BLAZE_LOG_ERROR("Invalid TextureWrapping enum value");
			return;
		}

		glTextureParameteri(id, GL_TEXTURE_WRAP_S, _xWrap);
		glTextureParameteri(id, GL_TEXTURE_MIN_FILTER, _min);
		glTextureParameteri(id, GL_TEXTURE_MAG_FILTER, _mag);
	}
}