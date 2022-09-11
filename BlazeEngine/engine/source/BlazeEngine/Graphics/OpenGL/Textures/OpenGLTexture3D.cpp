#include "BlazeEngine/Graphics/OpenGL/Textures/OpenGLTexture3D.h"
#include "source/BlazeEngine/Internal/Conversions.h"

#include <GL/glew.h>

#include "BlazeEngine/Graphics/Renderer.h"

namespace Blaze::OpenGL
{
	Texture3D::Texture3D()
		: id(-1), size(0, 0, 0)
	{
		glGenTextures(1, &id);		
	}
	Texture3D::Texture3D(Texture3D&& tex) noexcept
		: id(tex.id), size(0, 0, 0)
	{
		tex.id = -1;
	}
	Texture3D::~Texture3D()
	{
		if (id != -1)
			glDeleteTextures(1, &id);
	}

	void Texture3D::SetSettings(Texture3DSettings settings)
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

	void Texture3D::Create(Vec3i size, TextureInternalPixelFormat internalFormat)
	{
		this->size = size;		

		Renderer::SelectTexture(this);
		glTexImage3D(GL_TEXTURE_3D, 0, OpenGLInternalPixelFormat(internalFormat), size.x, size.y, size.z, 0, OpenGLFormatByInternalPixelFormat(internalFormat), GL_UNSIGNED_BYTE, nullptr);
	}

	void CalculateAlignmentAndStride(uint width, uint byteStride, uint pixelSize, uint& align, uint& pixelStride);

	void Texture3D::SetPixels(Vec3i offset, Vec3i size, uint stride, BitmapPixelFormat format, BitmapPixelType type, void* pixels)
	{
		GLenum _format = OpenGLPixelFormat(format);
		GLenum _type = OpenGLPixelType(type);

		uint align;
		uint str;
		CalculateAlignmentAndStride(size.x, stride, PixelTypeSize(type) * GetFormatDepth(format), align, str);
		glPixelStorei(GL_UNPACK_ALIGNMENT, align);
		glPixelStorei(GL_UNPACK_ROW_LENGTH, str);
		glTextureSubImage3D(id, 0, offset.x, offset.y, offset.z, size.x, size.y, size.z, _format, _type, pixels);
	}

	Texture3D& Texture3D::operator=(Texture3D&& tex) noexcept
	{
		if (id != -1)
			glDeleteTextures(1, &id);
		id = tex.id;
		tex.id = -1;
		return *this;
	}
}