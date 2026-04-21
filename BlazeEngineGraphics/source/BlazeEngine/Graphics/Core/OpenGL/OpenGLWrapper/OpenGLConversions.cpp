#include "pch.h"
#include "BlazeEngine/Core/Container/ArrayView.h"
#include "BlazeEngine/Graphics/Core/OpenGL/OpenGLWrapper/OpenGLConversions.h"

namespace Blaze::Graphics::OpenGL
{

	template<typename T>
	inline std::underlying_type_t<T> ToInteger(T value)
	{
		return static_cast<std::underlying_type_t<T>>(value);
	}

	template<typename T1, typename T2>
	constexpr bool BitRemap(const T1& in, T2& out, ArrayView<Tuple<T1, T2>> values)
	{
		T1 value = in;
		out = 0;

		for (auto& [v1, v2] :values)
		{
			if ((bool)(value & v1))
			{
				out |= v2;
				value ^= v1;
			}
		}

		return !(bool)value;
	}

	bool OpenGLPixelFormat(BitmapColorFormat value, GLenum& result)
	{
		switch (value)
		{
		case BitmapColorFormat::Red:  result = GL_RED; break;
		case BitmapColorFormat::RG:	  result = GL_RG; break;
		case BitmapColorFormat::RGB:  result = GL_RGB; break;
		case BitmapColorFormat::RGBA: result = GL_RGBA; break;
		case BitmapColorFormat::BGR:  result = GL_BGR; break;
		case BitmapColorFormat::BGRA: result = GL_BGRA; break;
		default: return false;
		}

		return true;
	}
	bool OpenGLToBlazePixelFormat(GLenum value, BitmapColorFormat& result)
	{
		switch (value)
		{
		case GL_LUMINANCE: result = BitmapColorFormat::Red; break;
		case GL_LUMINANCE_ALPHA: result = BitmapColorFormat::RG; break;
		case GL_RGB: result = BitmapColorFormat::RGB; break;
		case GL_RGBA: result = BitmapColorFormat::RGBA; break;
		case GL_BGR: result = BitmapColorFormat::BGR; break;
		case GL_BGRA: result = BitmapColorFormat::BGRA; break;
		default: return false;
		}

		return true;
	}


	bool OpenGLFormatByInternalPixelFormat(TextureInternalPixelFormat value, GLenum& result)
	{
		switch (value)
		{
		case TextureInternalPixelFormat::RGBA:				 result = GL_RGBA; break;
		case TextureInternalPixelFormat::RGB:				 result = GL_RGB; break;
		case TextureInternalPixelFormat::RG:				 result = GL_RG; break;
		case TextureInternalPixelFormat::R:				     result = GL_RED; break;
		case TextureInternalPixelFormat::RGBA8:				 result = GL_RGBA; break;
		case TextureInternalPixelFormat::RGB8:				 result = GL_RGB; break;
		case TextureInternalPixelFormat::RG8:				 result = GL_RG; break;
		case TextureInternalPixelFormat::R8:				 result = GL_RED; break;
		case TextureInternalPixelFormat::RGBA16:			 result = GL_RGBA; break;
		case TextureInternalPixelFormat::RGB16:				 result = GL_RGB; break;
		case TextureInternalPixelFormat::RG16:				 result = GL_RG; break;
		case TextureInternalPixelFormat::R16:				 result = GL_RED; break;
		case TextureInternalPixelFormat::RGBA16F:			 result = GL_RGBA; break;
		case TextureInternalPixelFormat::RGB16F:			 result = GL_RGB; break;
		case TextureInternalPixelFormat::RG16F:				 result = GL_RG; break;
		case TextureInternalPixelFormat::R16F:				 result = GL_RED; break;
		case TextureInternalPixelFormat::RGBA32F:			 result = GL_RGBA; break;
		case TextureInternalPixelFormat::RGB32F:			 result = GL_RGB; break;
		case TextureInternalPixelFormat::RG32F:				 result = GL_RG; break;
		case TextureInternalPixelFormat::R32F:				 result = GL_RED; break;
		case TextureInternalPixelFormat::RGBA8I:			 result = GL_RGBA; break;
		case TextureInternalPixelFormat::RGB8I:				 result = GL_RGB; break;
		case TextureInternalPixelFormat::RG8I:				 result = GL_RG; break;
		case TextureInternalPixelFormat::R8I:				 result = GL_RED; break;
		case TextureInternalPixelFormat::RGBA16I:			 result = GL_RGBA; break;
		case TextureInternalPixelFormat::RGB16I:			 result = GL_RGB; break;
		case TextureInternalPixelFormat::RG16I:				 result = GL_RG; break;
		case TextureInternalPixelFormat::R16I:				 result = GL_RED; break;
		case TextureInternalPixelFormat::RGBA32I:			 result = GL_RGBA; break;
		case TextureInternalPixelFormat::RGB32I:			 result = GL_RGB; break;
		case TextureInternalPixelFormat::RG32I:				 result = GL_RG; break;
		case TextureInternalPixelFormat::R32I:				 result = GL_RED; break;
		case TextureInternalPixelFormat::RGBA8UI:			 result = GL_RGBA; break;
		case TextureInternalPixelFormat::RGB8UI:			 result = GL_RGB; break;
		case TextureInternalPixelFormat::RG8UI:				 result = GL_RG; break;
		case TextureInternalPixelFormat::R8UI:				 result = GL_RED; break;
		case TextureInternalPixelFormat::RGBA16UI:			 result = GL_RGBA; break;
		case TextureInternalPixelFormat::RGB16UI:			 result = GL_RGB; break;
		case TextureInternalPixelFormat::RG16UI:			 result = GL_RG; break;
		case TextureInternalPixelFormat::R16UI:				 result = GL_RED; break;
		case TextureInternalPixelFormat::RGBA32UI:			 result = GL_RGBA; break;
		case TextureInternalPixelFormat::RGB32UI:			 result = GL_RGB; break;
		case TextureInternalPixelFormat::RG32UI:			 result = GL_RG; break;
		case TextureInternalPixelFormat::R32UI:				 result = GL_RED; break;
		case TextureInternalPixelFormat::Depth16:			 result = GL_DEPTH_COMPONENT; break;
		case TextureInternalPixelFormat::Depth24:			 result = GL_DEPTH_COMPONENT; break;
		case TextureInternalPixelFormat::Depth32F:			 result = GL_DEPTH_COMPONENT; break;
		case TextureInternalPixelFormat::Depth24Stencil8:	 result = GL_DEPTH_STENCIL; break;
		case TextureInternalPixelFormat::Depth32FStencil8:	 result = GL_DEPTH_STENCIL; break;
		case TextureInternalPixelFormat::Stencil8:			 result = GL_STENCIL_INDEX; break;
		case TextureInternalPixelFormat::R11F_G11F_B10F:	 result = GL_RGB; break;
		default: return false;
		}
		
		return true;
	}
	bool OpenGLInternalPixelFormat(TextureInternalPixelFormat value, GLenum& result)
	{
		switch (value)
		{
		case TextureInternalPixelFormat::RGBA:				 result = GL_RGBA; break;
		case TextureInternalPixelFormat::RGB:				 result = GL_RGB; break;
		case TextureInternalPixelFormat::RG:			     result = GL_RG; break;
		case TextureInternalPixelFormat::R:					 result = GL_RED; break;
		case TextureInternalPixelFormat::RGBA8:				 result = GL_RGBA8; break;
		case TextureInternalPixelFormat::RGB8:				 result = GL_RGB8; break;
		case TextureInternalPixelFormat::RG8:				 result = GL_RG8; break;
		case TextureInternalPixelFormat::R8:				 result = GL_R8; break;
		case TextureInternalPixelFormat::RGBA16:			 result = GL_RGBA16; break;
		case TextureInternalPixelFormat::RGB16:				 result = GL_RGB16; break;
		case TextureInternalPixelFormat::RG16:				 result = GL_RG16; break;
		case TextureInternalPixelFormat::R16:				 result = GL_R16; break;
		case TextureInternalPixelFormat::RGBA16F:			 result = GL_RGBA16F; break;
		case TextureInternalPixelFormat::RGB16F:			 result = GL_RGB16F; break;
		case TextureInternalPixelFormat::RG16F:				 result = GL_RG16F; break;
		case TextureInternalPixelFormat::R16F:				 result = GL_R16F; break;
		case TextureInternalPixelFormat::RGBA32F:			 result = GL_RGBA32F; break;
		case TextureInternalPixelFormat::RGB32F:			 result = GL_RGB32F; break;
		case TextureInternalPixelFormat::RG32F:				 result = GL_RG32F; break;
		case TextureInternalPixelFormat::R32F:				 result = GL_R32F; break;
		case TextureInternalPixelFormat::RGBA8I:			 result = GL_RGBA8I; break;
		case TextureInternalPixelFormat::RGB8I:				 result = GL_RGB8I; break;
		case TextureInternalPixelFormat::RG8I:				 result = GL_RG8I; break;
		case TextureInternalPixelFormat::R8I:				 result = GL_R8I; break;
		case TextureInternalPixelFormat::RGBA16I:			 result = GL_RGBA16I; break;
		case TextureInternalPixelFormat::RGB16I:			 result = GL_RGB16I; break;
		case TextureInternalPixelFormat::RG16I:				 result = GL_RG16I; break;
		case TextureInternalPixelFormat::R16I:				 result = GL_R16I; break;
		case TextureInternalPixelFormat::RGBA32I:			 result = GL_RGBA32I; break;
		case TextureInternalPixelFormat::RGB32I:			 result = GL_RGB32I; break;
		case TextureInternalPixelFormat::RG32I:				 result = GL_RG32I; break;
		case TextureInternalPixelFormat::R32I:				 result = GL_R32I; break;
		case TextureInternalPixelFormat::RGBA8UI:			 result = GL_RGBA8UI; break;
		case TextureInternalPixelFormat::RGB8UI:			 result = GL_RGB8UI; break;
		case TextureInternalPixelFormat::RG8UI:				 result = GL_RG8UI; break;
		case TextureInternalPixelFormat::R8UI:				 result = GL_R8UI; break;
		case TextureInternalPixelFormat::RGBA16UI:			 result = GL_RGBA16UI; break;
		case TextureInternalPixelFormat::RGB16UI:			 result = GL_RGB16UI; break;
		case TextureInternalPixelFormat::RG16UI:			 result = GL_RG16UI; break;
		case TextureInternalPixelFormat::R16UI:				 result = GL_R16UI; break;
		case TextureInternalPixelFormat::RGBA32UI:			 result = GL_RGBA32UI; break;
		case TextureInternalPixelFormat::RGB32UI:			 result = GL_RGB32UI; break;
		case TextureInternalPixelFormat::RG32UI:			 result = GL_RG32UI; break;
		case TextureInternalPixelFormat::R32UI:				 result = GL_R32UI; break;
		case TextureInternalPixelFormat::SRGB24:             result = GL_SRGB8; break;
		case TextureInternalPixelFormat::SRGBA32:            result = GL_SRGB8_ALPHA8; break;
		case TextureInternalPixelFormat::Depth16:			 result = GL_DEPTH_COMPONENT16; break;
		case TextureInternalPixelFormat::Depth24:			 result = GL_DEPTH_COMPONENT24; break;
		case TextureInternalPixelFormat::Depth32F:			 result = GL_DEPTH_COMPONENT32F; break;
		case TextureInternalPixelFormat::Depth24Stencil8:	 result = GL_DEPTH24_STENCIL8; break;
		case TextureInternalPixelFormat::Depth32FStencil8:	 result = GL_DEPTH32F_STENCIL8; break;
		case TextureInternalPixelFormat::Stencil8:			 result = GL_STENCIL_INDEX; break;
		case TextureInternalPixelFormat::R11F_G11F_B10F:	 result = GL_R11F_G11F_B10F; break;
		default: return false;
		}

		return true;
	}
	bool OpenGLBufferInternalPixelFormat(TextureBufferInternalPixelFormat value, GLenum& result)
	{
		switch (value)
		{
		case TextureBufferInternalPixelFormat::R8:       result = GL_R8; break;
		case TextureBufferInternalPixelFormat::R16:      result = GL_R16; break;
		case TextureBufferInternalPixelFormat::R16F:     result = GL_R16F; break;
		case TextureBufferInternalPixelFormat::R32F:     result = GL_R32F; break;
		case TextureBufferInternalPixelFormat::R8I:      result = GL_R8I; break;
		case TextureBufferInternalPixelFormat::R16I:     result = GL_R16I; break;
		case TextureBufferInternalPixelFormat::R32I:     result = GL_R32I; break;
		case TextureBufferInternalPixelFormat::R8UI:     result = GL_R8UI; break;
		case TextureBufferInternalPixelFormat::R16UI:    result = GL_R16UI; break;
		case TextureBufferInternalPixelFormat::R32UI:    result = GL_R32UI; break;
		case TextureBufferInternalPixelFormat::RG8:      result = GL_RG8; break;
		case TextureBufferInternalPixelFormat::RG16:     result = GL_RG16; break;
		case TextureBufferInternalPixelFormat::RG16F:    result = GL_RG16F; break;
		case TextureBufferInternalPixelFormat::RG32F:    result = GL_RG32F; break;
		case TextureBufferInternalPixelFormat::RG8I:     result = GL_RG8I; break;
		case TextureBufferInternalPixelFormat::RG16I:    result = GL_RG16I; break;
		case TextureBufferInternalPixelFormat::RG32I:    result = GL_RG32I; break;
		case TextureBufferInternalPixelFormat::RG8UI:    result = GL_RG8UI; break;
		case TextureBufferInternalPixelFormat::RG16UI:   result = GL_RG16UI; break;
		case TextureBufferInternalPixelFormat::RG32UI:   result = GL_RG32UI; break;
		case TextureBufferInternalPixelFormat::RGB32F:   result = GL_RGB32F; break;
		case TextureBufferInternalPixelFormat::RGB32I:   result = GL_RGB32I; break;
		case TextureBufferInternalPixelFormat::RGB32UI:  result = GL_RGB32UI; break;
		case TextureBufferInternalPixelFormat::RGBA8:    result = GL_RGBA8; break;
		case TextureBufferInternalPixelFormat::RGBA16:   result = GL_RGBA16; break;
		case TextureBufferInternalPixelFormat::RGBA16F:  result = GL_RGBA16F; break;
		case TextureBufferInternalPixelFormat::RGBA32F:  result = GL_RGBA32F; break;
		case TextureBufferInternalPixelFormat::RGBA8I:   result = GL_RGBA8I; break;
		case TextureBufferInternalPixelFormat::RGBA16I:  result = GL_RGBA16I; break;
		case TextureBufferInternalPixelFormat::RGBA32I:  result = GL_RGBA32I; break;
		case TextureBufferInternalPixelFormat::RGBA8UI:  result = GL_RGBA8UI; break;
		case TextureBufferInternalPixelFormat::RGBA16UI: result = GL_RGBA16UI; break;
		case TextureBufferInternalPixelFormat::RGBA32UI: result = GL_RGBA32UI; break;
		default: return false;
		}

		return true;
	}

	bool OpenGLPixelType(BitmapColorComponentType value, GLenum& result)
	{
		switch (value)
		{
		case BitmapColorComponentType::Int8:   result = GL_BYTE; break;
		case BitmapColorComponentType::Uint8:  result = GL_UNSIGNED_BYTE; break;
		case BitmapColorComponentType::Int16:  result = GL_SHORT; break;
		case BitmapColorComponentType::Uint16: result = GL_UNSIGNED_SHORT; break;
		case BitmapColorComponentType::Int32:  result = GL_INT; break;
		case BitmapColorComponentType::Uint32: result = GL_UNSIGNED_INT; break;
		case BitmapColorComponentType::Float:  result = GL_FLOAT; break;
		case BitmapColorComponentType::Double: result = GL_DOUBLE; break;
		default: return false;
		}

		return true;
	}

	bool OpenGLToBlazePixelType(GLenum value, BitmapColorComponentType& result)
	{
		switch (value)
		{
		case GL_BYTE:           result = BitmapColorComponentType::Int8; break;
		case GL_UNSIGNED_BYTE:  result = BitmapColorComponentType::Uint8; break;
		case GL_SHORT:          result = BitmapColorComponentType::Int16; break;
		case GL_UNSIGNED_SHORT: result = BitmapColorComponentType::Uint16; break;
		case GL_INT:            result = BitmapColorComponentType::Int32; break;
		case GL_UNSIGNED_INT:   result = BitmapColorComponentType::Uint32; break;
		case GL_FLOAT:          result = BitmapColorComponentType::Float; break;
		case GL_DOUBLE:         result = BitmapColorComponentType::Double; break;
		default: return false;
		}

		return true;
	}
	bool OpenGLTextureMinSampling(TextureSampling min, TextureSampling mip, bool mipmaps, GLenum& result)
	{
		if (mipmaps)
		{
			if (mip == TextureSampling::Nearest)
			{
				if (min == TextureSampling::Nearest)
					result = GL_NEAREST_MIPMAP_NEAREST;
				else if (min == TextureSampling::Linear)
					result = GL_LINEAR_MIPMAP_NEAREST;
				else
					return false;
			}
			else if (mip == TextureSampling::Linear)
			{
				if (min == TextureSampling::Nearest)
					result = GL_NEAREST_MIPMAP_LINEAR;
				else if (min == TextureSampling::Linear)
					result = GL_LINEAR_MIPMAP_LINEAR;
				else
					return false;
			}
			else
				return false;
		}
		else
			if (min == TextureSampling::Nearest)
				result = GL_NEAREST;
			else if (min == TextureSampling::Linear)
				result = GL_LINEAR;
			else
				return false;

		return true;
	}
	bool OpenGLTextureMagSampling(TextureSampling sampling, GLenum& result)
	{
		switch (sampling)
		{
		case TextureSampling::Nearest: result = GL_NEAREST; break;
		case TextureSampling::Linear:  result = GL_LINEAR; break;
		default: return false;
		}
		return true;
	}
	bool OpenGLTextureWrapping(TextureWrapping wrapping, GLenum& result)
	{
		switch (wrapping)
		{
		case TextureWrapping::ClampToBorder:  result = GL_CLAMP_TO_BORDER; break;
		case TextureWrapping::ClampToEdge:    result = GL_CLAMP_TO_EDGE; break;
		case TextureWrapping::MirroredRepeat: result = GL_MIRRORED_REPEAT; break;
		case TextureWrapping::Repeat:         result = GL_REPEAT; break;
		default: return false;
		}
		return true;
	}
	bool BlazeInternalTexturePixelFormat(BitmapColorFormat value, TextureInternalPixelFormat& result)
	{
		switch (value)
		{
		case BitmapColorFormat::Red:  result = TextureInternalPixelFormat::R32F; break;
		case BitmapColorFormat::RG:   result = TextureInternalPixelFormat::RG32F; break;
		case BitmapColorFormat::RGB:  result = TextureInternalPixelFormat::RGB32F; break;
		case BitmapColorFormat::RGBA: result = TextureInternalPixelFormat::RGBA32F; break;
		case BitmapColorFormat::BGR:  result = TextureInternalPixelFormat::RGB32F; break;
		case BitmapColorFormat::BGRA: result = TextureInternalPixelFormat::RGBA32F; break;
		default: return false;
		}

		return true;
	}
	bool BlazeBitmapColorFormat(TextureInternalPixelFormat value, BitmapColorFormat& result)
	{
		switch (value)
		{
		case TextureInternalPixelFormat::RGBA:           result = BitmapColorFormat::RGBA; break;
		case TextureInternalPixelFormat::RGB:            result = BitmapColorFormat::RGB; break;
		case TextureInternalPixelFormat::RG:             result = BitmapColorFormat::RG; break;
		case TextureInternalPixelFormat::R:              result = BitmapColorFormat::Red; break;
		case TextureInternalPixelFormat::RGBA8:          result = BitmapColorFormat::RGBA; break;
		case TextureInternalPixelFormat::RGB8:	         result = BitmapColorFormat::RGB; break;
		case TextureInternalPixelFormat::RG8:	         result = BitmapColorFormat::RG; break;
		case TextureInternalPixelFormat::R8:	         result = BitmapColorFormat::Red; break;
		case TextureInternalPixelFormat::RGBA16:         result = BitmapColorFormat::RGBA; break;
		case TextureInternalPixelFormat::RGB16:	         result = BitmapColorFormat::RGB; break;
		case TextureInternalPixelFormat::RG16:	         result = BitmapColorFormat::RG; break;
		case TextureInternalPixelFormat::R16:	         result = BitmapColorFormat::Red; break;
		case TextureInternalPixelFormat::RGBA16F:        result = BitmapColorFormat::RGBA; break;
		case TextureInternalPixelFormat::RGB16F:         result = BitmapColorFormat::RGB; break;
		case TextureInternalPixelFormat::RG16F:	         result = BitmapColorFormat::RG; break;
		case TextureInternalPixelFormat::R16F:	         result = BitmapColorFormat::Red; break;
		case TextureInternalPixelFormat::RGBA32F:        result = BitmapColorFormat::RGBA; break;
		case TextureInternalPixelFormat::RGB32F:         result = BitmapColorFormat::RGB; break;
		case TextureInternalPixelFormat::RG32F:	         result = BitmapColorFormat::RG; break;
		case TextureInternalPixelFormat::R32F:	         result = BitmapColorFormat::Red; break;
		case TextureInternalPixelFormat::RGBA8I:         result = BitmapColorFormat::RGBA; break;
		case TextureInternalPixelFormat::RGB8I:	         result = BitmapColorFormat::RGB; break;
		case TextureInternalPixelFormat::RG8I:	         result = BitmapColorFormat::RG; break;
		case TextureInternalPixelFormat::R8I:	         result = BitmapColorFormat::Red; break;
		case TextureInternalPixelFormat::RGBA16I:        result = BitmapColorFormat::RGBA; break;
		case TextureInternalPixelFormat::RGB16I:         result = BitmapColorFormat::RGB; break;
		case TextureInternalPixelFormat::RG16I:	         result = BitmapColorFormat::RG; break;
		case TextureInternalPixelFormat::R16I:	         result = BitmapColorFormat::Red; break;
		case TextureInternalPixelFormat::RGBA32I:        result = BitmapColorFormat::RGBA; break;
		case TextureInternalPixelFormat::RGB32I:         result = BitmapColorFormat::RGB; break;
		case TextureInternalPixelFormat::RG32I:	         result = BitmapColorFormat::RG; break;
		case TextureInternalPixelFormat::R32I:	         result = BitmapColorFormat::Red; break;
		case TextureInternalPixelFormat::RGBA8UI:        result = BitmapColorFormat::RGBA; break;
		case TextureInternalPixelFormat::RGB8UI:         result = BitmapColorFormat::RGB; break;
		case TextureInternalPixelFormat::RG8UI:	         result = BitmapColorFormat::RG; break;
		case TextureInternalPixelFormat::R8UI:	         result = BitmapColorFormat::Red; break;
		case TextureInternalPixelFormat::RGBA16UI:       result = BitmapColorFormat::RGBA; break;
		case TextureInternalPixelFormat::RGB16UI:        result = BitmapColorFormat::RGB; break;
		case TextureInternalPixelFormat::RG16UI:         result = BitmapColorFormat::RG; break;
		case TextureInternalPixelFormat::R16UI:	         result = BitmapColorFormat::Red; break;
		case TextureInternalPixelFormat::RGBA32UI:       result = BitmapColorFormat::RGBA; break;
		case TextureInternalPixelFormat::RGB32UI:        result = BitmapColorFormat::RGB; break;
		case TextureInternalPixelFormat::RG32UI:         result = BitmapColorFormat::RG; break;
		case TextureInternalPixelFormat::R32UI:	         result = BitmapColorFormat::Red; break;
		case TextureInternalPixelFormat::R11F_G11F_B10F: result = BitmapColorFormat::RGB; break;
		case TextureInternalPixelFormat::Depth16:
		case TextureInternalPixelFormat::Depth24:
		case TextureInternalPixelFormat::Depth32F:
		case TextureInternalPixelFormat::Depth24Stencil8:
		case TextureInternalPixelFormat::Depth32FStencil8:
		case TextureInternalPixelFormat::Stencil8:
		default:
			return false;
		}

		return true;
	}
	bool OpenGLIntegerVertexAttributeType(IntegerVertexAttributeType value, GLenum& result)
	{
		switch (value)
		{
		case Blaze::Graphics::OpenGL::IntegerVertexAttributeType::Int8:   result = GL_BYTE; break;
		case Blaze::Graphics::OpenGL::IntegerVertexAttributeType::Int16:  result = GL_SHORT; break;
		case Blaze::Graphics::OpenGL::IntegerVertexAttributeType::Int32:  result = GL_INT; break;
		case Blaze::Graphics::OpenGL::IntegerVertexAttributeType::Uint8:  result = GL_UNSIGNED_BYTE; break;
		case Blaze::Graphics::OpenGL::IntegerVertexAttributeType::Uint16: result = GL_UNSIGNED_SHORT; break;
		case Blaze::Graphics::OpenGL::IntegerVertexAttributeType::Uint32: result = GL_UNSIGNED_INT; break;
		default:
			return false;
		}

		return true;
	}
	bool OpenGLFloatVertexAttributeType(FloatVertexAttributeType value, GLenum& result)
	{
		switch (value)
		{
		case Blaze::Graphics::OpenGL::FloatVertexAttributeType::Int8: result = GL_BYTE; break;
		case Blaze::Graphics::OpenGL::FloatVertexAttributeType::Int16: result = GL_SHORT; break;
		case Blaze::Graphics::OpenGL::FloatVertexAttributeType::Int32: result = GL_INT; break;
		case Blaze::Graphics::OpenGL::FloatVertexAttributeType::Uint8: result = GL_UNSIGNED_BYTE; break;
		case Blaze::Graphics::OpenGL::FloatVertexAttributeType::Uint16: result = GL_UNSIGNED_SHORT; break;
		case Blaze::Graphics::OpenGL::FloatVertexAttributeType::Uint32: result = GL_UNSIGNED_INT; break;
		case Blaze::Graphics::OpenGL::FloatVertexAttributeType::Half: result = GL_HALF_FLOAT; break;
		case Blaze::Graphics::OpenGL::FloatVertexAttributeType::Float: result = GL_FLOAT; break;
		case Blaze::Graphics::OpenGL::FloatVertexAttributeType::Double: result = GL_DOUBLE; break;
		default:
			return false;
		}

		return true;
	}
	bool OpenGLPackedVertexAttributeType(PackedVertexAttributeType value, GLenum& result)
	{
		switch (value)
		{
		case Blaze::Graphics::OpenGL::PackedVertexAttributeType::Int_2_10_10_10_Rev: result = GL_INT_2_10_10_10_REV; break;
		case Blaze::Graphics::OpenGL::PackedVertexAttributeType::Uint_2_10_10_10_Rev: result = GL_UNSIGNED_INT_2_10_10_10_REV; break;
		case Blaze::Graphics::OpenGL::PackedVertexAttributeType::Uint_10F_11F_11F_Rev: result = GL_UNSIGNED_INT_10F_11F_11F_REV; break;
		default:
			return false;
		}

		return true;
	}
	bool OpenGLBGRAVertexAttributeType(BGRAVertexAttributeType value, GLenum& result)
	{
		switch (value)
		{
		case Blaze::Graphics::OpenGL::BGRAVertexAttributeType::Uint8: result = GL_UNSIGNED_BYTE; break;
		case Blaze::Graphics::OpenGL::BGRAVertexAttributeType::Int_2_10_10_10_Rev: result = GL_INT_2_10_10_10_REV; break;
		case Blaze::Graphics::OpenGL::BGRAVertexAttributeType::Uint_2_10_10_10_Rev: result = GL_UNSIGNED_INT_2_10_10_10_REV; break;
		default:
			return false;
		}

		return true;
	}
	bool OpenGLFramebufferAttachment(FramebufferAttachment value, GLenum& result)
	{
		switch (value)
		{
		case FramebufferAttachment::DepthStencil: result = GL_DEPTH_STENCIL_ATTACHMENT; break;
		case FramebufferAttachment::Stencil:	  result = GL_STENCIL_ATTACHMENT; break;
		case FramebufferAttachment::Depth:	      result = GL_DEPTH_ATTACHMENT; break;
		default :
			return false;
		}

		return true;
	}

	bool BlazeFramebufferStatus(GLenum value, FramebufferStatus& result)
	{
		switch (value)
		{
		case GL_FRAMEBUFFER_COMPLETE: result = FramebufferStatus::Complete; break;
		case GL_FRAMEBUFFER_UNDEFINED: result = FramebufferStatus::Undefined; break;
		case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT: result = FramebufferStatus::IncompleteAttachment; break;
		case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT: result = FramebufferStatus::MissingAttachment; break;
		case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER: result = FramebufferStatus::IncompleteDrawBuffer; break;
		case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER: result = FramebufferStatus::IncompleteReadBuffer; break;
		case GL_FRAMEBUFFER_UNSUPPORTED:result = FramebufferStatus::Unsupported; break;
		case GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE: result = FramebufferStatus::IncompleteMultisample; break;
		case GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS: result = FramebufferStatus::IncompleteLayerTargets; break;
		default:
			return false;
		}

		return true;
	}

	bool OpenGLRenderWindowFramebufferBufferTargets(RenderWindowFramebufferBufferTargets value, GLenum& result)
	{
		switch (value)
		{
		case Blaze::Graphics::OpenGL::RenderWindowFramebufferBufferTargets::FrontLeft: result = GL_FRONT_LEFT; break;
		case Blaze::Graphics::OpenGL::RenderWindowFramebufferBufferTargets::FrontRight: result = GL_FRONT_RIGHT; break;
		case Blaze::Graphics::OpenGL::RenderWindowFramebufferBufferTargets::BackLeft: result = GL_BACK_LEFT; break;
		case Blaze::Graphics::OpenGL::RenderWindowFramebufferBufferTargets::BackRight: result = GL_BACK_RIGHT; break;
		case Blaze::Graphics::OpenGL::RenderWindowFramebufferBufferTargets::Front: result = GL_FRONT; break;
		case Blaze::Graphics::OpenGL::RenderWindowFramebufferBufferTargets::Back: result = GL_BACK; break;
		case Blaze::Graphics::OpenGL::RenderWindowFramebufferBufferTargets::Left: result = GL_LEFT; break;
		case Blaze::Graphics::OpenGL::RenderWindowFramebufferBufferTargets::Right: result = GL_RIGHT; break;
		case Blaze::Graphics::OpenGL::RenderWindowFramebufferBufferTargets::FontAndBack: result = GL_FRONT_AND_BACK; break;
		default:
			return false;
		}

		return true;
	}

	bool OpenGLRenderWindowFramebufferBufferTarget(RenderWindowFramebufferBufferTarget value, GLenum& result)
	{
		switch (value)
		{
		case Blaze::Graphics::OpenGL::RenderWindowFramebufferBufferTarget::FrontLeft: result = GL_FRONT_LEFT; return true;
		case Blaze::Graphics::OpenGL::RenderWindowFramebufferBufferTarget::FrontRight: result = GL_FRONT_RIGHT; return true;
		case Blaze::Graphics::OpenGL::RenderWindowFramebufferBufferTarget::BackLeft: result = GL_BACK_LEFT; return true;
		case Blaze::Graphics::OpenGL::RenderWindowFramebufferBufferTarget::BackRight: result = GL_BACK_RIGHT; return true;
		default: return false;
		}
	}

	bool OpenGLBufferMappingAccess(GraphicsBufferMapAccessFlags value, GLenum& result)
	{
		return BitRemap(value, result, {
				{ GraphicsBufferMapAccessFlags::Read, GL_MAP_READ_BIT },
				{ GraphicsBufferMapAccessFlags::Write, GL_MAP_WRITE_BIT }
			});
	}
	bool OpenGLBufferMappingType(GraphicsBufferMapType value, GLenum& result)
	{
		return BitRemap(value, result, {
				{ GraphicsBufferMapType::PersistentCoherent, GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT },
				{ GraphicsBufferMapType::PersistentUncoherent, GL_MAP_PERSISTENT_BIT }
			});
	}
	bool OpenGLBufferMappingOptions(GraphicsBufferMapOptions value, GLenum& result)
	{
		return BitRemap(value, result, {
				{ GraphicsBufferMapOptions::ExplicitFlush, GL_MAP_FLUSH_EXPLICIT_BIT },
				{ GraphicsBufferMapOptions::InvalidateBuffer, GL_MAP_INVALIDATE_BUFFER_BIT },
				{ GraphicsBufferMapOptions::InvalidateRange, GL_MAP_INVALIDATE_RANGE_BIT },
				{ GraphicsBufferMapOptions::Unsynchronized, GL_MAP_UNSYNCHRONIZED_BIT }
			});
	}

	bool BlazeUniformType(GLenum value, UniformType& result)
	{
		switch (value)
		{
		case GL_FLOAT: result = UniformType::Float; break;
		case GL_FLOAT_VEC2: result = UniformType::Vec2f; break;
		case GL_FLOAT_VEC3: result = UniformType::Vec3f; break;
		case GL_FLOAT_VEC4: result = UniformType::Vec4f; break;
		case GL_DOUBLE: result = UniformType::Double; break;
		case GL_DOUBLE_VEC2: result = UniformType::Vec2d; break;
		case GL_DOUBLE_VEC3: result = UniformType::Vec3d; break;
		case GL_DOUBLE_VEC4: result = UniformType::Vec4d; break;
		case GL_INT: result = UniformType::Int; break;
		case GL_INT_VEC2: result = UniformType::Vec2i; break;
		case GL_INT_VEC3: result = UniformType::Vec3i; break;
		case GL_INT_VEC4: result = UniformType::Vec4i; break;
		case GL_UNSIGNED_INT: result = UniformType::UnsignedInt; break;
		case GL_UNSIGNED_INT_VEC2: result = UniformType::Vec2u; break;
		case GL_UNSIGNED_INT_VEC3: result = UniformType::Vec3u; break;
		case GL_UNSIGNED_INT_VEC4: result = UniformType::Vec4u; break;
		case GL_BOOL: result = UniformType::Bool; break;
		case GL_BOOL_VEC2: result = UniformType::BoolVec2; break;
		case GL_BOOL_VEC3: result = UniformType::BoolVec3; break;
		case GL_BOOL_VEC4: result = UniformType::BoolVec4; break;
		case GL_FLOAT_MAT2: result = UniformType::Mat2f; break;
		case GL_FLOAT_MAT3: result = UniformType::Mat3f; break;
		case GL_FLOAT_MAT4: result = UniformType::Mat4f; break;
		case GL_FLOAT_MAT2x3: result = UniformType::Mat2x3f; break;
		case GL_FLOAT_MAT2x4: result = UniformType::Mat2x4f; break;
		case GL_FLOAT_MAT3x2: result = UniformType::Mat3x2f; break;
		case GL_FLOAT_MAT3x4: result = UniformType::Mat3x4f; break;
		case GL_FLOAT_MAT4x2: result = UniformType::Mat4x2f; break;
		case GL_FLOAT_MAT4x3: result = UniformType::Mat4x3f; break;
		case GL_DOUBLE_MAT2: result = UniformType::Mat2d; break;
		case GL_DOUBLE_MAT3: result = UniformType::Mat3d; break;
		case GL_DOUBLE_MAT4: result = UniformType::Mat4d; break;
		case GL_DOUBLE_MAT2x3: result = UniformType::Mat2x3d; break;
		case GL_DOUBLE_MAT2x4: result = UniformType::Mat2x4d; break;
		case GL_DOUBLE_MAT3x2: result = UniformType::Mat3x2d; break;
		case GL_DOUBLE_MAT3x4: result = UniformType::Mat3x4d; break;
		case GL_DOUBLE_MAT4x2: result = UniformType::Mat4x2d; break;
		case GL_DOUBLE_MAT4x3: result = UniformType::Mat4x3d; break;
		case GL_SAMPLER_1D: result = UniformType::Sampler1D; break;
		case GL_SAMPLER_2D: result = UniformType::Sampler2D; break;
		case GL_SAMPLER_3D: result = UniformType::Sampler3D; break;
		case GL_SAMPLER_CUBE: result = UniformType::SamplerCube; break;
		case GL_SAMPLER_1D_SHADOW: result = UniformType::Sampler1DShadow; break;
		case GL_SAMPLER_2D_SHADOW: result = UniformType::Sampler2DShadow; break;
		case GL_SAMPLER_1D_ARRAY: result = UniformType::Sampler1DArray; break;
		case GL_SAMPLER_2D_ARRAY: result = UniformType::Sampler2DArray; break;
		case GL_SAMPLER_1D_ARRAY_SHADOW: result = UniformType::Sampler1DArrayShadow; break;
		case GL_SAMPLER_2D_ARRAY_SHADOW: result = UniformType::Sampler2DArrayShadow; break;
		case GL_SAMPLER_2D_MULTISAMPLE: result = UniformType::Sampler2DMultisample; break;
		case GL_SAMPLER_2D_MULTISAMPLE_ARRAY: result = UniformType::Sampler2DMultisampleArray; break;
		case GL_SAMPLER_CUBE_SHADOW: result = UniformType::SamplerCubeShadow; break;
		case GL_SAMPLER_BUFFER: result = UniformType::SamplerBuffer; break;
		case GL_SAMPLER_2D_RECT: result = UniformType::Sampler2DRect; break;
		case GL_SAMPLER_2D_RECT_SHADOW: result = UniformType::Sampler2DRectShadow; break;
		case GL_INT_SAMPLER_1D: result = UniformType::IntSampler1D; break;
		case GL_INT_SAMPLER_2D: result = UniformType::IntSampler2D; break;
		case GL_INT_SAMPLER_3D: result = UniformType::IntSampler3D; break;
		case GL_INT_SAMPLER_CUBE: result = UniformType::IntSamplerCube; break;
		case GL_INT_SAMPLER_1D_ARRAY: result = UniformType::IntSampler1DArray; break;
		case GL_INT_SAMPLER_2D_ARRAY: result = UniformType::IntSampler2DArray; break;
		case GL_INT_SAMPLER_2D_MULTISAMPLE: result = UniformType::IntSampler2DMultisample; break;
		case GL_INT_SAMPLER_2D_MULTISAMPLE_ARRAY: result = UniformType::IntSampler2DMultisampleArray; break;
		case GL_INT_SAMPLER_BUFFER: result = UniformType::IntSamplerBuffer; break;
		case GL_INT_SAMPLER_2D_RECT: result = UniformType::IntSampler2DRect; break;
		case GL_UNSIGNED_INT_SAMPLER_1D: result = UniformType::UnsignedIntSampler1D; break;
		case GL_UNSIGNED_INT_SAMPLER_2D: result = UniformType::UnsignedIntSampler2D; break;
		case GL_UNSIGNED_INT_SAMPLER_3D: result = UniformType::UnsignedIntSampler3D; break;
		case GL_UNSIGNED_INT_SAMPLER_CUBE: result = UniformType::UnsignedIntSamplerCube; break;
		case GL_UNSIGNED_INT_SAMPLER_1D_ARRAY: result = UniformType::UnsignedIntSampler1DArray; break;
		case GL_UNSIGNED_INT_SAMPLER_2D_ARRAY: result = UniformType::UnsignedIntSampler2DArray; break;
		case GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE: result = UniformType::UnsignedIntSampler2DMultisample; break;
		case GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE_ARRAY: result = UniformType::UnsignedIntSampler2DMultisampleArray; break;
		case GL_UNSIGNED_INT_SAMPLER_BUFFER: result = UniformType::UnsignedIntSamplerBuffer; break;
		case GL_UNSIGNED_INT_SAMPLER_2D_RECT: result = UniformType::UnsignedIntSampler2DRect; break;
		case GL_IMAGE_1D: result = UniformType::Image1D; break;
		case GL_IMAGE_2D: result = UniformType::Image2D; break;
		case GL_IMAGE_3D: result = UniformType::Image3D; break;
		case GL_IMAGE_2D_RECT: result = UniformType::Image2DRect; break;
		case GL_IMAGE_CUBE: result = UniformType::ImageCube; break;
		case GL_IMAGE_BUFFER: result = UniformType::ImageBuffer; break;
		case GL_IMAGE_1D_ARRAY: result = UniformType::Image1DArray; break;
		case GL_IMAGE_2D_ARRAY: result = UniformType::Image2DArray; break;
		case GL_IMAGE_2D_MULTISAMPLE: result = UniformType::Image2DMultisample; break;
		case GL_IMAGE_2D_MULTISAMPLE_ARRAY: result = UniformType::Image2DMultisampleArray; break;
		case GL_INT_IMAGE_1D: result = UniformType::IntImage1D; break;
		case GL_INT_IMAGE_2D: result = UniformType::IntImage2D; break;
		case GL_INT_IMAGE_3D: result = UniformType::IntImage3D; break;
		case GL_INT_IMAGE_2D_RECT: result = UniformType::IntImage2DRect; break;
		case GL_INT_IMAGE_CUBE: result = UniformType::IntImageCube; break;
		case GL_INT_IMAGE_BUFFER: result = UniformType::IntImageBuffer; break;
		case GL_INT_IMAGE_1D_ARRAY: result = UniformType::IntImage1DArray; break;
		case GL_INT_IMAGE_2D_ARRAY: result = UniformType::IntImage2DArray; break;
		case GL_INT_IMAGE_2D_MULTISAMPLE: result = UniformType::IntImage2DMultisample; break;
		case GL_INT_IMAGE_2D_MULTISAMPLE_ARRAY: result = UniformType::IntImage2DMultisampleArray; break;
		case GL_UNSIGNED_INT_IMAGE_1D: result = UniformType::UnsignedIntImage1D; break;
		case GL_UNSIGNED_INT_IMAGE_2D: result = UniformType::UnsignedIntImage2D; break;
		case GL_UNSIGNED_INT_IMAGE_3D: result = UniformType::UnsignedIntImage3D; break;
		case GL_UNSIGNED_INT_IMAGE_2D_RECT: result = UniformType::UnsignedIntImage2DRect; break;
		case GL_UNSIGNED_INT_IMAGE_CUBE: result = UniformType::UnsignedIntImageCube; break;
		case GL_UNSIGNED_INT_IMAGE_BUFFER: result = UniformType::UnsignedIntImageBuffer; break;
		case GL_UNSIGNED_INT_IMAGE_1D_ARRAY: result = UniformType::UnsignedIntImage1DArray; break;
		case GL_UNSIGNED_INT_IMAGE_2D_ARRAY: result = UniformType::UnsignedIntImage2DArray; break;
		case GL_UNSIGNED_INT_IMAGE_2D_MULTISAMPLE: result = UniformType::UnsignedIntImage2DMultisample; break;
		case GL_UNSIGNED_INT_IMAGE_2D_MULTISAMPLE_ARRAY: result = UniformType::UnsignedIntImage2DMultisampleArray; break;
		case GL_UNSIGNED_INT_ATOMIC_COUNTER: result = UniformType::UnsignedIntAtomicCounter; break;
		default: return false;
		}

		return true;
	}

	bool OpenGLMutableBufferUsage(GraphicsBufferUseFrequency value, GraphicsBufferUseType use, GLenum& result)
	{
		switch (value)
		{
		case GraphicsBufferUseFrequency::Static:	switch (use) {
			case GraphicsBufferUseType::Draw: result = GL_STATIC_DRAW; break;
			case GraphicsBufferUseType::Read: result = GL_STATIC_READ; break;
			case GraphicsBufferUseType::Copy: result = GL_STATIC_COPY; break;
		} break;
		case GraphicsBufferUseFrequency::Dynamic:	switch (use) {
			case GraphicsBufferUseType::Draw: result = GL_DYNAMIC_DRAW; break;
			case GraphicsBufferUseType::Read: result = GL_DYNAMIC_READ; break;
			case GraphicsBufferUseType::Copy: result = GL_DYNAMIC_COPY; break;
		} break;
		case GraphicsBufferUseFrequency::Stream:	switch (use) {
			case GraphicsBufferUseType::Draw: result = GL_STREAM_DRAW; break;
			case GraphicsBufferUseType::Read: result = GL_STREAM_READ; break;
			case GraphicsBufferUseType::Copy: result = GL_STREAM_COPY; break;
		} break;
		default: return false;
		}

		return true;
	}
}