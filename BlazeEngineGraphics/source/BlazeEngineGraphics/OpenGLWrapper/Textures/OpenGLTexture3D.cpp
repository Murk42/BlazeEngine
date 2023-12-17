#include "pch.h"
#include "BlazeEngineGraphics\OpenGLWrapper/OpenGLContext.h"
#include "BlazeEngineGraphics\OpenGLWrapper/OpenGLConversions.h"

namespace Blaze::Graphics::OpenGLWrapper
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

	Result Texture3D::SetSettings(Texture3DSettings settings)
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
		//unsigned _min;
		//if (settings.mipmaps)
		//{
		//	glGenerateTextureMipmap(id);
		//
		//	if (settings.mip == TextureSampling::Nearest)
		//		if (settings.min == TextureSampling::Nearest)
		//			_min = GL_NEAREST_MIPMAP_NEAREST;
		//		else
		//			_min = GL_LINEAR_MIPMAP_NEAREST;
		//	else
		//		if (settings.min == TextureSampling::Nearest)
		//			_min = GL_NEAREST_MIPMAP_LINEAR;
		//		else
		//			_min = GL_LINEAR_MIPMAP_LINEAR;
		//}
		//else
		//	if (settings.min == TextureSampling::Nearest)
		//		_min = GL_NEAREST;
		//	else
		//		_min = GL_LINEAR;

		glTextureParameteri(id, GL_TEXTURE_WRAP_S, _xWrap);
		glTextureParameteri(id, GL_TEXTURE_WRAP_T, _yWrap);
		glTextureParameteri(id, GL_TEXTURE_MIN_FILTER, _min);
		glTextureParameteri(id, GL_TEXTURE_MAG_FILTER, _mag);

		return result;
	}

	Result Texture3D::Create(Vec3u size, TextureInternalPixelFormat internalFormat)
	{
		Result result;
		auto internalPixelFormat = OpenGLInternalPixelFormat(internalFormat, result);
		CHECK_RESULT(result);
		auto format = OpenGLFormatByInternalPixelFormat(internalFormat, result);
		CHECK_RESULT(result);

		this->size = size;		

		SelectTexture(this);
		glTexImage3D(GL_TEXTURE_3D, 0, internalPixelFormat, size.x, size.y, size.z, 0, format, GL_UNSIGNED_BYTE, nullptr);

		return Result();
	}

	void CalculateAlignmentAndStride(uint width, uint byteStride, uint pixelSize, uint& align, uint& pixelStride);

	Result Texture3D::SetPixels(Vec3u offset, Vec3u size, uint stride, BitmapColorFormat format, BitmapColorComponentType type, void* pixels)
	{
		Result result;

		GLenum _format = OpenGLPixelFormat(format, result);
		CHECK_RESULT(result);
		GLenum _type = OpenGLPixelType(type, result);
		CHECK_RESULT(result);

		auto pixelTypeSize = BitmapColorComponentTypeSize(type);		
		auto formatDepth = BitmapColorFormatComponentCount(format);		

		uint align;
		uint str;
		CalculateAlignmentAndStride(size.x, stride, pixelTypeSize * formatDepth, align, str);
		glPixelStorei(GL_UNPACK_ALIGNMENT, align);
		glPixelStorei(GL_UNPACK_ROW_LENGTH, str);
		glTextureSubImage3D(id, 0, offset.x, offset.y, offset.z, size.x, size.y, size.z, _format, _type, pixels);

		return result;
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