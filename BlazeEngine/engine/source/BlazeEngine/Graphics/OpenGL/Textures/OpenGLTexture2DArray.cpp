#pragma once
#include "BlazeEngine/Graphics/OpenGL/Textures/OpenGLTexture2DArray.h"
#include "source/BlazeEngine/Internal/Conversions.h"

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

	void Texture2DArray::SetSettings(Texture2DArraySettings settings)
	{
		unsigned _min;
		if (settings.mipmaps)
		{
			glGenerateTextureMipmap(id);

			if (settings.mip == TextureSampling::Nearest)
				if (settings.min == TextureSampling::Nearest)
					_min = GL_NEAREST_MIPMAP_NEAREST;
				else
					_min = GL_LINEAR_MIPMAP_NEAREST;
			else
				if (settings.min == TextureSampling::Nearest)
					_min = GL_NEAREST_MIPMAP_LINEAR;
				else
					_min = GL_LINEAR_MIPMAP_LINEAR;
		}
		else
			if (settings.min == TextureSampling::Nearest)
				_min = GL_NEAREST;
			else
				_min = GL_LINEAR;

		glTextureParameteri(id, GL_TEXTURE_WRAP_S, (uint)settings.xWrap);
		glTextureParameteri(id, GL_TEXTURE_WRAP_T, (uint)settings.yWrap);
		glTextureParameteri(id, GL_TEXTURE_MIN_FILTER, _min);
		glTextureParameteri(id, GL_TEXTURE_MAG_FILTER, uint(settings.mag));
	}	

	void Texture2DArray::Create(Vec2i size, uint layers, TextureInternalPixelFormat internalFormat)
	{
		this->size = size;
		this->layers = layers;

		Graphics::Core::SelectTexture(this);
		glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, OpenGLInternalPixelFormat(internalFormat), static_cast<GLsizei>(size.x), static_cast<GLsizei>(size.y), static_cast<GLsizei>(layers), 0, OpenGLFormatByInternalPixelFormat(internalFormat), GL_UNSIGNED_BYTE, nullptr);
	}

	void Texture2DArray::SetPixels(Vec2i offset, uint layer, BitmapView bm)
	{
		GLenum format = OpenGLPixelFormat(bm.GetPixelFormat());
		GLenum type = OpenGLPixelType(bm.GetPixelType());
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		Graphics::Core::SelectTexture(this);
		glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, static_cast<GLint>(layer), static_cast<GLsizei>(bm.GetSize().x), static_cast<GLsizei>(bm.GetSize().y), 1, format, type, bm.GetPixels());
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