#include "pch.h"
#include "BlazeEngine/Core/Debug/Logger.h"
#include "BlazeEngine/Graphics/Core/OpenGL/OpenGLWrapper/OpenGLConversions.h"

namespace Blaze::Graphics::OpenGL
{

	template<typename T>
	inline std::underlying_type_t<T> ToInteger(T value)
	{
		return static_cast<std::underlying_type_t<T>>(value);
	}

	template<typename T>
	constexpr bool BitRemap(T in, GLenum& out, std::initializer_list<T> values, std::initializer_list<GLenum> enums)
	{
		out = 0;

		for (unsigned i = 0; i < values.size(); ++i)
		{
			if ((bool)(in & values.begin()[i]))
			{
				out |= enums.begin()[i];
				in ^= values.begin()[i];
			}
		}

		return !(bool)in;
	}

	GLenum OpenGLPixelFormat(BitmapColorFormat value, Result& result)
	{
		switch (value)
		{
		case BitmapColorFormat::Red: return GL_RED;
		case BitmapColorFormat::RG:	return GL_RG;
		case BitmapColorFormat::RGB: return GL_RGB;
		case BitmapColorFormat::RGBA: return GL_RGBA;
		case BitmapColorFormat::BGR: return GL_BGR;
		case BitmapColorFormat::BGRA: return GL_BGRA;
		}
		result += Debug::Log(Debug::LogType::Error, "Blaze Engine", Format("Invalid BitmapColorFormat enum value. The integer value was: {}", ToInteger(value)));
		return std::numeric_limits<GLenum>::max();
	}
	BitmapColorFormat OepnGLToBlazePixelFormat(GLenum value, Result& result)
	{
		switch (value)
		{
		case GL_LUMINANCE: return BitmapColorFormat::Red;
		case GL_LUMINANCE_ALPHA: return BitmapColorFormat::RG;
		case GL_RGB: return BitmapColorFormat::RGB;
		case GL_RGBA: return BitmapColorFormat::RGBA;
		case GL_BGR: return BitmapColorFormat::BGR;
		case GL_BGRA: return BitmapColorFormat::BGRA;
		}
		result += Debug::Log(Debug::LogType::Error, "Blaze Engine", Format("Unsupported/Invalid OpenGL pixel value enum value. The integer value was: {}", value));
		return std::numeric_limits<BitmapColorFormat>::max();
	}


	GLenum OpenGLFormatByInternalPixelFormat(TextureInternalPixelFormat value, Result& result)
	{
		switch (value)
		{
		case TextureInternalPixelFormat::RGBA:				 return GL_RGBA;
		case TextureInternalPixelFormat::RGB:				 return GL_RGB;
		case TextureInternalPixelFormat::RG:				     return GL_RG;
		case TextureInternalPixelFormat::R:				     return GL_RED;
		case TextureInternalPixelFormat::RGBA8:				 return GL_RGBA;
		case TextureInternalPixelFormat::RGB8:				 return GL_RGB;
		case TextureInternalPixelFormat::RG8:				 return GL_RG;
		case TextureInternalPixelFormat::R8:					 return GL_RED;
		case TextureInternalPixelFormat::RGBA16:				 return GL_RGBA;
		case TextureInternalPixelFormat::RGB16:				 return GL_RGB;
		case TextureInternalPixelFormat::RG16:				 return GL_RG;
		case TextureInternalPixelFormat::R16:				 return GL_RED;
		case TextureInternalPixelFormat::RGBA16F:			 return GL_RGBA;
		case TextureInternalPixelFormat::RGB16F:				 return GL_RGB;
		case TextureInternalPixelFormat::RG16F:				 return GL_RG;
		case TextureInternalPixelFormat::R16F:				 return GL_RED;
		case TextureInternalPixelFormat::RGBA32F:			 return GL_RGBA;
		case TextureInternalPixelFormat::RGB32F:				 return GL_RGB;
		case TextureInternalPixelFormat::RG32F:				 return GL_RG;
		case TextureInternalPixelFormat::R32F:				 return GL_RED;
		case TextureInternalPixelFormat::RGBA8I:				 return GL_RGBA;
		case TextureInternalPixelFormat::RGB8I:				 return GL_RGB;
		case TextureInternalPixelFormat::RG8I:				 return GL_RG;
		case TextureInternalPixelFormat::R8I:				 return GL_RED;
		case TextureInternalPixelFormat::RGBA16I:			 return GL_RGBA;
		case TextureInternalPixelFormat::RGB16I:				 return GL_RGB;
		case TextureInternalPixelFormat::RG16I:				 return GL_RG;
		case TextureInternalPixelFormat::R16I:				 return GL_RED;
		case TextureInternalPixelFormat::RGBA32I:			 return GL_RGBA;
		case TextureInternalPixelFormat::RGB32I:				 return GL_RGB;
		case TextureInternalPixelFormat::RG32I:				 return GL_RG;
		case TextureInternalPixelFormat::R32I:				 return GL_RED;
		case TextureInternalPixelFormat::RGBA8UI:			 return GL_RGBA;
		case TextureInternalPixelFormat::RGB8UI:				 return GL_RGB;
		case TextureInternalPixelFormat::RG8UI:				 return GL_RG;
		case TextureInternalPixelFormat::R8UI:				 return GL_RED;
		case TextureInternalPixelFormat::RGBA16UI:			 return GL_RGBA;
		case TextureInternalPixelFormat::RGB16UI:			 return GL_RGB;
		case TextureInternalPixelFormat::RG16UI:				 return GL_RG;
		case TextureInternalPixelFormat::R16UI:				 return GL_RED;
		case TextureInternalPixelFormat::RGBA32UI:			 return GL_RGBA;
		case TextureInternalPixelFormat::RGB32UI:			 return GL_RGB;
		case TextureInternalPixelFormat::RG32UI:				 return GL_RG;
		case TextureInternalPixelFormat::R32UI:				 return GL_RED;
		case TextureInternalPixelFormat::Depth16:			 return GL_DEPTH_COMPONENT;
		case TextureInternalPixelFormat::Depth24:			 return GL_DEPTH_COMPONENT;
		case TextureInternalPixelFormat::Depth32F:			 return GL_DEPTH_COMPONENT;
		case TextureInternalPixelFormat::Depth24Stencil8:	 return GL_DEPTH_STENCIL;
		case TextureInternalPixelFormat::Depth32FStencil8:	 return GL_DEPTH_STENCIL;
		case TextureInternalPixelFormat::Stencil8:			 return GL_STENCIL_INDEX;
		case TextureInternalPixelFormat::R11F_G11F_B10F:		 return GL_RGB;
		}
		result += Debug::Log(Debug::LogType::Error, "Blaze Engine", Format("Invalid TextureInternalPixelFormat enum value. The integer value was: {}", ToInteger(value)));
		return std::numeric_limits<GLenum>::max();
	}
	GLenum OpenGLInternalPixelFormat(TextureInternalPixelFormat value, Result& result)
	{
		switch (value)
		{
		case TextureInternalPixelFormat::RGBA:				 return GL_RGBA;
		case TextureInternalPixelFormat::RGB:				 return GL_RGB;
		case TextureInternalPixelFormat::RG:					 return GL_RG;
		case TextureInternalPixelFormat::R:					 return GL_RED;
		case TextureInternalPixelFormat::RGBA8:				 return GL_RGBA8;
		case TextureInternalPixelFormat::RGB8:				 return GL_RGB8;
		case TextureInternalPixelFormat::RG8:				 return GL_RG8;
		case TextureInternalPixelFormat::R8:					 return GL_R8;
		case TextureInternalPixelFormat::RGBA16:				 return GL_RGBA16;
		case TextureInternalPixelFormat::RGB16:				 return GL_RGB16;
		case TextureInternalPixelFormat::RG16:				 return GL_RG16;
		case TextureInternalPixelFormat::R16:				 return GL_R16;
		case TextureInternalPixelFormat::RGBA16F:			 return GL_RGBA16F;
		case TextureInternalPixelFormat::RGB16F:				 return GL_RGB16F;
		case TextureInternalPixelFormat::RG16F:				 return GL_RG16F;
		case TextureInternalPixelFormat::R16F:				 return GL_R16F;
		case TextureInternalPixelFormat::RGBA32F:			 return GL_RGBA32F;
		case TextureInternalPixelFormat::RGB32F:				 return GL_RGB32F;
		case TextureInternalPixelFormat::RG32F:				 return GL_RG32F;
		case TextureInternalPixelFormat::R32F:				 return GL_R32F;
		case TextureInternalPixelFormat::RGBA8I:				 return GL_RGBA8I;
		case TextureInternalPixelFormat::RGB8I:				 return GL_RGB8I;
		case TextureInternalPixelFormat::RG8I:				 return GL_RG8I;
		case TextureInternalPixelFormat::R8I:				 return GL_R8I;
		case TextureInternalPixelFormat::RGBA16I:			 return GL_RGBA16I;
		case TextureInternalPixelFormat::RGB16I:				 return GL_RGB16I;
		case TextureInternalPixelFormat::RG16I:				 return GL_RG16I;
		case TextureInternalPixelFormat::R16I:				 return GL_R16I;
		case TextureInternalPixelFormat::RGBA32I:			 return GL_RGBA32I;
		case TextureInternalPixelFormat::RGB32I:				 return GL_RGB32I;
		case TextureInternalPixelFormat::RG32I:				 return GL_RG32I;
		case TextureInternalPixelFormat::R32I:				 return GL_R32I;
		case TextureInternalPixelFormat::RGBA8UI:			 return GL_RGBA8UI;
		case TextureInternalPixelFormat::RGB8UI:				 return GL_RGB8UI;
		case TextureInternalPixelFormat::RG8UI:				 return GL_RG8UI;
		case TextureInternalPixelFormat::R8UI:				 return GL_R8UI;
		case TextureInternalPixelFormat::RGBA16UI:			 return GL_RGBA16UI;
		case TextureInternalPixelFormat::RGB16UI:			 return GL_RGB16UI;
		case TextureInternalPixelFormat::RG16UI:				 return GL_RG16UI;
		case TextureInternalPixelFormat::R16UI:				 return GL_R16UI;
		case TextureInternalPixelFormat::RGBA32UI:			 return GL_RGBA32UI;
		case TextureInternalPixelFormat::RGB32UI:			 return GL_RGB32UI;
		case TextureInternalPixelFormat::RG32UI:				 return GL_RG32UI;
		case TextureInternalPixelFormat::R32UI:				 return GL_R32UI;
		case TextureInternalPixelFormat::SRGB24:             return GL_SRGB8;
		case TextureInternalPixelFormat::SRGBA32:            return GL_SRGB8_ALPHA8;
		case TextureInternalPixelFormat::Depth16:			 return GL_DEPTH_COMPONENT16;
		case TextureInternalPixelFormat::Depth24:			 return GL_DEPTH_COMPONENT24;
		case TextureInternalPixelFormat::Depth32F:			 return GL_DEPTH_COMPONENT32F;
		case TextureInternalPixelFormat::Depth24Stencil8:	 return GL_DEPTH24_STENCIL8;
		case TextureInternalPixelFormat::Depth32FStencil8:	 return GL_DEPTH32F_STENCIL8;
		case TextureInternalPixelFormat::Stencil8:			 return GL_STENCIL_INDEX;
		case TextureInternalPixelFormat::R11F_G11F_B10F:		 return GL_R11F_G11F_B10F;
		}
		result += Debug::Log(Debug::LogType::Error, "Blaze Engine", Format("Invalid TextureInternalPixelFormat enum value. The integer value was: {}", ToInteger(value)));
		return std::numeric_limits<GLenum>::max();
	}
	GLenum OpenGLBufferInternalPixelFormat(TextureBufferInternalPixelFormat value, Result& result)
	{
		switch (value)
		{
		case TextureBufferInternalPixelFormat::R8: return GL_R8;
		case TextureBufferInternalPixelFormat::R16: return GL_R16;
		case TextureBufferInternalPixelFormat::R16F: return GL_R16F;
		case TextureBufferInternalPixelFormat::R32F: return GL_R32F;
		case TextureBufferInternalPixelFormat::R8I: return GL_R8I;
		case TextureBufferInternalPixelFormat::R16I: return GL_R16I;
		case TextureBufferInternalPixelFormat::R32I: return GL_R32I;
		case TextureBufferInternalPixelFormat::R8UI: return GL_R8UI;
		case TextureBufferInternalPixelFormat::R16UI: return GL_R16UI;
		case TextureBufferInternalPixelFormat::R32UI: return GL_R32UI;
		case TextureBufferInternalPixelFormat::RG8: return GL_RG8;
		case TextureBufferInternalPixelFormat::RG16: return GL_RG16;
		case TextureBufferInternalPixelFormat::RG16F: return GL_RG16F;
		case TextureBufferInternalPixelFormat::RG32F: return GL_RG32F;
		case TextureBufferInternalPixelFormat::RG8I: return GL_RG8I;
		case TextureBufferInternalPixelFormat::RG16I: return GL_RG16I;
		case TextureBufferInternalPixelFormat::RG32I: return GL_RG32I;
		case TextureBufferInternalPixelFormat::RG8UI: return GL_RG8UI;
		case TextureBufferInternalPixelFormat::RG16UI: return GL_RG16UI;
		case TextureBufferInternalPixelFormat::RG32UI: return GL_RG32UI;
		case TextureBufferInternalPixelFormat::RGB32F: return GL_RGB32F;
		case TextureBufferInternalPixelFormat::RGB32I: return GL_RGB32I;
		case TextureBufferInternalPixelFormat::RGB32UI: return GL_RGB32UI;
		case TextureBufferInternalPixelFormat::RGBA8: return GL_RGBA8;
		case TextureBufferInternalPixelFormat::RGBA16: return GL_RGBA16;
		case TextureBufferInternalPixelFormat::RGBA16F: return GL_RGBA16F;
		case TextureBufferInternalPixelFormat::RGBA32F: return GL_RGBA32F;
		case TextureBufferInternalPixelFormat::RGBA8I: return GL_RGBA8I;
		case TextureBufferInternalPixelFormat::RGBA16I: return GL_RGBA16I;
		case TextureBufferInternalPixelFormat::RGBA32I: return GL_RGBA32I;
		case TextureBufferInternalPixelFormat::RGBA8UI: return GL_RGBA8UI;
		case TextureBufferInternalPixelFormat::RGBA16UI: return GL_RGBA16UI;
		case TextureBufferInternalPixelFormat::RGBA32UI: return GL_RGBA32UI;
		}
		result += Debug::Log(Debug::LogType::Error, "Blaze Engine", Format("Invalid TextureBufferInternalPixelFormat enum value. The integer value was: {}", ToInteger(value)));
		return std::numeric_limits<GLenum>::max();
	}

	GLenum OpenGLPixelType(BitmapColorComponentType value, Result& result)
	{
		switch (value)
		{
		case BitmapColorComponentType::Int8: return GL_BYTE;
		case BitmapColorComponentType::Uint8:	return GL_UNSIGNED_BYTE;
		case BitmapColorComponentType::Int16:	return GL_SHORT;
		case BitmapColorComponentType::Uint16: return GL_UNSIGNED_SHORT;
		case BitmapColorComponentType::Int32:	return GL_INT;
		case BitmapColorComponentType::Uint32: return GL_UNSIGNED_INT;
		case BitmapColorComponentType::Float:	return GL_FLOAT;
		case BitmapColorComponentType::Double: return GL_DOUBLE;
		}

		result += Debug::Log(Debug::LogType::Error, "Blaze Engine", Format("Invalid BitmapColorComponentType enum value. The integer value was: {}", ToInteger(value)));
		return std::numeric_limits<GLenum>::max();
	}

	BitmapColorComponentType OpenGLToBlazePixelType(GLenum value, Result& result)
	{
		switch (value)
		{
		case GL_BYTE: return BitmapColorComponentType::Int8;
		case GL_UNSIGNED_BYTE: return BitmapColorComponentType::Uint8;
		case GL_SHORT: return BitmapColorComponentType::Int16;
		case GL_UNSIGNED_SHORT: return BitmapColorComponentType::Uint16;
		case GL_INT: return BitmapColorComponentType::Int32;
		case GL_UNSIGNED_INT: return BitmapColorComponentType::Uint32;
		case GL_FLOAT: return BitmapColorComponentType::Float;
		case GL_DOUBLE: return BitmapColorComponentType::Double;
		}
		result += Debug::Log(Debug::LogType::Error, "Blaze Engine", Format("Unsupported/Invalid OpenGL pixel value enum value. The integer value was: {}", value));
		return std::numeric_limits<BitmapColorComponentType>::max();
	}
	GLenum OpenGLTextureMinSampling(TextureSampling min, TextureSampling mip, bool mipmaps, Result& result)
	{
		if (mipmaps)
		{
			if (mip == TextureSampling::Nearest)
				if (min == TextureSampling::Nearest)
					return GL_NEAREST_MIPMAP_NEAREST;
				else if (min == TextureSampling::Linear)
					return GL_LINEAR_MIPMAP_NEAREST;
				else
				{
					result += Debug::Log(Debug::LogType::Error, "Blaze Engine", Format("Invalid TextureSampling enum value. The integer value was: {}", ToInteger(min)));
					return std::numeric_limits<GLenum>::max();
				}
			else if (mip == TextureSampling::Linear)
				if (min == TextureSampling::Nearest)
					return GL_NEAREST_MIPMAP_LINEAR;
				else if (min == TextureSampling::Linear)
					return GL_LINEAR_MIPMAP_LINEAR;
				else
				{
					result += Debug::Log(Debug::LogType::Error, "Blaze Engine", Format("Invalid TextureSampling enum value. The integer value was: {}", ToInteger(min)));
					return std::numeric_limits<GLenum>::max();
				}
			else
			{
				result += Debug::Log(Debug::LogType::Error, "Blaze Engine", Format("Invalid TextureSampling enum value. The integer value was: {}", ToInteger(mip)));
				return std::numeric_limits<GLenum>::max();
			}
		}
		else
			if (min == TextureSampling::Nearest)
				return GL_NEAREST;
			else if (min == TextureSampling::Linear)
				return GL_LINEAR;
			else
			{
				result += Debug::Log(Debug::LogType::Error, "Blaze Engine", Format("Invalid TextureSampling enum value. The integer value was: {}", ToInteger(min)));
				return std::numeric_limits<GLenum>::max();
			}
	}
	GLenum OpenGLTextureMagSampling(TextureSampling sampling, Result& result)
	{
		switch (sampling)
		{
		case TextureSampling::Nearest: return GL_NEAREST;
		case TextureSampling::Linear: return GL_LINEAR;
		}
		result += Debug::Log(Debug::LogType::Error, "Blaze Engine", Format("Invalid TextureSampling enum value. The integer value was: {}", ToInteger(sampling)));
		return std::numeric_limits<GLenum>::max();
	}
	GLenum OpenGLTextureWrapping(TextureWrapping wrapping, Result& result)
	{
		switch (wrapping)
		{
		case TextureWrapping::ClampToBorder: return GL_CLAMP_TO_BORDER;
		case TextureWrapping::ClampToEdge: return GL_CLAMP_TO_EDGE;
		case TextureWrapping::MirroredRepeat: return GL_MIRRORED_REPEAT;
		case TextureWrapping::Repeat: return GL_REPEAT;
		}
		result += Debug::Log(Debug::LogType::Error, "Blaze Engine", Format("Invalid TextureWrapping enum value. The integer value was: {}", ToInteger(wrapping)));
		return std::numeric_limits<GLenum>::max();
	}
	TextureInternalPixelFormat MapInternalTexturePixelFormat(BitmapColorFormat value, Result& result)
	{
		switch (value)
		{
		case BitmapColorFormat::Red: return TextureInternalPixelFormat::R32F;
		case BitmapColorFormat::RG: return TextureInternalPixelFormat::RG32F;
		case BitmapColorFormat::RGB: return TextureInternalPixelFormat::RGB32F;
		case BitmapColorFormat::RGBA: return TextureInternalPixelFormat::RGBA32F;
		case BitmapColorFormat::BGR: return TextureInternalPixelFormat::RGB32F;
		case BitmapColorFormat::BGRA: return TextureInternalPixelFormat::RGBA32F;
		}
		result += Debug::Log(Debug::LogType::Error, "Blaze Engine", Format("Invalid BitmapColorFormat enum value. The integer value was: {}", ToInteger(value)));
		return std::numeric_limits<TextureInternalPixelFormat>::max();
	}
	BitmapColorFormat MapInternalTexturePixelFormat(TextureInternalPixelFormat value, Result& result)
	{
		switch (value)
		{
		case TextureInternalPixelFormat::RGBA: return BitmapColorFormat::RGBA;
		case TextureInternalPixelFormat::RGB:  return BitmapColorFormat::RGB;
		case TextureInternalPixelFormat::RG:   return BitmapColorFormat::RG;
		case TextureInternalPixelFormat::R:    return BitmapColorFormat::Red;
		case TextureInternalPixelFormat::RGBA8:  return BitmapColorFormat::RGBA;
		case TextureInternalPixelFormat::RGB8:	return BitmapColorFormat::RGB;
		case TextureInternalPixelFormat::RG8:	return BitmapColorFormat::RG;
		case TextureInternalPixelFormat::R8:		return BitmapColorFormat::Red;
		case TextureInternalPixelFormat::RGBA16:	return BitmapColorFormat::RGBA;
		case TextureInternalPixelFormat::RGB16:	return BitmapColorFormat::RGB;
		case TextureInternalPixelFormat::RG16:	return BitmapColorFormat::RG;
		case TextureInternalPixelFormat::R16:	return BitmapColorFormat::Red;
		case TextureInternalPixelFormat::RGBA16F:return BitmapColorFormat::RGBA;
		case TextureInternalPixelFormat::RGB16F:	return BitmapColorFormat::RGB;
		case TextureInternalPixelFormat::RG16F:	return BitmapColorFormat::RG;
		case TextureInternalPixelFormat::R16F:	return BitmapColorFormat::Red;
		case TextureInternalPixelFormat::RGBA32F:return BitmapColorFormat::RGBA;
		case TextureInternalPixelFormat::RGB32F:	return BitmapColorFormat::RGB;
		case TextureInternalPixelFormat::RG32F:	return BitmapColorFormat::RG;
		case TextureInternalPixelFormat::R32F:	return BitmapColorFormat::Red;
		case TextureInternalPixelFormat::RGBA8I:	return BitmapColorFormat::RGBA;
		case TextureInternalPixelFormat::RGB8I:	return BitmapColorFormat::RGB;
		case TextureInternalPixelFormat::RG8I:	return BitmapColorFormat::RG;
		case TextureInternalPixelFormat::R8I:	return BitmapColorFormat::Red;
		case TextureInternalPixelFormat::RGBA16I:return BitmapColorFormat::RGBA;
		case TextureInternalPixelFormat::RGB16I:	return BitmapColorFormat::RGB;
		case TextureInternalPixelFormat::RG16I:	return BitmapColorFormat::RG;
		case TextureInternalPixelFormat::R16I:	return BitmapColorFormat::Red;
		case TextureInternalPixelFormat::RGBA32I:return BitmapColorFormat::RGBA;
		case TextureInternalPixelFormat::RGB32I:	return BitmapColorFormat::RGB;
		case TextureInternalPixelFormat::RG32I:	return BitmapColorFormat::RG;
		case TextureInternalPixelFormat::R32I:	return BitmapColorFormat::Red;
		case TextureInternalPixelFormat::RGBA8UI:return BitmapColorFormat::RGBA;
		case TextureInternalPixelFormat::RGB8UI:	return BitmapColorFormat::RGB;
		case TextureInternalPixelFormat::RG8UI:	return BitmapColorFormat::RG;
		case TextureInternalPixelFormat::R8UI:	return BitmapColorFormat::Red;
		case TextureInternalPixelFormat::RGBA16UI:return BitmapColorFormat::RGBA;
		case TextureInternalPixelFormat::RGB16UI:return BitmapColorFormat::RGB;
		case TextureInternalPixelFormat::RG16UI:	return BitmapColorFormat::RG;
		case TextureInternalPixelFormat::R16UI:	 return BitmapColorFormat::Red;
		case TextureInternalPixelFormat::RGBA32UI:return BitmapColorFormat::RGBA;
		case TextureInternalPixelFormat::RGB32UI:return BitmapColorFormat::RGB;
		case TextureInternalPixelFormat::RG32UI:	return BitmapColorFormat::RG;
		case TextureInternalPixelFormat::R32UI:	return BitmapColorFormat::Red;
		case TextureInternalPixelFormat::Depth16:
		case TextureInternalPixelFormat::Depth24:
		case TextureInternalPixelFormat::Depth32F:
		case TextureInternalPixelFormat::Depth24Stencil8:
		case TextureInternalPixelFormat::Depth32FStencil8:
		case TextureInternalPixelFormat::Stencil8:
			BLAZE_LOG_ERROR("Unsupported TextureInternalPixelFormat enum value");
			return BitmapColorFormat::Red;
		case TextureInternalPixelFormat::R11F_G11F_B10F:	return BitmapColorFormat::RGB;
		default:
			BLAZE_LOG_ERROR("Invalid TextureInternalPixelFormat enum value");
			return BitmapColorFormat::Red;
			break;
		}
	}
	GLenum ConvertToOpenGLEnum(IntegerVertexAttributeType value)
	{
		switch (value)
		{
		case Blaze::Graphics::OpenGL::IntegerVertexAttributeType::Int8: return GL_BYTE;
		case Blaze::Graphics::OpenGL::IntegerVertexAttributeType::Int16: return GL_SHORT;
		case Blaze::Graphics::OpenGL::IntegerVertexAttributeType::Int32: return GL_INT;
		case Blaze::Graphics::OpenGL::IntegerVertexAttributeType::Uint8: return GL_UNSIGNED_BYTE;
		case Blaze::Graphics::OpenGL::IntegerVertexAttributeType::Uint16: return GL_UNSIGNED_SHORT;
		case Blaze::Graphics::OpenGL::IntegerVertexAttributeType::Uint32: return GL_UNSIGNED_INT;
		default:
			Debug::Logger::LogError("Blaze Engine Graphics", "Invalid OpenGL integer vertex attribute value enum value.");
			return 0;
		}
	}
	GLenum ConvertToOpenGLEnum(FloatVertexAttributeType value)
	{
		switch (value)
		{
		case Blaze::Graphics::OpenGL::FloatVertexAttributeType::Int8: return GL_BYTE;
		case Blaze::Graphics::OpenGL::FloatVertexAttributeType::Int16: return GL_SHORT;
		case Blaze::Graphics::OpenGL::FloatVertexAttributeType::Int32: return GL_INT;
		case Blaze::Graphics::OpenGL::FloatVertexAttributeType::Uint8: return GL_UNSIGNED_BYTE;
		case Blaze::Graphics::OpenGL::FloatVertexAttributeType::Uint16: return GL_UNSIGNED_SHORT;
		case Blaze::Graphics::OpenGL::FloatVertexAttributeType::Uint32: return GL_UNSIGNED_INT;
		case Blaze::Graphics::OpenGL::FloatVertexAttributeType::Half: return GL_HALF_FLOAT;
		case Blaze::Graphics::OpenGL::FloatVertexAttributeType::Float: return GL_FLOAT;
		case Blaze::Graphics::OpenGL::FloatVertexAttributeType::Double: return GL_DOUBLE;
		default:
			Debug::Logger::LogError("Blaze Engine Graphics", "Invalid OpenGL float vertex attribute value enum value.");
			return 0;
		}
	}
	GLenum ConvertToOpenGLEnum(PackedVertexAttributeType value)
	{
		switch (value)
		{
		case Blaze::Graphics::OpenGL::PackedVertexAttributeType::Int_2_10_10_10_Rev: return GL_INT_2_10_10_10_REV;
		case Blaze::Graphics::OpenGL::PackedVertexAttributeType::Uint_2_10_10_10_Rev: return GL_UNSIGNED_INT_2_10_10_10_REV;
		case Blaze::Graphics::OpenGL::PackedVertexAttributeType::Uint_10F_11F_11F_Rev: return GL_UNSIGNED_INT_10F_11F_11F_REV;
		default:
			Debug::Logger::LogError("Blaze Engine Graphics", "Invalid OpenGL packed vertex attribute value enum value.");
			return 0;
		}
	}
	GLenum ConvertToOpenGLEnum(BGRAVertexAttributeType value)
	{
		switch (value)
		{
		case Blaze::Graphics::OpenGL::BGRAVertexAttributeType::Uint8: return GL_UNSIGNED_BYTE;
		case Blaze::Graphics::OpenGL::BGRAVertexAttributeType::Int_2_10_10_10_Rev: return GL_INT_2_10_10_10_REV;
		case Blaze::Graphics::OpenGL::BGRAVertexAttributeType::Uint_2_10_10_10_Rev: return GL_UNSIGNED_INT_2_10_10_10_REV;
		default:
			Debug::Logger::LogError("Blaze Engine Graphics", "Invalid OpenGL BGRA vertex attribute value enum value.");
			return 0;
		}
	}
	GLenum OpenGLFramebufferAttachment(FramebufferAttachment value, Result& result)
	{
		switch (value)
		{
		case FramebufferAttachment::DepthStencil: return GL_DEPTH_STENCIL_ATTACHMENT;
		case FramebufferAttachment::Stencil:	return GL_STENCIL_ATTACHMENT;
		case FramebufferAttachment::Depth: return GL_DEPTH_ATTACHMENT;
		}
		result += Debug::Log(Debug::LogType::Error, "Blaze Engine", Format("Invalid FramebufferAttachment enum value. The integer value was: {}", ToInteger(value)));
		return std::numeric_limits<GLenum>::max();
	}

	FramebufferStatus MapFramebufferStatus(GLenum value, Result& result)
	{
		switch (value)
		{
		case GL_FRAMEBUFFER_COMPLETE: return FramebufferStatus::Complete;
		case GL_FRAMEBUFFER_UNDEFINED: return FramebufferStatus::Undefined;
		case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT: return FramebufferStatus::IncompleteAttachment;
		case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT: return FramebufferStatus::MissingAttachment;
		case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER: return FramebufferStatus::IncompleteDrawBuffer;
		case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER: return FramebufferStatus::IncompleteReadBuffer;
		case GL_FRAMEBUFFER_UNSUPPORTED:return FramebufferStatus::Unsupported;
		case GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE: return FramebufferStatus::IncompleteMultisample;
		case GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS: return FramebufferStatus::IncompleteLayerTargets;
		}

		result += Debug::Log(Debug::LogType::Error, "Blaze Engine", Format("Invalid framebuffer value enum value. The integer value was: {}", value));
		return std::numeric_limits<FramebufferStatus>::max();
	}

	GLenum OpenGLBufferMappingAccess(GraphicsBufferMapAccessFlags value, Result& result)
	{
		GLenum out;

		if (!BitRemap(value, out, {
				GraphicsBufferMapAccessFlags::Read,
				GraphicsBufferMapAccessFlags::Write
			}, {
				GL_MAP_READ_BIT,
				GL_MAP_WRITE_BIT
			}))
		{
			result += Debug::Log(Debug::LogType::Error, "Blaze Engine", Format("Invalid GraphicsBufferMapAccessFlags enum value. The integer value was: {}", ToInteger(value)));
			return std::numeric_limits<GLenum>::max();
		}

		return out;
	}
	GLenum OpenGLBufferMappingType(GraphicsBufferMapType value, Result& result)
	{
		GLenum out;

		if (!BitRemap(value, out, {
				GraphicsBufferMapType::PersistentCoherent,
				GraphicsBufferMapType::PersistentUncoherent
			}, {
				GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT,
				GL_MAP_PERSISTENT_BIT
			}))
		{
			result += Debug::Log(Debug::LogType::Error, "Blaze Engine", Format("Invalid GraphicsBufferMapType enum value. The integer value was: {}", ToInteger(value)));
			return std::numeric_limits<GLenum>::max();
		}

		return out;
	}
	GLenum OpenGLBufferMappingOptions(GraphicsBufferMapOptions value, Result& result)
	{
		GLenum out;

		if (!BitRemap(value, out, {
				GraphicsBufferMapOptions::ExplicitFlush,
				GraphicsBufferMapOptions::InvalidateBuffer,
				GraphicsBufferMapOptions::InvalidateRange,
				GraphicsBufferMapOptions::Unsynchronized,
			}, {
				GL_MAP_FLUSH_EXPLICIT_BIT,
				GL_MAP_INVALIDATE_BUFFER_BIT,
				GL_MAP_INVALIDATE_RANGE_BIT,
				GL_MAP_UNSYNCHRONIZED_BIT
			}))
		{
			result += Debug::Log(Debug::LogType::Error, "Blaze Engine", Format("Invalid GraphicsBufferMapOptions enum value. The integer value was: {}", static_cast<uint>(value)));
			return std::numeric_limits<GLenum>::max();
		}

		return out;
	}

	UniformType MapOpenGLUniformType(GLenum value)
	{
		switch (value)
		{
		case GL_FLOAT: return UniformType::Float;
		case GL_FLOAT_VEC2: return UniformType::Vec2f;
		case GL_FLOAT_VEC3: return UniformType::Vec3f;
		case GL_FLOAT_VEC4: return UniformType::Vec4f;
		case GL_DOUBLE: return UniformType::Double;
		case GL_DOUBLE_VEC2: return UniformType::Vec2d;
		case GL_DOUBLE_VEC3: return UniformType::Vec3d;
		case GL_DOUBLE_VEC4: return UniformType::Vec4d;
		case GL_INT: return UniformType::Int;
		case GL_INT_VEC2: return UniformType::Vec2i;
		case GL_INT_VEC3: return UniformType::Vec3i;
		case GL_INT_VEC4: return UniformType::Vec4i;
		case GL_UNSIGNED_INT: return UniformType::UnsignedInt;
		case GL_UNSIGNED_INT_VEC2: return UniformType::Vec2u;
		case GL_UNSIGNED_INT_VEC3: return UniformType::Vec3u;
		case GL_UNSIGNED_INT_VEC4: return UniformType::Vec4u;
		case GL_BOOL: return UniformType::Bool;
		case GL_BOOL_VEC2: return UniformType::BoolVec2;
		case GL_BOOL_VEC3: return UniformType::BoolVec3;
		case GL_BOOL_VEC4: return UniformType::BoolVec4;
		case GL_FLOAT_MAT2: return UniformType::Mat2f;
		case GL_FLOAT_MAT3: return UniformType::Mat3f;
		case GL_FLOAT_MAT4: return UniformType::Mat4f;
		case GL_FLOAT_MAT2x3: return UniformType::Mat2x3f;
		case GL_FLOAT_MAT2x4: return UniformType::Mat2x4f;
		case GL_FLOAT_MAT3x2: return UniformType::Mat3x2f;
		case GL_FLOAT_MAT3x4: return UniformType::Mat3x4f;
		case GL_FLOAT_MAT4x2: return UniformType::Mat4x2f;
		case GL_FLOAT_MAT4x3: return UniformType::Mat4x3f;
		case GL_DOUBLE_MAT2: return UniformType::Mat2d;
		case GL_DOUBLE_MAT3: return UniformType::Mat3d;
		case GL_DOUBLE_MAT4: return UniformType::Mat4d;
		case GL_DOUBLE_MAT2x3: return UniformType::Mat2x3d;
		case GL_DOUBLE_MAT2x4: return UniformType::Mat2x4d;
		case GL_DOUBLE_MAT3x2: return UniformType::Mat3x2d;
		case GL_DOUBLE_MAT3x4: return UniformType::Mat3x4d;
		case GL_DOUBLE_MAT4x2: return UniformType::Mat4x2d;
		case GL_DOUBLE_MAT4x3: return UniformType::Mat4x3d;
		case GL_SAMPLER_1D: return UniformType::Sampler1D;
		case GL_SAMPLER_2D: return UniformType::Sampler2D;
		case GL_SAMPLER_3D: return UniformType::Sampler3D;
		case GL_SAMPLER_CUBE: return UniformType::SamplerCube;
		case GL_SAMPLER_1D_SHADOW: return UniformType::Sampler1DShadow;
		case GL_SAMPLER_2D_SHADOW: return UniformType::Sampler2DShadow;
		case GL_SAMPLER_1D_ARRAY: return UniformType::Sampler1DArray;
		case GL_SAMPLER_2D_ARRAY: return UniformType::Sampler2DArray;
		case GL_SAMPLER_1D_ARRAY_SHADOW: return UniformType::Sampler1DArrayShadow;
		case GL_SAMPLER_2D_ARRAY_SHADOW: return UniformType::Sampler2DArrayShadow;
		case GL_SAMPLER_2D_MULTISAMPLE: return UniformType::Sampler2DMultisample;
		case GL_SAMPLER_2D_MULTISAMPLE_ARRAY: return UniformType::Sampler2DMultisampleArray;
		case GL_SAMPLER_CUBE_SHADOW: return UniformType::SamplerCubeShadow;
		case GL_SAMPLER_BUFFER: return UniformType::SamplerBuffer;
		case GL_SAMPLER_2D_RECT: return UniformType::Sampler2DRect;
		case GL_SAMPLER_2D_RECT_SHADOW: return UniformType::Sampler2DRectShadow;
		case GL_INT_SAMPLER_1D: return UniformType::IntSampler1D;
		case GL_INT_SAMPLER_2D: return UniformType::IntSampler2D;
		case GL_INT_SAMPLER_3D: return UniformType::IntSampler3D;
		case GL_INT_SAMPLER_CUBE: return UniformType::IntSamplerCube;
		case GL_INT_SAMPLER_1D_ARRAY: return UniformType::IntSampler1DArray;
		case GL_INT_SAMPLER_2D_ARRAY: return UniformType::IntSampler2DArray;
		case GL_INT_SAMPLER_2D_MULTISAMPLE: return UniformType::IntSampler2DMultisample;
		case GL_INT_SAMPLER_2D_MULTISAMPLE_ARRAY: return UniformType::IntSampler2DMultisampleArray;
		case GL_INT_SAMPLER_BUFFER: return UniformType::IntSamplerBuffer;
		case GL_INT_SAMPLER_2D_RECT: return UniformType::IntSampler2DRect;
		case GL_UNSIGNED_INT_SAMPLER_1D: return UniformType::UnsignedIntSampler1D;
		case GL_UNSIGNED_INT_SAMPLER_2D: return UniformType::UnsignedIntSampler2D;
		case GL_UNSIGNED_INT_SAMPLER_3D: return UniformType::UnsignedIntSampler3D;
		case GL_UNSIGNED_INT_SAMPLER_CUBE: return UniformType::UnsignedIntSamplerCube;
		case GL_UNSIGNED_INT_SAMPLER_1D_ARRAY: return UniformType::UnsignedIntSampler1DArray;
		case GL_UNSIGNED_INT_SAMPLER_2D_ARRAY: return UniformType::UnsignedIntSampler2DArray;
		case GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE: return UniformType::UnsignedIntSampler2DMultisample;
		case GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE_ARRAY: return UniformType::UnsignedIntSampler2DMultisampleArray;
		case GL_UNSIGNED_INT_SAMPLER_BUFFER: return UniformType::UnsignedIntSamplerBuffer;
		case GL_UNSIGNED_INT_SAMPLER_2D_RECT: return UniformType::UnsignedIntSampler2DRect;
		case GL_IMAGE_1D: return UniformType::Image1D;
		case GL_IMAGE_2D: return UniformType::Image2D;
		case GL_IMAGE_3D: return UniformType::Image3D;
		case GL_IMAGE_2D_RECT: return UniformType::Image2DRect;
		case GL_IMAGE_CUBE: return UniformType::ImageCube;
		case GL_IMAGE_BUFFER: return UniformType::ImageBuffer;
		case GL_IMAGE_1D_ARRAY: return UniformType::Image1DArray;
		case GL_IMAGE_2D_ARRAY: return UniformType::Image2DArray;
		case GL_IMAGE_2D_MULTISAMPLE: return UniformType::Image2DMultisample;
		case GL_IMAGE_2D_MULTISAMPLE_ARRAY: return UniformType::Image2DMultisampleArray;
		case GL_INT_IMAGE_1D: return UniformType::IntImage1D;
		case GL_INT_IMAGE_2D: return UniformType::IntImage2D;
		case GL_INT_IMAGE_3D: return UniformType::IntImage3D;
		case GL_INT_IMAGE_2D_RECT: return UniformType::IntImage2DRect;
		case GL_INT_IMAGE_CUBE: return UniformType::IntImageCube;
		case GL_INT_IMAGE_BUFFER: return UniformType::IntImageBuffer;
		case GL_INT_IMAGE_1D_ARRAY: return UniformType::IntImage1DArray;
		case GL_INT_IMAGE_2D_ARRAY: return UniformType::IntImage2DArray;
		case GL_INT_IMAGE_2D_MULTISAMPLE: return UniformType::IntImage2DMultisample;
		case GL_INT_IMAGE_2D_MULTISAMPLE_ARRAY: return UniformType::IntImage2DMultisampleArray;
		case GL_UNSIGNED_INT_IMAGE_1D: return UniformType::UnsignedIntImage1D;
		case GL_UNSIGNED_INT_IMAGE_2D: return UniformType::UnsignedIntImage2D;
		case GL_UNSIGNED_INT_IMAGE_3D: return UniformType::UnsignedIntImage3D;
		case GL_UNSIGNED_INT_IMAGE_2D_RECT: return UniformType::UnsignedIntImage2DRect;
		case GL_UNSIGNED_INT_IMAGE_CUBE: return UniformType::UnsignedIntImageCube;
		case GL_UNSIGNED_INT_IMAGE_BUFFER: return UniformType::UnsignedIntImageBuffer;
		case GL_UNSIGNED_INT_IMAGE_1D_ARRAY: return UniformType::UnsignedIntImage1DArray;
		case GL_UNSIGNED_INT_IMAGE_2D_ARRAY: return UniformType::UnsignedIntImage2DArray;
		case GL_UNSIGNED_INT_IMAGE_2D_MULTISAMPLE: return UniformType::UnsignedIntImage2DMultisample;
		case GL_UNSIGNED_INT_IMAGE_2D_MULTISAMPLE_ARRAY: return UniformType::UnsignedIntImage2DMultisampleArray;
		case GL_UNSIGNED_INT_ATOMIC_COUNTER: return UniformType::UnsignedIntAtomicCounter;
		default:
			Debug::Logger::LogError("Blaze Engine Graphics", "Invalid OpenGL uniform value value enum value.");
			return UniformType::Invalid;
		}
	}

	GLenum OpenGLMutableBufferUsage(GraphicsBufferUseFrequency value, GraphicsBufferUseType use, Result& result)
	{
		switch (value)
		{
		case GraphicsBufferUseFrequency::Static:	switch (use) {
		case GraphicsBufferUseType::Draw: return GL_STATIC_DRAW;
		case GraphicsBufferUseType::Read: return GL_STATIC_READ;
		case GraphicsBufferUseType::Copy: return GL_STATIC_COPY;
		}
		case GraphicsBufferUseFrequency::Dynamic:	switch (use) {
		case GraphicsBufferUseType::Draw: return GL_DYNAMIC_DRAW;
		case GraphicsBufferUseType::Read: return GL_DYNAMIC_READ;
		case GraphicsBufferUseType::Copy: return GL_DYNAMIC_COPY;
		}
		case GraphicsBufferUseFrequency::Stream:	switch (use) {
		case GraphicsBufferUseType::Draw: return GL_STREAM_DRAW;
		case GraphicsBufferUseType::Read: return GL_STREAM_READ;
		case GraphicsBufferUseType::Copy: return GL_STREAM_COPY;
		}
		default:
			result += Debug::Log(Debug::LogType::Error, "Blaze Engine", Format("Invalid GraphicsBufferUseFrequency enum value. The integer value was: {}", ToInteger(value)));
			return std::numeric_limits<GLenum>::max();
			break;
		}

		result += Debug::Log(Debug::LogType::Error, "Blaze Engine", Format("Engine error. Invalid GraphicsBufferUseType enum value. The integer value was: {}", ToInteger(use)));
		return std::numeric_limits<GLenum>::max();
	}
}