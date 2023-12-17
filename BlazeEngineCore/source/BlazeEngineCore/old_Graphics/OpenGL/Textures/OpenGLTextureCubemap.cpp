#pragma once
#include "BlazeEngine/Graphics/OpenGL/Textures/OpenGLTextureCubemap.h"
#include "BlazeEngine/Internal/Conversions.h"

#include <GL/glew.h>

#include "BlazeEngine/Graphics/GraphicsCore.h"

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

	Result TextureCubemap::SetSettings(TextureCubemapSettings settings)
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

		if (settings.mipmaps)
			glGenerateTextureMipmap(id);

		return Result();
	}	

	Result TextureCubemap::Load(StringView path, CubemapFileType fileType)
	{

		Bitmap bm;
		CHECK_RESULT(bm.Load(path));		

		Result result;
		GLenum format = OpenGLPixelFormat(bm.GetPixelFormat(), result);
		CHECK_RESULT(result);
		GLenum type = OpenGLPixelType(bm.GetPixelType(), result);
		CHECK_RESULT(result);
		
		Graphics::Renderer::SelectTexture(this);		

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

		return result;
	}
	Result TextureCubemap::Load(StringView path, CubemapFace face)
	{

		Bitmap bm;
		CHECK_RESULT(bm.Load(path));
		
		size = bm.GetSize().x;

		Result result;
		GLenum format = OpenGLPixelFormat(bm.GetPixelFormat(), result);
		CHECK_RESULT(result);
		GLenum type = OpenGLPixelType(bm.GetPixelType(), result);
		CHECK_RESULT(result);

		Graphics::Renderer::SelectTexture(this);
		glTexImage2D((GLenum)face, 0, format, size, size, 0, format, type, bm.GetPixels());

		return result;
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