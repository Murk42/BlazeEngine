#include "pch.h"
#include "BlazeEngineGraphics/Core/OpenGL/OpenGLWrapper/Textures/OpenGLTexture1D.h"
#include "BlazeEngineGraphics/Core/OpenGL/OpenGLWrapper/OpenGLConversions.h"
#include <GL/glew.h>

namespace Blaze::Graphics::OpenGLWrapper
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
		Result result;
		auto internalPixelFormat = OpenGLInternalPixelFormat(internalFormat, result);
		if (result) return;		

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
		Result result;
		
		GLenum _format = OpenGLPixelFormat(format, result);
		if (result) return;
		GLenum _type = OpenGLPixelType(type, result);
		if (result) return;

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
		Result result;
		GLenum _min = OpenGLTextureMinSampling(settings.min, settings.mip, settings.textureLevelCount > 1, result);
		if (result) return;
		GLenum _mag = OpenGLTextureMagSampling(settings.mag, result);
		if (result) return;
		GLenum _xWrap = OpenGLTextureWrapping(settings.xWrap, result);
		if (result) return;

		glTextureParameteri(id, GL_TEXTURE_WRAP_S, _xWrap);
		glTextureParameteri(id, GL_TEXTURE_MIN_FILTER, _min);
		glTextureParameteri(id, GL_TEXTURE_MAG_FILTER, _mag);
	}
}