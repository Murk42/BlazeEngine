#include "BlazeEngine/Graphics/Texture.h"
#include "BlazeEngine/Utilities/File.h"
#include "BlazeEngine/Graphics/Bitmap.h"

#include "GL/glew.h"
#include "IL/il.h"
#include "IL/ilu.h"

namespace Blaze
{
	PixelFormat ToInternalFormat(PixelFormat format)
	{
		switch (format)
		{
		case PixelFormat::BGR: return PixelFormat::RGB;
		case PixelFormat::BGRA: return PixelFormat::RGBA;
		default: return format;
		}
	}

	Texture::Texture(TextureType type)
		: type(type), hasMipmaps(false)
	{		
		glGenTextures(1, &id);
		glBindTexture((GLenum)type, id);
		glTexParameteri((GLenum)type, GL_TEXTURE_WRAP_S, (uint)TextureWrapping::ClampToEdge);
		glTexParameteri((GLenum)type, GL_TEXTURE_WRAP_T, (uint)TextureWrapping::ClampToEdge);
		glTexParameteri((GLenum)type, GL_TEXTURE_MIN_FILTER, (uint)TextureSampling::Nearest);
		glTexParameteri((GLenum)type, GL_TEXTURE_MAG_FILTER, (uint)TextureSampling::Nearest);

		settings.xWrap = TextureWrapping::ClampToEdge;
		settings.yWrap = TextureWrapping::ClampToEdge;
		settings.min = TextureSampling::Nearest;
		settings.mag = TextureSampling::Nearest;
		settings.mag = TextureSampling::Nearest;
	}
	Texture::~Texture()
	{
		if (id != 0)
			glDeleteTextures(1, &id);			
	}
	
	TextureType Texture::GetType() const
	{
		return type;
	}

	TextureSettings Texture::GetSettings() const
	{
		return settings;
	}			

	void Texture::SetSettings(TextureSettings settings)
	{
		SetSettings(settings.xWrap, settings.yWrap, settings.min, settings.mag, settings.min);
	}
	void Texture::SetSettings(TextureWrapping xWrap, TextureWrapping yWrap)
	{
		Bind();		

		glTexParameteri((GLenum)type, GL_TEXTURE_WRAP_S, (uint)xWrap);
		glTexParameteri((GLenum)type, GL_TEXTURE_WRAP_T, (uint)yWrap);				

		settings.xWrap = xWrap;
		settings.yWrap = yWrap;		
	}
	void Texture::SetSettings(TextureSampling min, TextureSampling mag)
	{		
		Bind();
		unsigned _min = !hasMipmaps ? (uint)min : (min == TextureSampling::Nearest ? (settings.mip == TextureSampling::Nearest ? GL_NEAREST_MIPMAP_NEAREST : GL_NEAREST_MIPMAP_LINEAR) :
			(settings.mip == TextureSampling::Nearest ? GL_LINEAR_MIPMAP_NEAREST : GL_LINEAR_MIPMAP_LINEAR));
		/*
			GL_NEAREST = 0x2600 = 9728
			GL_LINEAR = 0x2601 = 9729
			GL_NEAREST_MIPMAP_NEAREST = 0x2700 = 9984
			GL_NEAREST_MIPMAP_LINEAR = 0x2702 = 9986
			GL_LINEAR_MIPMAP_NEAREST = 0x2701 = 9985
			GL_LINEAR_MIPMAP_LINEAR = 0x2703 = 9987
		*/
			
		
		glTexParameteri((GLenum)type, GL_TEXTURE_MIN_FILTER, _min);
		glTexParameteri((GLenum)type, GL_TEXTURE_MAG_FILTER, (uint)mag);
		
		settings.min = min;
		settings.mag = mag;		
	}
	void Texture::SetSettings(TextureWrapping xWrap, TextureWrapping yWrap, TextureSampling min, TextureSampling mag)
	{
		Bind();
		
		unsigned _min = !hasMipmaps ? (uint)min : (min == TextureSampling::Nearest ? (settings.mip == TextureSampling::Nearest ? GL_NEAREST_MIPMAP_NEAREST : GL_NEAREST_MIPMAP_LINEAR) :
			(settings.mip == TextureSampling::Nearest ? GL_LINEAR_MIPMAP_NEAREST : GL_LINEAR_MIPMAP_LINEAR));

		glTexParameteri((GLenum)type, GL_TEXTURE_WRAP_S, (uint)xWrap);
		glTexParameteri((GLenum)type, GL_TEXTURE_WRAP_T, (uint)yWrap);
		glTexParameteri((GLenum)type, GL_TEXTURE_MIN_FILTER, _min);
		glTexParameteri((GLenum)type, GL_TEXTURE_MAG_FILTER, uint(mag));

		settings.xWrap = xWrap;
		settings.yWrap = yWrap;
		settings.min = min;
		settings.mag = mag;		
	}
	void Texture::SetSettings(TextureSampling min, TextureSampling mag, TextureSampling mip)
	{
		Bind();

		unsigned _min = !hasMipmaps ? (uint)min : (min == TextureSampling::Nearest ? (settings.mip == TextureSampling::Nearest ? GL_NEAREST_MIPMAP_NEAREST : GL_NEAREST_MIPMAP_LINEAR) :
			(settings.mip == TextureSampling::Nearest ? GL_LINEAR_MIPMAP_NEAREST : GL_LINEAR_MIPMAP_LINEAR));

		glTexParameteri((GLenum)type, GL_TEXTURE_MIN_FILTER, _min);
		glTexParameteri((GLenum)type, GL_TEXTURE_MAG_FILTER, (uint)mag);
		
		settings.min = min;
		settings.mag = mag;
		settings.mip = mip;
	}
	void Texture::SetSettings(TextureWrapping xWrap, TextureWrapping yWrap, TextureSampling min, TextureSampling mag, TextureSampling mip)
	{
		Bind();

		unsigned _min = !hasMipmaps ? (uint)min : (min == TextureSampling::Nearest ? (settings.mip == TextureSampling::Nearest ? GL_NEAREST_MIPMAP_NEAREST : GL_NEAREST_MIPMAP_LINEAR) :
			(settings.mip == TextureSampling::Nearest ? GL_LINEAR_MIPMAP_NEAREST : GL_LINEAR_MIPMAP_LINEAR));

		glTexParameteri((GLenum)type, GL_TEXTURE_WRAP_S, (uint)xWrap);
		glTexParameteri((GLenum)type, GL_TEXTURE_WRAP_T, (uint)yWrap);
		glTexParameteri((GLenum)type, GL_TEXTURE_MIN_FILTER, _min);
		glTexParameteri((GLenum)type, GL_TEXTURE_MAG_FILTER, (uint)mag);

		settings.xWrap = xWrap;
		settings.yWrap = yWrap;
		settings.min = min;
		settings.mag = mag;
		settings.mip = mip;
	}

	void Texture::GenerateMipmaps()
	{
		hasMipmaps = true;
		Bind();
		glGenerateMipmap((GLenum)type);
	}

	void Texture::Bind() const
	{
		glBindTexture((GLenum)type, id);
	}
	void Texture::Bind(unsigned slot) const
	{
		glActiveTexture(GL_TEXTURE0 + slot);
		Bind();
	}

	Texture& Texture::operator=(Texture&& tex) noexcept
	{
		id = tex.id;
		settings = tex.settings;
		type = tex.type;
		hasMipmaps = tex.hasMipmaps;
		return *this;
	}



	Texture2D::Texture2D()
		: Texture(TextureType::Single2D), format(PixelFormat::RGBA), size(0)
	{
	}
	Texture2D::Texture2D(const String& path)
		: Texture2D()
	{
		Load(path);
	}		
	void Texture2D::Create(const Bitmap& bm)
	{		
		format = bm.GetFormat();
		size = bm.GetSize();

		glBindTexture(GL_TEXTURE_2D, id);

		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		glTexImage2D(GL_TEXTURE_2D, 0, (uint)ToInternalFormat(format), size.x, size.y, 0, (uint)format, GL_UNSIGNED_BYTE, bm.GetPixels());		
	}
	void Texture2D::Create(Vec2i size, PixelFormat format, void* pixels)
	{
		this->format = format;
		this->size = size;

		glBindTexture(GL_TEXTURE_2D, id);

		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		glTexImage2D(GL_TEXTURE_2D, 0, (uint)ToInternalFormat(format), size.x, size.y, 0, (uint)format, GL_UNSIGNED_BYTE, pixels);
	}

	void Texture2D::SetPixels(Vec2i offset, Vec2i size, PixelFormat format, void* pixels)
	{
		glBindTexture(GL_TEXTURE_2D, id);

		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		glTexSubImage2D(GL_TEXTURE_2D, 0, offset.x, offset.y, size.x, size.y, (uint)format, GL_UNSIGNED_BYTE, pixels);
	}

	void Texture2D::GetPixels(Bitmap& bm)
	{
		if (bm.GetSize() != size || bm.GetFormat() != format)
			bm.Create(size, format);

		glGetTexImage(GL_TEXTURE_2D, 0, (uint)format, GL_UNSIGNED_BYTE, bm.GetPixels());
	}

	Texture2D& Texture2D::operator=(Texture2D&& tex) noexcept
	{
		((Texture*)this)->operator=(std::move(tex));		
		size = tex.size;
		format = tex.format;		
		return *this;
	}

	void Texture2D::Load(const String& path)
	{		
		Bitmap bm;
		bm.Load(path);		

		size = bm.GetSize();
		format = bm.GetFormat();

		glBindTexture(GL_TEXTURE_2D, id);		

		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		glTexImage2D(GL_TEXTURE_2D, 0, (uint)ToInternalFormat(format), size.x, size.y, 0, (uint)format, GL_UNSIGNED_BYTE, bm.GetPixels());
	}
	void Texture2D::Save(const String& path)
	{
		Bitmap bm;
		GetPixels(bm);
		bm.Save(path);
	}	



	TextureCubemap::TextureCubemap()
		: Texture(TextureType::Cubemap), format(PixelFormat::RGBA), size(0)
	{
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, (uint)TextureWrapping::ClampToEdge);
	}
	TextureCubemap::TextureCubemap(const String& xp, const String& xn, const String& yp, const String& yn, const String& zp, const String& zn)
		: TextureCubemap()
	{
		Load(xp, xn, yp, yn, zp, zn);
	}
	
	void TextureCubemap::Load(StringView path, CubemapFileType type)
	{
		Bitmap bm;
		bm.Load(path);		

		format = bm.GetFormat();

		Bind();
		switch (type)
		{
		case CubemapFileType::VerticalCross: {
			size = bm.GetSize().y / 4;
			glPixelStorei(GL_UNPACK_ROW_LENGTH, bm.GetSize().x);

			glPixelStorei(GL_UNPACK_SKIP_PIXELS, size);

			glPixelStorei(GL_UNPACK_SKIP_ROWS, 0);
			glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, (uint)ToInternalFormat(format), size, size, 0, (uint)format, GL_UNSIGNED_BYTE, bm.GetPixels());

			glPixelStorei(GL_UNPACK_SKIP_ROWS, size * 1);
			glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, (uint)ToInternalFormat(format), size, size, 0, (uint)format, GL_UNSIGNED_BYTE, bm.GetPixels());

			glPixelStorei(GL_UNPACK_SKIP_ROWS, size * 3);
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, (uint)ToInternalFormat(format), size, size, 0, (uint)format, GL_UNSIGNED_BYTE, bm.GetPixels());

			glPixelStorei(GL_UNPACK_SKIP_ROWS, size * 2);
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, (uint)ToInternalFormat(format), size, size, 0, (uint)format, GL_UNSIGNED_BYTE, bm.GetPixels());

			glPixelStorei(GL_UNPACK_SKIP_PIXELS, 0);
			glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, (uint)ToInternalFormat(format), size, size, 0, (uint)format, GL_UNSIGNED_BYTE, bm.GetPixels());

			glPixelStorei(GL_UNPACK_SKIP_PIXELS, size * 2);
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, (uint)ToInternalFormat(format), size, size, 0, (uint)format, GL_UNSIGNED_BYTE, bm.GetPixels());
			break;
		}
		case CubemapFileType::HorizontalCross: {
			size = bm.GetSize().x / 4;
			glPixelStorei(GL_UNPACK_ROW_LENGTH, bm.GetSize().x);

			glPixelStorei(GL_UNPACK_SKIP_ROWS, size);					

			glPixelStorei(GL_UNPACK_SKIP_PIXELS, 0);
			glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, (uint)ToInternalFormat(format), size, size, 0, (uint)format, GL_UNSIGNED_BYTE, bm.GetPixels());

			glPixelStorei(GL_UNPACK_SKIP_PIXELS, size * 2);
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, (uint)ToInternalFormat(format), size, size, 0, (uint)format, GL_UNSIGNED_BYTE, bm.GetPixels());

			glPixelStorei(GL_UNPACK_SKIP_PIXELS, size * 3);
			glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, (uint)ToInternalFormat(format), size, size, 0, (uint)format, GL_UNSIGNED_BYTE, bm.GetPixels());

			glPixelStorei(GL_UNPACK_SKIP_PIXELS, size);
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, (uint)ToInternalFormat(format), size, size, 0, (uint)format, GL_UNSIGNED_BYTE, bm.GetPixels());

			glPixelStorei(GL_UNPACK_SKIP_ROWS, 0);
			glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, (uint)ToInternalFormat(format), size, size, 0, (uint)format, GL_UNSIGNED_BYTE, bm.GetPixels());

			glPixelStorei(GL_UNPACK_SKIP_ROWS, size * 2);
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, (uint)ToInternalFormat(format), size, size, 0, (uint)format, GL_UNSIGNED_BYTE, bm.GetPixels());
			break;
		}
		case CubemapFileType::Column: {
			size = bm.GetSize().y / 6;
			glPixelStorei(GL_UNPACK_ROW_LENGTH, bm.GetSize().x);

			glPixelStorei(GL_UNPACK_SKIP_PIXELS, 0);

			glPixelStorei(GL_UNPACK_SKIP_ROWS, 0);
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, (uint)ToInternalFormat(format), size, size, 0, (uint)format, GL_UNSIGNED_BYTE, bm.GetPixels());

			glPixelStorei(GL_UNPACK_SKIP_ROWS, size);
			glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, (uint)ToInternalFormat(format), size, size, 0, (uint)format, GL_UNSIGNED_BYTE, bm.GetPixels());

			glPixelStorei(GL_UNPACK_SKIP_ROWS, size * 2);
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, (uint)ToInternalFormat(format), size, size, 0, (uint)format, GL_UNSIGNED_BYTE, bm.GetPixels());

			glPixelStorei(GL_UNPACK_SKIP_ROWS, size * 3);
			glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, (uint)ToInternalFormat(format), size, size, 0, (uint)format, GL_UNSIGNED_BYTE, bm.GetPixels());

			glPixelStorei(GL_UNPACK_SKIP_ROWS, size * 4);
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, (uint)ToInternalFormat(format), size, size, 0, (uint)format, GL_UNSIGNED_BYTE, bm.GetPixels());

			glPixelStorei(GL_UNPACK_SKIP_ROWS, size * 5);
			glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, (uint)ToInternalFormat(format), size, size, 0, (uint)format, GL_UNSIGNED_BYTE, bm.GetPixels());
			break;
		}
		case CubemapFileType::Row: {
			size = bm.GetSize().x / 6;
			glPixelStorei(GL_UNPACK_ROW_LENGTH, bm.GetSize().x);

			glPixelStorei(GL_UNPACK_SKIP_ROWS, 0);

			glPixelStorei(GL_UNPACK_SKIP_PIXELS, 0);
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, (uint)ToInternalFormat(format), size, size, 0, (uint)format, GL_UNSIGNED_BYTE, bm.GetPixels());

			glPixelStorei(GL_UNPACK_SKIP_PIXELS, size);
			glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, (uint)ToInternalFormat(format), size, size, 0, (uint)format, GL_UNSIGNED_BYTE, bm.GetPixels());

			glPixelStorei(GL_UNPACK_SKIP_PIXELS, size * 2);
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, (uint)ToInternalFormat(format), size, size, 0, (uint)format, GL_UNSIGNED_BYTE, bm.GetPixels());

			glPixelStorei(GL_UNPACK_SKIP_PIXELS, size * 3);
			glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, (uint)ToInternalFormat(format), size, size, 0, (uint)format, GL_UNSIGNED_BYTE, bm.GetPixels());

			glPixelStorei(GL_UNPACK_SKIP_PIXELS, size * 4);
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, (uint)ToInternalFormat(format), size, size, 0, (uint)format, GL_UNSIGNED_BYTE, bm.GetPixels());

			glPixelStorei(GL_UNPACK_SKIP_PIXELS, size * 5);
			glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, (uint)ToInternalFormat(format), size, size, 0, (uint)format, GL_UNSIGNED_BYTE, bm.GetPixels());
			break;
		}
		}

		glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
		glPixelStorei(GL_UNPACK_SKIP_PIXELS, 0);
		glPixelStorei(GL_UNPACK_SKIP_ROWS, 0);
	}
	void TextureCubemap::Load(const String& path, CubemapFace face)
	{		
		Bitmap bm;
		bm.Load(path);
		
		format = bm.GetFormat();
		size = bm.GetSize().x;

		Bind();		
		glTexImage2D((GLenum)face, 0, (uint)ToInternalFormat(format), size, size, 0, (uint)format, GL_UNSIGNED_BYTE, bm.GetPixels());
	}
	void TextureCubemap::Load(const String& xp, const String& xn, const String& yp, const String& yn, const String& zp, const String& zn)
	{
		Load(xp, CubemapFace::PositiveX);
		Load(xn, CubemapFace::NegativeX);
		Load(yp, CubemapFace::PositiveY);
		Load(yn, CubemapFace::NegativeY);
		Load(zp, CubemapFace::PositiveZ);
		Load(zn, CubemapFace::NegativeZ);
	}
	void TextureCubemap::Save(const String& path, CubemapFace face)
	{
		Bitmap bm;
		GetPixels(bm, face);
		bm.Save(path);
	}

	void TextureCubemap::GetPixels(Bitmap& bm, CubemapFace face)
	{
		if (bm.GetSize() != Vec2i(size, size) || bm.GetFormat() != format)
			bm.Create(size, format);

		glGetTexImage((GLenum)face, 0, (uint)format, GL_UNSIGNED_BYTE, bm.GetPixels());
	}

	TextureArray2D::TextureArray2D()
		: Texture(TextureType::Array2D), format(PixelFormat::RGBA), size(0)
	{ 
	}
	TextureArray2D::TextureArray2D(StringView path, Vec2i textureSize)
		: TextureArray2D()
	{
		Load(path, textureSize);
	}

	void TextureArray2D::Load(StringView path, Vec2i textureSize)
	{
		Bitmap bm;
		bm.Load(path);		

		format = bm.GetFormat();
		size.x = textureSize.x;
		size.y = textureSize.y;
		size.z = (bm.GetSize().x / size.x) * (bm.GetSize().y / size.y);

		glBindTexture(GL_TEXTURE_2D_ARRAY, id);
		glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, (uint)ToInternalFormat(format), size.x, size.y, size.z, 0, (uint)format, GL_UNSIGNED_BYTE, bm.GetPixels());
	}
	void TextureArray2D::Save(StringView path)
	{
		Bitmap bm;
		GetPixels(bm);
		bm.Save(path);
	}
	void TextureArray2D::GetPixels(Bitmap& bm)
	{
		if (bm.GetSize() != Vec2i(size.x, size.y * size.z) || bm.GetFormat() != format)
			bm.Create(Vec2i(size.x, size.y * size.z), format);

		glGetTexImage(GL_TEXTURE_2D_ARRAY, 0, GL_RGBA, GL_UNSIGNED_BYTE, bm.GetPixels());
	}
}