#include "pch.h"
#include "BlazeEngineGraphics/Core/OpenGL/OpenGLWrapper/OpenGLContext.h"
#include "BlazeEngineGraphics/Core/OpenGL/OpenGLWrapper/OpenGLConversions.h"

namespace Blaze::Graphics::OpenGLWrapper
{
	void CalculateAlignmentAndStride(uint width, uintMem byteStride, uint pixelSize, uint& align, uint& pixelStride)
	{
		uintMem byteWidth = (uintMem)width * pixelSize;

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
		
		pixelStride = (uint)byteStride / pixelSize;
	}

	static StringView GetGLError()
	{
		const char* ptr = (const char*)gluErrorString(glGetError());
		uintMem len = strlen(ptr);
		return StringView(ptr, len);
	}

	Texture2D::Texture2D()
		: id(0), size(0)
	{				
		glGenTextures(1, &id);
		
		if (id == 0)
		{
			Debug::Logger::LogError("Blaze Engine Graphics", "Failed to create texture object");

			if (glGetError() != GL_NO_ERROR)
				Debug::Logger::LogError("Blaze Engine Graphics", "OpenGL threw an error: \"" + GetGLError() + "\"");
		}
	}
	Texture2D::Texture2D(Texture2D&& tex) noexcept
		: id(tex.id), size(tex.size)
	{
		tex.id = 0;
		tex.size = Vec2u();
	}
	Texture2D::~Texture2D()
	{
		if (id != 0)
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
		glTextureParameteri(id, GL_TEXTURE_BASE_LEVEL, 0);

		SelectTexture(this);
		if (settings.mipmaps)
			glGenerateMipmap(GL_TEXTURE_2D);

		glTextureParameteri(id, GL_TEXTURE_BASE_LEVEL, 0);

		return result;
	}		
	void Texture2D::SetSwizzle(TextureSwizzle red, TextureSwizzle green, TextureSwizzle blue, TextureSwizzle alpha)
	{		
		auto set = [&](GLenum channel, TextureSwizzle value)
			{
			switch (value)
			{
			case TextureSwizzle::R: glTextureParameteri(id, channel, GL_RED); break;
			case TextureSwizzle::G: glTextureParameteri(id, channel, GL_GREEN); break;
			case TextureSwizzle::B: glTextureParameteri(id, channel, GL_BLUE); break;
			case TextureSwizzle::A: glTextureParameteri(id, channel, GL_ALPHA); break;
			case TextureSwizzle::One: glTextureParameteri(id, channel, GL_ONE); break;
			case TextureSwizzle::Zero: glTextureParameteri(id, channel, GL_ZERO); break;
			}
			};

		set(GL_TEXTURE_SWIZZLE_R, red);
		set(GL_TEXTURE_SWIZZLE_G, green);
		set(GL_TEXTURE_SWIZZLE_B, blue);
		set(GL_TEXTURE_SWIZZLE_A, alpha);
	}

	Result Texture2D::Create(Vec2u size, TextureInternalPixelFormat internalFormat)
	{
		Result result;

		auto internalPixelFormat = OpenGLInternalPixelFormat(internalFormat, result);
		CHECK_RESULT(result);
		auto format = OpenGLFormatByInternalPixelFormat(internalFormat, result);
		CHECK_RESULT(result);

		this->size = size;		
		SelectTexture(this);				
		glTexImage2D(GL_TEXTURE_2D, 0, internalPixelFormat, size.x, size.y, 0, format, GL_UNSIGNED_BYTE, nullptr);
		glTextureParameteri(id, GL_TEXTURE_BASE_LEVEL, 0);

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
	Result Texture2D::Create(BitmapView sourceBitmap, TextureInternalPixelFormat internalFormat)
	{		
		Bitmap bitmap{ sourceBitmap, sourceBitmap.GetByteWidth() };

		Result result;
		size = bitmap.GetSize();

		GLenum format = OpenGLPixelFormat(bitmap.GetPixelFormat(), result);
		CHECK_RESULT(result);
		GLenum type = OpenGLPixelType(bitmap.GetPixelType(), result);
		CHECK_RESULT(result);
		auto internalPixelFormat = OpenGLInternalPixelFormat(internalFormat, result);
		CHECK_RESULT(result);
		
		SelectTexture(this);
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
		glTexImage2D(GL_TEXTURE_2D, 0, internalPixelFormat, size.x, size.y, 0, format, type, bitmap.GetPixels());
		glTextureParameteri(id, GL_TEXTURE_BASE_LEVEL, 0);

		SetSettings({ });

		return result;
	}
	 
	Result Texture2D::Load(Path path) 
	{				
		Bitmap bm;
		
		if (Result r = bm.Load(path, true))
			return r + BLAZE_ERROR_RESULT("", "");

		Create(bm);		

		return Result();
	}
	Result Texture2D::Load(Path path, TextureInternalPixelFormat internalFormat)
	{
		Bitmap bm;
		CHECK_RESULT(bm.Load(path, true));
		
		CHECK_RESULT(Create(bm, internalFormat));

		return Result();
	}

	Bitmap Texture2D::GetBitmap(BitmapColorFormat colorFormat, BitmapColorComponentType componentType) const
	{
		Bitmap bm;		
		Vec2u size = GetSize();

		Result result;		
		GLenum _format = OpenGLPixelFormat(colorFormat, result);	
		if (result) return Bitmap();

		bm.Create(size, colorFormat, componentType, nullptr);		

		GLenum _type = OpenGLPixelType(componentType, result);		
		if (result) return Bitmap();

		glGetTextureImage(id, 0, _format, _type, bm.GetPixelSize() * size.x * size.y, bm.GetPixels());		

		return bm; 
	}

	Result Texture2D::CopyPixels(Vec2u offset, BitmapView bm)
	{		
		SelectTexture(this);

		Result result;
		GLenum format = OpenGLPixelFormat(bm.GetPixelFormat(), result);
		CHECK_RESULT(result);
		GLenum type = OpenGLPixelType(bm.GetPixelType(), result);
		CHECK_RESULT(result);

		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
		glTexSubImage2D(GL_TEXTURE_2D, 0, offset.x, offset.y, bm.GetSize().x, bm.GetSize().y, format, type, bm.GetPixels());
		glTextureParameteri(id, GL_TEXTURE_BASE_LEVEL, 0);

		return result;
	}	

	void Texture2D::GenerateMipmaps()
	{
		glTextureParameteri(id, GL_TEXTURE_BASE_LEVEL, 0);
		SelectTexture(this);
		glGenerateMipmap(GL_TEXTURE_2D);
		glTextureParameteri(id, GL_TEXTURE_BASE_LEVEL, 0);
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