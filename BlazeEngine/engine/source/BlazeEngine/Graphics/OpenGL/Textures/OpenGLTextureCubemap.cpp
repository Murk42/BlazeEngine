#pragma once
#include "BlazeEngine/Graphics/OpenGL/Textures/OpenGLTextureCubemap.h"
#include "source/BlazeEngine/Internal/Conversions.h"

#include <GL/glew.h>

#include "BlazeEngine/Graphics/Renderer.h"

namespace Blaze::OpenGL
{
	TextureCubemap::TextureCubemap()
		: id(-1), size(0)
	{
		glGenTextures(1, &id);		
	}
	TextureCubemap::TextureCubemap(TextureCubemap&& tex) noexcept
		: id(tex.id), size(tex.size)
	{		
		tex.id = -1;
	}
	TextureCubemap::~TextureCubemap()
	{
		if (id != -1)
			glDeleteTextures(1, &id);
	}

	void TextureCubemap::SetSettings(TextureCubemapSettings settings)
	{
		unsigned _min;
		if (settings.mip == TextureSampling::Nearest)
			if (settings.min == TextureSampling::Nearest)
				_min = GL_NEAREST_MIPMAP_NEAREST;
			else
				_min = GL_NEAREST_MIPMAP_LINEAR;
		else
			if (settings.min == TextureSampling::Nearest)
				_min = GL_LINEAR_MIPMAP_NEAREST;
			else
				_min = GL_LINEAR_MIPMAP_LINEAR;

		glTextureParameteri(id, GL_TEXTURE_WRAP_S, (uint)settings.xWrap);
		glTextureParameteri(id, GL_TEXTURE_WRAP_T, (uint)settings.yWrap);		
		glTextureParameteri(id, GL_TEXTURE_MIN_FILTER, _min);
		glTextureParameteri(id, GL_TEXTURE_MAG_FILTER, uint(settings.mag));
	}

	void TextureCubemap::GenerateMipmaps()
	{
		glGenerateTextureMipmap(id);
	}

	void TextureCubemap::Load(StringView path, CubemapFileType fileType)
	{
		Bitmap bm;
		bm.Load(path);

		GLenum format = OpenGLPixelFormat(bm.GetPixelFormat());
		GLenum type = OpenGLPixelType(bm.GetPixelType());
		
		Renderer::SelectTexture(this);		

		switch (fileType)
		{
		case CubemapFileType::VerticalCross: {
			size = bm.GetSize().y / 4;
			glPixelStorei(GL_UNPACK_ROW_LENGTH, bm.GetSize().x);

			glPixelStorei(GL_UNPACK_SKIP_PIXELS, size);

			glPixelStorei(GL_UNPACK_SKIP_ROWS, 0);
			glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, format, size, size, 0, format, type, bm.GetPixels());

			glPixelStorei(GL_UNPACK_SKIP_ROWS, size * 1);
			glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, format, size, size, 0, format, type, bm.GetPixels());

			glPixelStorei(GL_UNPACK_SKIP_ROWS, size * 3);
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, format, size, size, 0, format, type, bm.GetPixels());

			glPixelStorei(GL_UNPACK_SKIP_ROWS, size * 2);
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, format, size, size, 0, format, type, bm.GetPixels());

			glPixelStorei(GL_UNPACK_SKIP_PIXELS, 0);
			glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, format, size, size, 0, format, type, bm.GetPixels());

			glPixelStorei(GL_UNPACK_SKIP_PIXELS, size * 2);
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, format, size, size, 0, format, type, bm.GetPixels());
			break;
		}
		case CubemapFileType::HorizontalCross: {
			size = bm.GetSize().x / 4;
			glPixelStorei(GL_UNPACK_ROW_LENGTH, bm.GetSize().x);

			glPixelStorei(GL_UNPACK_SKIP_ROWS, size);

			glPixelStorei(GL_UNPACK_SKIP_PIXELS, 0);
			glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, format, size, size, 0, format, type, bm.GetPixels());

			glPixelStorei(GL_UNPACK_SKIP_PIXELS, size * 2);
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, format, size, size, 0, format, type, bm.GetPixels());

			glPixelStorei(GL_UNPACK_SKIP_PIXELS, size * 3);
			glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, format, size, size, 0, format, type, bm.GetPixels());

			glPixelStorei(GL_UNPACK_SKIP_PIXELS, size);
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, format, size, size, 0, format, type, bm.GetPixels());

			glPixelStorei(GL_UNPACK_SKIP_ROWS, 0);
			glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, format, size, size, 0, format, type, bm.GetPixels());

			glPixelStorei(GL_UNPACK_SKIP_ROWS, size * 2);
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, format, size, size, 0, format, type, bm.GetPixels());
			break;
		}
		case CubemapFileType::Column: {
			size = bm.GetSize().y / 6;
			glPixelStorei(GL_UNPACK_ROW_LENGTH, bm.GetSize().x);

			glPixelStorei(GL_UNPACK_SKIP_PIXELS, 0);

			glPixelStorei(GL_UNPACK_SKIP_ROWS, 0);
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, format, size, size, 0, format, type, bm.GetPixels());

			glPixelStorei(GL_UNPACK_SKIP_ROWS, size);
			glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, format, size, size, 0, format, type, bm.GetPixels());

			glPixelStorei(GL_UNPACK_SKIP_ROWS, size * 2);
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, format, size, size, 0, format, type, bm.GetPixels());

			glPixelStorei(GL_UNPACK_SKIP_ROWS, size * 3);
			glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, format, size, size, 0, format, type, bm.GetPixels());

			glPixelStorei(GL_UNPACK_SKIP_ROWS, size * 4);
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, format, size, size, 0, format, type, bm.GetPixels());

			glPixelStorei(GL_UNPACK_SKIP_ROWS, size * 5);
			glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, format, size, size, 0, format, type, bm.GetPixels());
			break;
		}
		case CubemapFileType::Row: {
			size = bm.GetSize().x / 6;
			glPixelStorei(GL_UNPACK_ROW_LENGTH, bm.GetSize().x);

			glPixelStorei(GL_UNPACK_SKIP_ROWS, 0);

			glPixelStorei(GL_UNPACK_SKIP_PIXELS, 0);
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, format, size, size, 0, format, type, bm.GetPixels());

			glPixelStorei(GL_UNPACK_SKIP_PIXELS, size);
			glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, format, size, size, 0, format, type, bm.GetPixels());

			glPixelStorei(GL_UNPACK_SKIP_PIXELS, size * 2);
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, format, size, size, 0, format, type, bm.GetPixels());

			glPixelStorei(GL_UNPACK_SKIP_PIXELS, size * 3);
			glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, format, size, size, 0, format, type, bm.GetPixels());

			glPixelStorei(GL_UNPACK_SKIP_PIXELS, size * 4);
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, format, size, size, 0, format, type, bm.GetPixels());

			glPixelStorei(GL_UNPACK_SKIP_PIXELS, size * 5);
			glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, format, size, size, 0, format, type, bm.GetPixels());
			break;
		}
		}

		glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
		glPixelStorei(GL_UNPACK_SKIP_PIXELS, 0);
		glPixelStorei(GL_UNPACK_SKIP_ROWS, 0);
	}
	void TextureCubemap::Load(StringView path, CubemapFace face)
	{
		Bitmap bm;
		bm.Load(path);
		
		size = bm.GetSize().x;

		GLenum format = OpenGLPixelFormat(bm.GetPixelFormat());
		GLenum type = OpenGLPixelType(bm.GetPixelType());

		Renderer::SelectTexture(this);
		glTexImage2D((GLenum)face, 0, format, size, size, 0, format, type, bm.GetPixels());
	}		

	TextureCubemap& TextureCubemap::operator=(TextureCubemap&& tex) noexcept
	{
		if (id != -1)
			glDeleteTextures(1, &id);
		id = tex.id;
		tex.id = -1;
		size = tex.size;		
		return *this;
	}
}