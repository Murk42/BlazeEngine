#pragma once
#include "BlazeEngine/Graphics/OpenGL/Textures/OpenGLTexture2DArray.h"
#include "BlazeEngine/Internal/Conversions.h"

#include <GL/glew.h>

#include "BlazeEngine/Graphics/GraphicsCore.h"

namespace Blaze::OpenGL
{
	Texture2DArray::Texture2DArray()
		: id(-1), size(0, 0), layers(0)
	{
		glGenTextures(1, &id);
	}
	Texture2DArray::Texture2DArray(Texture2DArray&& tex) noexcept
		: id(tex.id), size(0, 0), layers(0)
	{
		tex.id = -1;
	}
	Texture2DArray::~Texture2DArray()
	{
		if (id != -1)
			glDeleteTextures(1, &id);
	}

	Result Texture2DArray::SetSettings(Texture2DArraySettings settings)
	{
		Result result;

		GLenum _min = OpenGLTextureMinSampling(settings.min, settings.mip, settings.mipmaps, result);
		CHECK_RESULT(result);
		GLenum _mag = OpenGLTextureMagSampling(settings.mag, result);
		CHECK_RESULT(result);
		GLenum _xWrap = OpenGLTextureWrapping(settings.xWrap, result);
		CHECK_RESULT(result);
		GLenum _yWrap = OpenGLTextureWrapping(settings.yWrap, result);
		CHECK_RESULT(result);		

		glTextureParameteri(id, GL_TEXTURE_WRAP_S, _xWrap);
		glTextureParameteri(id, GL_TEXTURE_WRAP_T, _yWrap);
		glTextureParameteri(id, GL_TEXTURE_MIN_FILTER, _min);
		glTextureParameteri(id, GL_TEXTURE_MAG_FILTER, _mag);

		return result;
	}	

	Result Texture2DArray::Create(Vec2i size, uint layers, TextureInternalPixelFormat internalFormat)
	{
		Result result;

		auto internalPixelFormat = OpenGLInternalPixelFormat(internalFormat, result);
		CHECK_RESULT(result);
		auto format = OpenGLFormatByInternalPixelFormat(internalFormat, result);
		CHECK_RESULT(result);

		this->size = size;
		this->layers = layers;

		Graphics::Renderer::SelectTexture(this);
		glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, internalPixelFormat, static_cast<GLsizei>(size.x), static_cast<GLsizei>(size.y), static_cast<GLsizei>(layers), 0, format, GL_UNSIGNED_BYTE, nullptr);

		return Result();
	}

	Result Texture2DArray::SetPixels(Vec2i offset, uint layer, BitmapView bm)
	{
		Result result;

		GLenum format = OpenGLPixelFormat(bm.GetPixelFormat(), result);
		CHECK_RESULT(result);
		GLenum type = OpenGLPixelType(bm.GetPixelType(), result);
		CHECK_RESULT(result);

		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		Graphics::Renderer::SelectTexture(this);
		glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, static_cast<GLint>(layer), static_cast<GLsizei>(bm.GetSize().x), static_cast<GLsizei>(bm.GetSize().y), 1, format, type, bm.GetPixels());

		return Result();
	}

	Texture2DArray& Texture2DArray::operator=(Texture2DArray&& tex) noexcept
	{
		if (id != -1)
			glDeleteTextures(1, &id);
		id = tex.id;
		tex.id = -1;
		return *this;
	}
}