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

	Result Texture2D::SetSettings(Texture2DSettings settings)
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

		Graphics::Core::SelectTexture(this);
		if (settings.mipmaps)
			glGenerateMipmap(GL_TEXTURE_2D);

		return result;
	}		
	Result Texture2D::Create(Vec2i size, TextureInternalPixelFormat internalFormat)
	{
		Result result;

		auto internalPixelFormat = OpenGLInternalPixelFormat(internalFormat, result);
		CHECK_RESULT(result);
		auto format = OpenGLFormatByInternalPixelFormat(internalFormat, result);
		CHECK_RESULT(result);

		this->size = size;		
		Graphics::Core::SelectTexture(this);				
		glTexImage2D(GL_TEXTURE_2D, 0, internalPixelFormat, size.x, size.y, 0, format, GL_UNSIGNED_BYTE, nullptr);

		return Result();
	}
	Result Texture2D::Create(BitmapView bm)
	{	
		Result result;
		auto internalFormat = MapInternalTexturePixelFormat(bm.GetPixelFormat(), result);
		CHECK_RESULT(result);

		Create(bm, internalFormat);

		return result;
	}
	Result Texture2D::Create(BitmapView bm, TextureInternalPixelFormat internalFormat)
	{
		Result result;

		size = bm.GetSize();

		Graphics::Core::SelectTexture(this);

		GLenum format = OpenGLPixelFormat(bm.GetPixelFormat(), result);
		CHECK_RESULT(result);
		GLenum type = OpenGLPixelType(bm.GetPixelType(), result);
		CHECK_RESULT(result);
		auto internalPixelFormat = OpenGLInternalPixelFormat(internalFormat, result);
		CHECK_RESULT(result);

		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
		glTexImage2D(GL_TEXTURE_2D, 0, internalPixelFormat, size.x, size.y, 0, format, type, bm.GetPixels());

		return result;
	}

	Result Texture2D::Load(Path path)
	{
		Result result;

		unsigned bm = ilGenImage();
		ilBindImage(bm);
				
		std::wstring wide = to_wstring((std::string)path.GetString().Ptr());
		if (!ilLoadImage(wide.c_str()))
		{
			ilDeleteImage(bm);

			return BLAZE_ERROR_RESULT("Blaze Engine", "Failed to load image file with DevIL error code: " + StringParsing::Convert(ilGetError()));			
		}

		BitmapPixelFormat bmFormat = DevILToBlazePixelFormat(ilGetInteger(IL_IMAGE_FORMAT), result);
		CHECK_RESULT(result);
		BitmapPixelType bmType = DevILToBlazePixelType(ilGetInteger(IL_IMAGE_TYPE), result);
		CHECK_RESULT(result);
		Vec2i bmSize = Vec2i(ilGetInteger(IL_IMAGE_WIDTH), ilGetInteger(IL_IMAGE_HEIGHT));

		result = Create(BitmapView(bmSize, bmFormat, bmType, ilGetData()));

		ilDeleteImage(bm);
		
		return result;
	}
	Result Texture2D::Load(Path path, TextureInternalPixelFormat internalFormat)
	{
		Result result;

		unsigned bm = ilGenImage();
		ilBindImage(bm);

		std::wstring wide = to_wstring((std::string)path.GetString().Ptr());
		if (!ilLoadImage(wide.c_str()))
		{			
			ilDeleteImage(bm);
			return BLAZE_WARNING_RESULT("Blaze Engine", "Failed to load image file with DevIL error code: " + StringParsing::Convert(ilGetError()));			
		}

		BitmapPixelFormat bmFormat = DevILToBlazePixelFormat(ilGetInteger(IL_IMAGE_FORMAT), result);
		CHECK_RESULT(result);
		BitmapPixelType bmType = DevILToBlazePixelType(ilGetInteger(IL_IMAGE_TYPE), result);
		CHECK_RESULT(result);
		Vec2i bmSize = Vec2i(ilGetInteger(IL_IMAGE_WIDTH), ilGetInteger(IL_IMAGE_HEIGHT));

		result = Create(BitmapView(bmSize, bmFormat, bmType, ilGetData()), internalFormat);

		ilDeleteImage(bm);

		return result;
	}


	Result Texture2D::SetPixels(Vec2i offset, BitmapView bm)
	{		
		Graphics::Core::SelectTexture(this);

		Result result;
		GLenum format = OpenGLPixelFormat(bm.GetPixelFormat(), result);
		CHECK_RESULT(result);
		GLenum type = OpenGLPixelType(bm.GetPixelType(), result);
		CHECK_RESULT(result);

		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
		glTexSubImage2D(GL_TEXTURE_2D, 0, offset.x, offset.y, bm.GetSize().x, bm.GetSize().y, format, type, bm.GetPixels());

		return result;
	}

	Result Texture2D::SetPixels(Vec2i offset, Vec2i size, uint stride, BitmapPixelFormat format, BitmapPixelType type, void* pixels)
	{
		Result result;
		GLenum _format = OpenGLPixelFormat(format, result);
		CHECK_RESULT(result);
		GLenum _type = OpenGLPixelType(type, result);
		CHECK_RESULT(result);

		auto pixelTypeSize = PixelTypeSize(type, result);
		CHECK_RESULT(result);
		auto formatDepth = GetFormatDepth(format, result);
		CHECK_RESULT(result);
		
		uint align;
		uint str;
		CalculateAlignmentAndStride(size.x, stride, pixelTypeSize * formatDepth, align, str);
		glPixelStorei(GL_UNPACK_ALIGNMENT, align);
		glPixelStorei(GL_UNPACK_ROW_LENGTH, str);		
		glTextureSubImage2D(id, 0, offset.x, offset.y, size.x, size.y, _format, _type, pixels);

		return result;
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