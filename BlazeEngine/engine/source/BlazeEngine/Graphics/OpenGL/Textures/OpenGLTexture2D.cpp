#pragma once
#include "BlazeEngine/Graphics/OpenGL/Textures/OpenGLTexture2D.h"
#include "source/BlazeEngine/Internal/Conversions.h"

#include "GL/glew.h"
#include "IL/il.h"

#include "BlazeEngine/Graphics/GraphicsCore.h"

#include <codecvt>

extern std::wstring to_wstring(const std::string& s);

namespace Blaze::OpenGL
{
	void CalculateAlignmentAndStride(uint width, uint byteStride, uint pixelSize, uint& align, uint& pixelStride)
	{
		uint byteWidth = width * pixelSize;

		if (byteWidth == byteStride)
		{
			align = 1;
		}
		else
		{
			if (byteStride % 8 == 0)
			{
				align = 8;
			}
			if (byteStride % 4 == 0)
			{
				align = 4;
			}
			else if (byteStride % 2 == 0)
			{
				align = 2;
			}
			else
				align = 1;
		}
		
		pixelStride = byteStride / pixelSize;		
	}

	Texture2D::Texture2D()
		: id(-1), size(0)
	{
		glGenTextures(1, &id);
	}
	Texture2D::Texture2D(Texture2D&& tex) noexcept
		: id(tex.id), size(tex.size)	
	{
		tex.id = -1;
	}
	Texture2D::~Texture2D()
	{
		if (id != -1)
			glDeleteTextures(1, &id);
	}

	void Texture2D::SetSettings(Texture2DSettings settings)
	{
		GLenum _min = OpenGLTextureMinSampling(settings.min, settings.mip, settings.mipmaps);		
		GLenum _mag = OpenGLTextureMagSampling(settings.mag);		
		GLenum _xWrap = OpenGLTextureWrapping(settings.xWrap);
		GLenum _yWrap = OpenGLTextureWrapping(settings.yWrap);
		
		glTextureParameteri(id, GL_TEXTURE_WRAP_S, _xWrap);
		glTextureParameteri(id, GL_TEXTURE_WRAP_T, _yWrap);
		glTextureParameteri(id, GL_TEXTURE_MIN_FILTER, _min);
		glTextureParameteri(id, GL_TEXTURE_MAG_FILTER, _mag);

		Graphics::Core::SelectTexture(this);
		if (settings.mipmaps)
			glGenerateMipmap(GL_TEXTURE_2D);
	}		
	void Texture2D::Create(Vec2i size, TextureInternalPixelFormat internalFormat)
	{
		this->size = size;		
		Graphics::Core::SelectTexture(this);				
		glTexImage2D(GL_TEXTURE_2D, 0, OpenGLInternalPixelFormat(internalFormat), size.x, size.y, 0, OpenGLFormatByInternalPixelFormat(internalFormat), GL_UNSIGNED_BYTE, nullptr);
	}
	void Texture2D::Create(BitmapView bm)
	{
		Create(bm, MapInternalTexturePixelFormat(bm.GetPixelFormat()));
	}
	void Texture2D::Create(BitmapView bm, TextureInternalPixelFormat internalFormat)
	{
		size = bm.GetSize();

		Graphics::Core::SelectTexture(this);

		GLenum format = OpenGLPixelFormat(bm.GetPixelFormat());
		GLenum type = OpenGLPixelType(bm.GetPixelType());

		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
		glTexImage2D(GL_TEXTURE_2D, 0, OpenGLInternalPixelFormat(internalFormat), size.x, size.y, 0, format, type, bm.GetPixels());
	}

	Result Texture2D::Load(Path path)
	{
		unsigned bm = ilGenImage();
		ilBindImage(bm);
				
		std::wstring wide = to_wstring((std::string)path.GetString().Ptr());
		if (!ilLoadImage(wide.c_str()))
		{
			ilDeleteImage(bm);

			return BLAZE_ERROR_RESULT("Blaze Engine", "Failed to load image file with DevIL error code: " + StringParsing::Convert(ilGetError()).value);			
		}

		BitmapPixelFormat bmFormat = DevILToBlazePixelFormat(ilGetInteger(IL_IMAGE_FORMAT));
		BitmapPixelType bmType = DevILToBlazePixelType(ilGetInteger(IL_IMAGE_TYPE));
		Vec2i bmSize = Vec2i(ilGetInteger(IL_IMAGE_WIDTH), ilGetInteger(IL_IMAGE_HEIGHT));

		Create(BitmapView(bmSize, bmFormat, bmType, ilGetData()));

		ilDeleteImage(bm);
		return Result();
	}
	Result Texture2D::Load(Path path, TextureInternalPixelFormat internalFormat)
	{
		unsigned bm = ilGenImage();
		ilBindImage(bm);

		std::wstring wide = to_wstring((std::string)path.GetString().Ptr());
		if (!ilLoadImage(wide.c_str()))
		{			
			ilDeleteImage(bm);
			return BLAZE_WARNING_RESULT("Blaze Engine", "Failed to load image file with DevIL error code: " + StringParsing::Convert(ilGetError()).value);			
		}

		BitmapPixelFormat bmFormat = DevILToBlazePixelFormat(ilGetInteger(IL_IMAGE_FORMAT));
		BitmapPixelType bmType = DevILToBlazePixelType(ilGetInteger(IL_IMAGE_TYPE));
		Vec2i bmSize = Vec2i(ilGetInteger(IL_IMAGE_WIDTH), ilGetInteger(IL_IMAGE_HEIGHT));

		Create(BitmapView(bmSize, bmFormat, bmType, ilGetData()), internalFormat);

		ilDeleteImage(bm);
		return Result();
	}


	void Texture2D::SetPixels(Vec2i offset, BitmapView bm)
	{
		Graphics::Core::SelectTexture(this);

		GLenum format = OpenGLPixelFormat(bm.GetPixelFormat());
		GLenum type = OpenGLPixelType(bm.GetPixelType());

		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
		glTexSubImage2D(GL_TEXTURE_2D, 0, offset.x, offset.y, bm.GetSize().x, bm.GetSize().y, format, type, bm.GetPixels());
	}

	void Texture2D::SetPixels(Vec2i offset, Vec2i size, uint stride, BitmapPixelFormat format, BitmapPixelType type, void* pixels)
	{
		GLenum _format = OpenGLPixelFormat(format);
		GLenum _type = OpenGLPixelType(type);
		
		uint align;
		uint str;
		CalculateAlignmentAndStride(size.x, stride, PixelTypeSize(type) * GetFormatDepth(format), align, str);
		glPixelStorei(GL_UNPACK_ALIGNMENT, align);
		glPixelStorei(GL_UNPACK_ROW_LENGTH, str);		
		glTextureSubImage2D(id, 0, offset.x, offset.y, size.x, size.y, _format, _type, pixels);
	}

	void Texture2D::GenerateMipmaps()
	{
		Graphics::Core::SelectTexture(this);
		glGenerateMipmap(GL_TEXTURE_2D);
	}

	Texture2D& Texture2D::operator=(Texture2D&& tex) noexcept
	{
		if (id != -1)
			glDeleteTextures(1, &id);
		id = tex.id;
		tex.id = -1;
		size = tex.size;
		return *this;
	}
}