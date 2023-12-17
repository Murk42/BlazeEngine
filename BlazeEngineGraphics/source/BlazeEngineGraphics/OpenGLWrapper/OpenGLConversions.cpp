#include "pch.h"
#include "BlazeEngineGraphics\OpenGLWrapper/OpenGLConversions.h"

namespace Blaze::Graphics::OpenGLWrapper
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

	GLenum OpenGLPixelFormat(BitmapColorFormat format, Result& result)
	{
		switch (format)
		{
		case BitmapColorFormat::Red: return GL_RED;
		case BitmapColorFormat::RG:	return GL_RG;
		case BitmapColorFormat::RGB: return GL_RGB;
		case BitmapColorFormat::RGBA: return GL_RGBA;
		case BitmapColorFormat::BGR: return GL_BGR;
		case BitmapColorFormat::BGRA: return GL_BGRA;
		}
		result += BLAZE_ERROR_LOG("Blaze Engine", "Invalid BitmapColorFormat enum value. The integer value was: " + StringParsing::Convert(ToInteger(format)));
		return std::numeric_limits<GLenum>::max();
	}
	BitmapColorFormat OepnGLToBlazePixelFormat(GLenum format, Result& result)
	{
		switch (format)
		{
		case GL_LUMINANCE: return BitmapColorFormat::Red;
		case GL_LUMINANCE_ALPHA: return BitmapColorFormat::RG;
		case GL_RGB: return BitmapColorFormat::RGB;
		case GL_RGBA: return BitmapColorFormat::RGBA;
		case GL_BGR: return BitmapColorFormat::BGR;
		case GL_BGRA: return BitmapColorFormat::BGRA;
		}
		result += BLAZE_ERROR_LOG("Blaze Engine", "Unsupported/Invalid OpenGL pixel format enum value. The integer value was: " + StringParsing::Convert(static_cast<uint>(format)));
		return std::numeric_limits<BitmapColorFormat>::max();
	}


	GLenum OpenGLFormatByInternalPixelFormat(TextureInternalPixelFormat format, Result& result)
	{
		switch (format)
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
		result += BLAZE_ERROR_LOG("Blaze Engine", "Invalid TextureInternalPixelFormat enum value. The integer value was: " + StringParsing::Convert(ToInteger(format)));
		return std::numeric_limits<GLenum>::max();
	}
	GLenum OpenGLInternalPixelFormat(TextureInternalPixelFormat format, Result& result)
	{
		switch (format)
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
		case TextureInternalPixelFormat::Depth16:			 return GL_DEPTH_COMPONENT16;
		case TextureInternalPixelFormat::Depth24:			 return GL_DEPTH_COMPONENT24;
		case TextureInternalPixelFormat::Depth32F:			 return GL_DEPTH_COMPONENT32F;
		case TextureInternalPixelFormat::Depth24Stencil8:	 return GL_DEPTH24_STENCIL8;
		case TextureInternalPixelFormat::Depth32FStencil8:	 return GL_DEPTH32F_STENCIL8;
		case TextureInternalPixelFormat::Stencil8:			 return GL_STENCIL_INDEX;
		case TextureInternalPixelFormat::R11F_G11F_B10F:		 return GL_R11F_G11F_B10F;
		}
		result += BLAZE_ERROR_LOG("Blaze Engine", "Invalid TextureInternalPixelFormat enum value. The integer value was: " + StringParsing::Convert(ToInteger(format)));
		return std::numeric_limits<GLenum>::max();
	}
	GLenum OpenGLBufferInternalPixelFormat(TextureBufferInternalPixelFormat format, Result& result)
	{
		switch (format)
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
		result += BLAZE_ERROR_LOG("Blaze Engine", "Invalid TextureBufferInternalPixelFormat enum value. The integer value was: " + StringParsing::Convert(ToInteger(format)));
		return std::numeric_limits<GLenum>::max();
	}	

	GLenum OpenGLPixelType(BitmapColorComponentType type, Result& result)
	{
		switch (type)
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

		result += BLAZE_ERROR_LOG("Blaze Engine", "Invalid BitmapColorComponentType enum value. The integer value was: " + StringParsing::Convert(ToInteger(type)));
		return std::numeric_limits<GLenum>::max();
	}	

	BitmapColorComponentType OpenGLToBlazePixelType(GLenum type, Result& result)
	{
		switch (type)
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
		result += BLAZE_ERROR_LOG("Blaze Engine", "Unsupported/Invalid OpenGL pixel type enum value. The integer value was: " + StringParsing::Convert(static_cast<uint>(type)));
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
					result += BLAZE_ERROR_LOG("Blaze Engine", "Invalid TextureSampling enum value. The integer value was: " + StringParsing::Convert(ToInteger(min)));
					return std::numeric_limits<GLenum>::max();
				}
			else if (mip == TextureSampling::Linear)
				if (min == TextureSampling::Nearest)
					return GL_NEAREST_MIPMAP_LINEAR;
				else if (min == TextureSampling::Linear)
					return GL_LINEAR_MIPMAP_LINEAR;
				else
				{
					result += BLAZE_ERROR_LOG("Blaze Engine", "Invalid TextureSampling enum value. The integer value was: " + StringParsing::Convert(ToInteger(min)));
					return std::numeric_limits<GLenum>::max();
				}
			else
			{
				result += BLAZE_ERROR_LOG("Blaze Engine", "Invalid TextureSampling enum value. The integer value was: " + StringParsing::Convert(ToInteger(mip)));
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
				result += BLAZE_ERROR_LOG("Blaze Engine", "Invalid TextureSampling enum value. The integer value was: " + StringParsing::Convert(ToInteger(min)));
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
		result += BLAZE_ERROR_LOG("Blaze Engine", "Invalid TextureSampling enum value. The integer value was: " + StringParsing::Convert(ToInteger(sampling)));
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
		result += BLAZE_ERROR_LOG("Blaze Engine", "Invalid TextureWrapping enum value. The integer value was: " + StringParsing::Convert(ToInteger(wrapping)));
		return std::numeric_limits<GLenum>::max();
	}
	TextureInternalPixelFormat MapInternalTexturePixelFormat(BitmapColorFormat format, Result& result)
	{
		switch (format)
		{
		case BitmapColorFormat::Red: return TextureInternalPixelFormat::R32F;
		case BitmapColorFormat::RG: return TextureInternalPixelFormat::RG32F;
		case BitmapColorFormat::RGB: return TextureInternalPixelFormat::RGB32F;
		case BitmapColorFormat::RGBA: return TextureInternalPixelFormat::RGBA32F;
		case BitmapColorFormat::BGR: return TextureInternalPixelFormat::RGB32F;
		case BitmapColorFormat::BGRA: return TextureInternalPixelFormat::RGBA32F;
		}
		result += BLAZE_ERROR_LOG("Blaze Engine", "Invalid BitmapColorFormat enum value. The integer value was: " + StringParsing::Convert(ToInteger(format)));
		return std::numeric_limits<TextureInternalPixelFormat>::max();
	}
	BitmapColorFormat MapInternalTexturePixelFormat(TextureInternalPixelFormat format, Result& result)
	{
		switch (format)
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
			Debug::Logger::LogError("Blaze Engine", "Unsupported TextureInternalPixelFormat enum value");
			return BitmapColorFormat::Red;
		case TextureInternalPixelFormat::R11F_G11F_B10F:	return BitmapColorFormat::RGB;
		default:
			Debug::Logger::LogError("Blaze Engine", "Invalid TextureInternalPixelFormat enum value");
			return BitmapColorFormat::Red;
			break;
		}
	}
	GLenum OpenGLVertexAttributeType(VertexAttributeType type, Result& result)
	{
		switch (type)
		{
		case VertexAttributeType::Int8: return GL_BYTE;
		case VertexAttributeType::Uint8: return GL_UNSIGNED_BYTE;
		case VertexAttributeType::Int16: return GL_SHORT;
		case VertexAttributeType::Uint16: return GL_UNSIGNED_SHORT;
		case VertexAttributeType::Int32: return GL_INT;
		case VertexAttributeType::Uint32: return GL_UNSIGNED_INT;
		case VertexAttributeType::Float: return GL_FLOAT;
		case VertexAttributeType::Double: return GL_DOUBLE;
		}
		result += BLAZE_FATAL_LOG("Blaze Engine", "Invalid VertexAttributeType enum value. The integer value was: " + StringParsing::Convert(ToInteger(type)));
		return std::numeric_limits<GLenum>::max();
	}
	GLenum OpenGLFramebufferAttachment(FramebufferAttachment attachment, Result& result)
	{
		switch (attachment)
		{
		case FramebufferAttachment::DepthStencil: return GL_DEPTH_STENCIL_ATTACHMENT;
		case FramebufferAttachment::Stencil:	return GL_STENCIL_ATTACHMENT;
		case FramebufferAttachment::Depth: return GL_DEPTH_ATTACHMENT;
		}
		result += BLAZE_ERROR_LOG("Blaze Engine", "Invalid FramebufferAttachment enum value. The integer value was: " + StringParsing::Convert(ToInteger(attachment)));
		return std::numeric_limits<GLenum>::max();
	}

	GLenum OpenGLBufferMappingAccess(ImmutableGraphicsBufferMapAccess mapping, Result& result)
	{
		GLenum out;

		if (!BitRemap(mapping, out, {
				ImmutableGraphicsBufferMapAccess::Read,
				ImmutableGraphicsBufferMapAccess::Write
			}, {
				GL_MAP_READ_BIT,
				GL_MAP_WRITE_BIT
			}))
		{
			result += BLAZE_ERROR_LOG("Blaze Engine", "Invalid ImmutableGraphicsBufferMapAccess enum value. The integer value was: " + StringParsing::Convert(ToInteger(mapping)));
			return std::numeric_limits<GLenum>::max();
		}

		return out;
	}
	GLenum OpenGLBufferMappingType(ImmutableGraphicsBufferMapType type, Result& result)
	{
		GLenum out;

		if (!BitRemap(type, out, {
				ImmutableGraphicsBufferMapType::PersistentCoherent,
				ImmutableGraphicsBufferMapType::PersistentUncoherent
			}, {
				GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT,
				GL_MAP_PERSISTENT_BIT
			}))
		{
			result += BLAZE_ERROR_LOG("Blaze Engine", "Invalid ImmutableGraphicsBufferMapType enum value. The integer value was: " + StringParsing::Convert(ToInteger(type)));
			return std::numeric_limits<GLenum>::max();
		}

		return out;
	}
	GLenum OpenGLBufferMappingOptions(ImmutableGraphicsBufferMapOptions options, Result& result)
	{
		GLenum out;

		if (!BitRemap(options, out, {
				ImmutableGraphicsBufferMapOptions::ExplicitFlush,
				ImmutableGraphicsBufferMapOptions::InvalidateBuffer,
				ImmutableGraphicsBufferMapOptions::InvalidateRange,
				ImmutableGraphicsBufferMapOptions::Unsynchronized,
			}, {
				GL_MAP_FLUSH_EXPLICIT_BIT,
				GL_MAP_INVALIDATE_BUFFER_BIT,
				GL_MAP_INVALIDATE_RANGE_BIT,
				GL_MAP_UNSYNCHRONIZED_BIT
			}))
		{
			result += BLAZE_ERROR_LOG("Blaze Engine", "Invalid ImmutableGraphicsBufferMapOptions enum value. The integer value was: " + StringParsing::Convert(ToInteger(options)));
			return std::numeric_limits<GLenum>::max();
		}

		return out;
	}
	GLenum OpenGLMutableBufferUsage(MutableGraphicsBufferUseFrequency frequency, MutableGraphicsBufferUseType use, Result& result)
	{
		switch (frequency)
		{
		case MutableGraphicsBufferUseFrequency::Static:	switch (use) {
		case MutableGraphicsBufferUseType::Draw: return GL_STATIC_DRAW;
		case MutableGraphicsBufferUseType::Read: return GL_STATIC_READ;
		case MutableGraphicsBufferUseType::Copy: return GL_STATIC_COPY;
		}
		case MutableGraphicsBufferUseFrequency::Dynamic:	switch (use) {
		case MutableGraphicsBufferUseType::Draw: return GL_DYNAMIC_DRAW;
		case MutableGraphicsBufferUseType::Read: return GL_DYNAMIC_READ;
		case MutableGraphicsBufferUseType::Copy: return GL_DYNAMIC_COPY;
		}
		case MutableGraphicsBufferUseFrequency::Stream:	switch (use) {
		case MutableGraphicsBufferUseType::Draw: return GL_STREAM_DRAW;
		case MutableGraphicsBufferUseType::Read: return GL_STREAM_READ;
		case MutableGraphicsBufferUseType::Copy: return GL_STREAM_COPY;
		}
		default:
			return BLAZE_ERROR_RESULT("Blaze Engine", "Invalid MutableGraphicsBufferUseFrequency enum value. The integer value was: " + StringParsing::Convert(ToInteger(frequency)));
			return std::numeric_limits<GLenum>::max();
			break;
		}

		return BLAZE_ERROR_RESULT("Blaze Engine", "Engine error. Invalid MutableGraphicsBufferUseType enum value. The integer value was: " + StringParsing::Convert(ToInteger(use)));
		return std::numeric_limits<GLenum>::max();
	}
}