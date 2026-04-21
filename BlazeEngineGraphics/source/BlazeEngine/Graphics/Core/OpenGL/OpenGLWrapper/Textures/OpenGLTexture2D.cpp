#include "pch.h"
#include "BlazeEngine/Graphics/Core/OpenGL/OpenGLWrapper/Textures/OpenGLTexture2D.h"
#include "BlazeEngine/Graphics/Core/OpenGL/OpenGLWrapper/OpenGLConversions.h"
#include <GL/glew.h>

namespace Blaze::Graphics::OpenGL
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

	Texture2D::Texture2D()
		: id(0), size(0)
	{
		glGenTextures(1, &id);

		//It is important to bind the texture to set it up as a GL_TEXTURE_2D, otherwise glTextureStorage2D wont work
		glBindTexture(GL_TEXTURE_2D, id);
	}
	Texture2D::Texture2D(Texture2D&& tex) noexcept
		: id(tex.id), size(tex.size)
	{
		tex.id = 0;
		tex.size = Vec2u();
	}
	Texture2D::Texture2D(Vec2u size, TextureInternalPixelFormat internalFormat, const Texture2DSettings& settings)
		: Texture2D()
	{
		Create(size, internalFormat, settings);
	}
	Texture2D::Texture2D(BitmapView bm, const Texture2DSettings& settings)
		: Texture2D()
	{
		Create(bm, settings);
	}
	Texture2D::Texture2D(BitmapView bm, TextureInternalPixelFormat internalFormat, const Texture2DSettings& settings)
		: Texture2D()
	{
		Create(bm, internalFormat, settings);
	}
	Texture2D::Texture2D(Path path, const Texture2DSettings& settings)
		: Texture2D()
	{
		Load(path, settings);
	}
	Texture2D::Texture2D(Path path, TextureInternalPixelFormat internalFormat, const Texture2DSettings& settings)
		: Texture2D()
	{
		Load(path, internalFormat, settings);
	}
	Texture2D::~Texture2D()
	{
		if (id != 0)
			glDeleteTextures(1, &id);
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

	void Texture2D::Create(Vec2u size, TextureInternalPixelFormat internalFormat, const Texture2DSettings& settings)
	{
		GLenum internalPixelFormat;
		if (!OpenGLInternalPixelFormat(internalFormat, internalPixelFormat))
		{
			BLAZE_LOG_ERROR("Invalid TextureInternalPixelFormat enum value");
			return;
		}

		this->size = size;

		glTextureStorage2D(id, settings.textureLevelCount, internalPixelFormat, size.x, size.y);

		SetSettings(settings);
	}
	void Texture2D::Create(BitmapView bm, const Texture2DSettings& settings)
	{
		TextureInternalPixelFormat internalFormat;
		if (!BlazeInternalTexturePixelFormat(bm.GetPixelFormat(), internalFormat))
		{
			BLAZE_LOG_ERROR("Invalid TextureInternalPixelFormat enum value");
			return;
		}

		Create(bm.GetSize(), internalFormat, settings);

		SetPixels(Vec2u(), bm, 0);
	}
	void Texture2D::Create(BitmapView bm, TextureInternalPixelFormat internalFormat, const Texture2DSettings& settings)
	{
		Create(bm.GetSize(), internalFormat, settings);
		SetPixels(Vec2u(), bm, 0);
	}

	void Texture2D::Load(Path path, const Texture2DSettings& settings)
	{
		Bitmap bm;

		bm.Load(path, true);
		Create(bm, settings);
	}
	void Texture2D::Load(Path path, TextureInternalPixelFormat internalFormat, const Texture2DSettings& settings)
	{
		Bitmap bm;

		bm.Load(path, true);

		Create(bm, internalFormat, settings);
	}

	Bitmap Texture2D::GetBitmap(BitmapColorFormat colorFormat, BitmapColorComponentType componentType) const
	{
		Bitmap bm;
		Vec2u size = GetSize();
		
		GLenum _format;
		if (!OpenGLPixelFormat(colorFormat, _format))
		{
			BLAZE_LOG_ERROR("Invalid BitmapColorFormat enum value");
			return { };
		}

		bm.Create(size, colorFormat, componentType, nullptr);

		GLenum _type;
		if (!OpenGLPixelType(componentType, _type))
		{
			BLAZE_LOG_ERROR("Invalid BitmapComponentColorType enum value");
			return { };
		}

		glPixelStorei(GL_PACK_ALIGNMENT, 1);
		glGetTextureImage(id, 0, _format, _type, static_cast<GLsizei>(bm.GetPixelSize() * size.x * size.y), bm.GetPixels());

		return bm;
	}

	void Texture2D::SetPixels(Vec2u offset, BitmapView bm, uint textureLevel)
	{		
		GLenum format;
		if (!OpenGLPixelFormat(bm.GetPixelFormat(), format))
		{
			BLAZE_LOG_ERROR("Invalid BitmapColorFormats enum value");
			return;
		}
		GLenum type;
		if (!OpenGLPixelType(bm.GetPixelType(), type))
		{
			BLAZE_LOG_ERROR("Invalid BitmapComponentColorType enum value");
			return;
		}

		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
		glTextureSubImage2D(id, textureLevel, offset.x, offset.y, bm.GetSize().x, bm.GetSize().y, format, type, bm.GetPixels());
	}

	void Texture2D::AutoGenerateMipmaps()
	{
		glGenerateTextureMipmap(id);
	}

	Texture2D& Texture2D::operator=(Texture2D&& tex) noexcept
	{
		if (id != 0)
			glDeleteTextures(1, &id);
		id = tex.id;
		tex.id = 0;
		size = tex.size;
		return *this;
	}

	void Texture2D::SetSettings(const Texture2DSettings& settings)
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
		GLenum _yWrap;
		if (!OpenGLTextureWrapping(settings.yWrap, _yWrap))
		{
			BLAZE_LOG_ERROR("Invalid TextureWrapping enum value");
			return;
		}

		glTextureParameteri(id, GL_TEXTURE_WRAP_S, _xWrap);
		glTextureParameteri(id, GL_TEXTURE_WRAP_T, _yWrap);
		glTextureParameteri(id, GL_TEXTURE_MIN_FILTER, _min);
		glTextureParameteri(id, GL_TEXTURE_MAG_FILTER, _mag);
	}
}