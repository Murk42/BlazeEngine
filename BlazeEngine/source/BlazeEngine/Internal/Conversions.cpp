#include "pch.h"
#include "Conversions.h"

/*template<typename T>
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
}*/

namespace Blaze
{
	
	template<typename T>
	inline std::underlying_type_t<T> ToInteger(T value)
	{
		return static_cast<std::underlying_type_t<T>>(value);
	}
	uint BitmapColorFormatComponentCount(BitmapColorFormat format)
	{
		switch (format)
		{
		case Blaze::BitmapColorFormat::Red: return 1;
		case Blaze::BitmapColorFormat::RG: return 2;
		case Blaze::BitmapColorFormat::RGB:	return 3;
		case Blaze::BitmapColorFormat::RGBA: return 4;
		case Blaze::BitmapColorFormat::BGR:	return 3;
		case Blaze::BitmapColorFormat::BGRA: return 4;		
		default:
			Debug::Logger::LogError("Blaze Engine", "Invalid BitmapColorFormat enum value. The integer value was: " + StringParsing::Convert(ToInteger(format)));
			return 4;
		}		
	}

	uintMem BitmapColorComponentTypeSize(BitmapColorComponentType type)
	{
		switch (type)
		{
		case Blaze::BitmapColorComponentType::Int8: return sizeof(int8);
		case Blaze::BitmapColorComponentType::Uint8: return sizeof(uint8);			
		case Blaze::BitmapColorComponentType::Int16:	return sizeof(int);
		case Blaze::BitmapColorComponentType::Uint16: return sizeof(uint16);
		case Blaze::BitmapColorComponentType::Int32:	return sizeof(int32);
		case Blaze::BitmapColorComponentType::Uint32: return sizeof(uint32);
		case Blaze::BitmapColorComponentType::Float:	return sizeof(float);
		case Blaze::BitmapColorComponentType::Double: return sizeof(double);		
		default:
			Debug::Logger::LogError("Blaze Engine", "Invalid BitmapColorComponentType enum value. The integer value was: " + StringParsing::Convert(ToInteger(type)));
			return sizeof(uint8);
		}				
	}

	/*
	ILenum DevILColorFormat(BitmapColorFormat format)
	{
		switch (format)
		{
		case BitmapColorFormat::Red: return IL_LUMINANCE;
		case BitmapColorFormat::RG:	return IL_LUMINANCE_ALPHA;
		case BitmapColorFormat::RGB: return IL_RGB;
		case BitmapColorFormat::RGBA: return IL_RGBA;
		case BitmapColorFormat::BGR: return IL_BGR;
		case BitmapColorFormat::BGRA: return IL_BGRA;	
		default:
			Debug::Logger::LogError("Blaze Engine", "Invalid BitmapColorFormat enum value. The integer value was: " + StringParsing::Convert(ToInteger(format)));
			return IL_RGBA;
		}		
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
	SDL_PixelFormatEnum SDLPixelFormat(BitmapColorFormat format, Result& result)
	{
		switch (format)
		{
		case BitmapColorFormat::Red: throw;
		case BitmapColorFormat::RG:	throw;
		case BitmapColorFormat::RGB: return SDL_PIXELFORMAT_RGB24;
		case BitmapColorFormat::RGBA: return SDL_PIXELFORMAT_RGBA32;
		case BitmapColorFormat::BGR: return SDL_PIXELFORMAT_BGR24;
		case BitmapColorFormat::BGRA: return SDL_PIXELFORMAT_BGRA32;		
		}
		result += BLAZE_ERROR_LOG("Blaze Engine", "Invalid BitmapColorFormat enum value. The integer value was: " + StringParsing::Convert(ToInteger(format)));
		return std::numeric_limits<SDL_PixelFormatEnum>::max();
	}
	BitmapColorFormat DevILToBlazePixelFormat(ILenum format)
	{
		switch (format)
		{
		case IL_LUMINANCE: return BitmapColorFormat::Red;
		case IL_LUMINANCE_ALPHA: return BitmapColorFormat::RG;
		case IL_RGB: return BitmapColorFormat::RGB;
		case IL_RGBA: return BitmapColorFormat::RGBA;
		case IL_BGR: return BitmapColorFormat::BGR;
		case IL_BGRA: return BitmapColorFormat::BGRA;		
		default:
			Debug::Logger::LogError("Blaze Engine", "Unsupported/Invalid DevIL pixel format enum value. The integer value was: " + StringParsing::Convert(static_cast<uint>(format)));		
			return BitmapColorFormat::RGBA;
		}
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

	
	GLenum OpenGLFormatByInternalPixelFormat(GraphicsLibrary::TextureInternalPixelFormat format, Result& result)
	{
		switch (format)
		{
		case Blaze::GraphicsLibrary::TextureInternalPixelFormat::RGBA:				 return GL_RGBA;
		case Blaze::GraphicsLibrary::TextureInternalPixelFormat::RGB:				 return GL_RGB;
		case Blaze::GraphicsLibrary::TextureInternalPixelFormat::RG:				     return GL_RG;
		case Blaze::GraphicsLibrary::TextureInternalPixelFormat::R:				     return GL_RED;
		case Blaze::GraphicsLibrary::TextureInternalPixelFormat::RGBA8:				 return GL_RGBA;
		case Blaze::GraphicsLibrary::TextureInternalPixelFormat::RGB8:				 return GL_RGB;
		case Blaze::GraphicsLibrary::TextureInternalPixelFormat::RG8:				 return GL_RG;
		case Blaze::GraphicsLibrary::TextureInternalPixelFormat::R8:					 return GL_RED;
		case Blaze::GraphicsLibrary::TextureInternalPixelFormat::RGBA16:				 return GL_RGBA;
		case Blaze::GraphicsLibrary::TextureInternalPixelFormat::RGB16:				 return GL_RGB;
		case Blaze::GraphicsLibrary::TextureInternalPixelFormat::RG16:				 return GL_RG;
		case Blaze::GraphicsLibrary::TextureInternalPixelFormat::R16:				 return GL_RED;
		case Blaze::GraphicsLibrary::TextureInternalPixelFormat::RGBA16F:			 return GL_RGBA;
		case Blaze::GraphicsLibrary::TextureInternalPixelFormat::RGB16F:				 return GL_RGB;
		case Blaze::GraphicsLibrary::TextureInternalPixelFormat::RG16F:				 return GL_RG;
		case Blaze::GraphicsLibrary::TextureInternalPixelFormat::R16F:				 return GL_RED;
		case Blaze::GraphicsLibrary::TextureInternalPixelFormat::RGBA32F:			 return GL_RGBA;
		case Blaze::GraphicsLibrary::TextureInternalPixelFormat::RGB32F:				 return GL_RGB;
		case Blaze::GraphicsLibrary::TextureInternalPixelFormat::RG32F:				 return GL_RG;
		case Blaze::GraphicsLibrary::TextureInternalPixelFormat::R32F:				 return GL_RED;
		case Blaze::GraphicsLibrary::TextureInternalPixelFormat::RGBA8I:				 return GL_RGBA;
		case Blaze::GraphicsLibrary::TextureInternalPixelFormat::RGB8I:				 return GL_RGB;
		case Blaze::GraphicsLibrary::TextureInternalPixelFormat::RG8I:				 return GL_RG;
		case Blaze::GraphicsLibrary::TextureInternalPixelFormat::R8I:				 return GL_RED;
		case Blaze::GraphicsLibrary::TextureInternalPixelFormat::RGBA16I:			 return GL_RGBA;
		case Blaze::GraphicsLibrary::TextureInternalPixelFormat::RGB16I:				 return GL_RGB;
		case Blaze::GraphicsLibrary::TextureInternalPixelFormat::RG16I:				 return GL_RG;
		case Blaze::GraphicsLibrary::TextureInternalPixelFormat::R16I:				 return GL_RED;
		case Blaze::GraphicsLibrary::TextureInternalPixelFormat::RGBA32I:			 return GL_RGBA;
		case Blaze::GraphicsLibrary::TextureInternalPixelFormat::RGB32I:				 return GL_RGB;
		case Blaze::GraphicsLibrary::TextureInternalPixelFormat::RG32I:				 return GL_RG;
		case Blaze::GraphicsLibrary::TextureInternalPixelFormat::R32I:				 return GL_RED;
		case Blaze::GraphicsLibrary::TextureInternalPixelFormat::RGBA8UI:			 return GL_RGBA;
		case Blaze::GraphicsLibrary::TextureInternalPixelFormat::RGB8UI:				 return GL_RGB;
		case Blaze::GraphicsLibrary::TextureInternalPixelFormat::RG8UI:				 return GL_RG;
		case Blaze::GraphicsLibrary::TextureInternalPixelFormat::R8UI:				 return GL_RED;
		case Blaze::GraphicsLibrary::TextureInternalPixelFormat::RGBA16UI:			 return GL_RGBA;
		case Blaze::GraphicsLibrary::TextureInternalPixelFormat::RGB16UI:			 return GL_RGB;
		case Blaze::GraphicsLibrary::TextureInternalPixelFormat::RG16UI:				 return GL_RG;
		case Blaze::GraphicsLibrary::TextureInternalPixelFormat::R16UI:				 return GL_RED;
		case Blaze::GraphicsLibrary::TextureInternalPixelFormat::RGBA32UI:			 return GL_RGBA;
		case Blaze::GraphicsLibrary::TextureInternalPixelFormat::RGB32UI:			 return GL_RGB;
		case Blaze::GraphicsLibrary::TextureInternalPixelFormat::RG32UI:				 return GL_RG;
		case Blaze::GraphicsLibrary::TextureInternalPixelFormat::R32UI:				 return GL_RED;
		case Blaze::GraphicsLibrary::TextureInternalPixelFormat::Depth16:			 return GL_DEPTH_COMPONENT;
		case Blaze::GraphicsLibrary::TextureInternalPixelFormat::Depth24:			 return GL_DEPTH_COMPONENT;
		case Blaze::GraphicsLibrary::TextureInternalPixelFormat::Depth32F:			 return GL_DEPTH_COMPONENT;
		case Blaze::GraphicsLibrary::TextureInternalPixelFormat::Depth24Stencil8:	 return GL_DEPTH_STENCIL;
		case Blaze::GraphicsLibrary::TextureInternalPixelFormat::Depth32FStencil8:	 return GL_DEPTH_STENCIL;
		case Blaze::GraphicsLibrary::TextureInternalPixelFormat::Stencil8:			 return GL_STENCIL_INDEX;
		case Blaze::GraphicsLibrary::TextureInternalPixelFormat::R11F_G11F_B10F:		 return GL_RGB;		
		}
		result += BLAZE_ERROR_LOG("Blaze Engine", "Invalid TextureInternalPixelFormat enum value. The integer value was: " + StringParsing::Convert(ToInteger(format)));
		return std::numeric_limits<GLenum>::max();
	}
	GLenum OpenGLInternalPixelFormat(GraphicsLibrary::TextureInternalPixelFormat format, Result& result)
	{
		switch (format)
		{
		case Blaze::GraphicsLibrary::TextureInternalPixelFormat::RGBA:				 return GL_RGBA;
		case Blaze::GraphicsLibrary::TextureInternalPixelFormat::RGB:				 return GL_RGB;
		case Blaze::GraphicsLibrary::TextureInternalPixelFormat::RG:					 return GL_RG;
		case Blaze::GraphicsLibrary::TextureInternalPixelFormat::R:					 return GL_RED;
		case Blaze::GraphicsLibrary::TextureInternalPixelFormat::RGBA8:				 return GL_RGBA8;
		case Blaze::GraphicsLibrary::TextureInternalPixelFormat::RGB8:				 return GL_RGB8;
		case Blaze::GraphicsLibrary::TextureInternalPixelFormat::RG8:				 return GL_RG8;
		case Blaze::GraphicsLibrary::TextureInternalPixelFormat::R8:					 return GL_R8;
		case Blaze::GraphicsLibrary::TextureInternalPixelFormat::RGBA16:				 return GL_RGBA16;
		case Blaze::GraphicsLibrary::TextureInternalPixelFormat::RGB16:				 return GL_RGB16;
		case Blaze::GraphicsLibrary::TextureInternalPixelFormat::RG16:				 return GL_RG16;
		case Blaze::GraphicsLibrary::TextureInternalPixelFormat::R16:				 return GL_R16;
		case Blaze::GraphicsLibrary::TextureInternalPixelFormat::RGBA16F:			 return GL_RGBA16F;
		case Blaze::GraphicsLibrary::TextureInternalPixelFormat::RGB16F:				 return GL_RGB16F;
		case Blaze::GraphicsLibrary::TextureInternalPixelFormat::RG16F:				 return GL_RG16F;
		case Blaze::GraphicsLibrary::TextureInternalPixelFormat::R16F:				 return GL_R16F;
		case Blaze::GraphicsLibrary::TextureInternalPixelFormat::RGBA32F:			 return GL_RGBA32F;
		case Blaze::GraphicsLibrary::TextureInternalPixelFormat::RGB32F:				 return GL_RGB32F;
		case Blaze::GraphicsLibrary::TextureInternalPixelFormat::RG32F:				 return GL_RG32F;
		case Blaze::GraphicsLibrary::TextureInternalPixelFormat::R32F:				 return GL_R32F;
		case Blaze::GraphicsLibrary::TextureInternalPixelFormat::RGBA8I:				 return GL_RGBA8I;
		case Blaze::GraphicsLibrary::TextureInternalPixelFormat::RGB8I:				 return GL_RGB8I;
		case Blaze::GraphicsLibrary::TextureInternalPixelFormat::RG8I:				 return GL_RG8I;
		case Blaze::GraphicsLibrary::TextureInternalPixelFormat::R8I:				 return GL_R8I;
		case Blaze::GraphicsLibrary::TextureInternalPixelFormat::RGBA16I:			 return GL_RGBA16I;
		case Blaze::GraphicsLibrary::TextureInternalPixelFormat::RGB16I:				 return GL_RGB16I;
		case Blaze::GraphicsLibrary::TextureInternalPixelFormat::RG16I:				 return GL_RG16I;
		case Blaze::GraphicsLibrary::TextureInternalPixelFormat::R16I:				 return GL_R16I;
		case Blaze::GraphicsLibrary::TextureInternalPixelFormat::RGBA32I:			 return GL_RGBA32I;
		case Blaze::GraphicsLibrary::TextureInternalPixelFormat::RGB32I:				 return GL_RGB32I;
		case Blaze::GraphicsLibrary::TextureInternalPixelFormat::RG32I:				 return GL_RG32I;
		case Blaze::GraphicsLibrary::TextureInternalPixelFormat::R32I:				 return GL_R32I;
		case Blaze::GraphicsLibrary::TextureInternalPixelFormat::RGBA8UI:			 return GL_RGBA8UI;
		case Blaze::GraphicsLibrary::TextureInternalPixelFormat::RGB8UI:				 return GL_RGB8UI;
		case Blaze::GraphicsLibrary::TextureInternalPixelFormat::RG8UI:				 return GL_RG8UI;
		case Blaze::GraphicsLibrary::TextureInternalPixelFormat::R8UI:				 return GL_R8UI;
		case Blaze::GraphicsLibrary::TextureInternalPixelFormat::RGBA16UI:			 return GL_RGBA16UI;
		case Blaze::GraphicsLibrary::TextureInternalPixelFormat::RGB16UI:			 return GL_RGB16UI;
		case Blaze::GraphicsLibrary::TextureInternalPixelFormat::RG16UI:				 return GL_RG16UI;
		case Blaze::GraphicsLibrary::TextureInternalPixelFormat::R16UI:				 return GL_R16UI;
		case Blaze::GraphicsLibrary::TextureInternalPixelFormat::RGBA32UI:			 return GL_RGBA32UI;
		case Blaze::GraphicsLibrary::TextureInternalPixelFormat::RGB32UI:			 return GL_RGB32UI;
		case Blaze::GraphicsLibrary::TextureInternalPixelFormat::RG32UI:				 return GL_RG32UI;
		case Blaze::GraphicsLibrary::TextureInternalPixelFormat::R32UI:				 return GL_R32UI;
		case Blaze::GraphicsLibrary::TextureInternalPixelFormat::Depth16:			 return GL_DEPTH_COMPONENT16;
		case Blaze::GraphicsLibrary::TextureInternalPixelFormat::Depth24:			 return GL_DEPTH_COMPONENT24;
		case Blaze::GraphicsLibrary::TextureInternalPixelFormat::Depth32F:			 return GL_DEPTH_COMPONENT32F;
		case Blaze::GraphicsLibrary::TextureInternalPixelFormat::Depth24Stencil8:	 return GL_DEPTH24_STENCIL8;
		case Blaze::GraphicsLibrary::TextureInternalPixelFormat::Depth32FStencil8:	 return GL_DEPTH32F_STENCIL8;
		case Blaze::GraphicsLibrary::TextureInternalPixelFormat::Stencil8:			 return GL_STENCIL_INDEX;
		case Blaze::GraphicsLibrary::TextureInternalPixelFormat::R11F_G11F_B10F:		 return GL_R11F_G11F_B10F;		
		}
		result += BLAZE_ERROR_LOG("Blaze Engine", "Invalid TextureInternalPixelFormat enum value. The integer value was: " + StringParsing::Convert(ToInteger(format)));
		return std::numeric_limits<GLenum>::max();
	}
	GLenum OpenGLBufferInternalPixelFormat(GraphicsLibrary::TextureBufferInternalPixelFormat format, Result& result)
	{
		switch (format)
		{			
			case GraphicsLibrary::TextureBufferInternalPixelFormat::R8		: return GL_R8		;
			case GraphicsLibrary::TextureBufferInternalPixelFormat::R16		: return GL_R16		;
			case GraphicsLibrary::TextureBufferInternalPixelFormat::R16F		: return GL_R16F	;
			case GraphicsLibrary::TextureBufferInternalPixelFormat::R32F		: return GL_R32F	;
			case GraphicsLibrary::TextureBufferInternalPixelFormat::R8I		: return GL_R8I		;
			case GraphicsLibrary::TextureBufferInternalPixelFormat::R16I		: return GL_R16I	;
			case GraphicsLibrary::TextureBufferInternalPixelFormat::R32I		: return GL_R32I	;
			case GraphicsLibrary::TextureBufferInternalPixelFormat::R8UI		: return GL_R8UI	;
			case GraphicsLibrary::TextureBufferInternalPixelFormat::R16UI	: return GL_R16UI	;
			case GraphicsLibrary::TextureBufferInternalPixelFormat::R32UI	: return GL_R32UI	;
			case GraphicsLibrary::TextureBufferInternalPixelFormat::RG8		: return GL_RG8		;
			case GraphicsLibrary::TextureBufferInternalPixelFormat::RG16		: return GL_RG16	;
			case GraphicsLibrary::TextureBufferInternalPixelFormat::RG16F	: return GL_RG16F	;
			case GraphicsLibrary::TextureBufferInternalPixelFormat::RG32F	: return GL_RG32F	;
			case GraphicsLibrary::TextureBufferInternalPixelFormat::RG8I		: return GL_RG8I	;
			case GraphicsLibrary::TextureBufferInternalPixelFormat::RG16I	: return GL_RG16I	;
			case GraphicsLibrary::TextureBufferInternalPixelFormat::RG32I	: return GL_RG32I	;
			case GraphicsLibrary::TextureBufferInternalPixelFormat::RG8UI	: return GL_RG8UI	;
			case GraphicsLibrary::TextureBufferInternalPixelFormat::RG16UI	: return GL_RG16UI	;
			case GraphicsLibrary::TextureBufferInternalPixelFormat::RG32UI	: return GL_RG32UI	;
			case GraphicsLibrary::TextureBufferInternalPixelFormat::RGB32F	: return GL_RGB32F	;
			case GraphicsLibrary::TextureBufferInternalPixelFormat::RGB32I	: return GL_RGB32I	;
			case GraphicsLibrary::TextureBufferInternalPixelFormat::RGB32UI	: return GL_RGB32UI ;
			case GraphicsLibrary::TextureBufferInternalPixelFormat::RGBA8	: return GL_RGBA8	;
			case GraphicsLibrary::TextureBufferInternalPixelFormat::RGBA16	: return GL_RGBA16	;
			case GraphicsLibrary::TextureBufferInternalPixelFormat::RGBA16F	: return GL_RGBA16F ;
			case GraphicsLibrary::TextureBufferInternalPixelFormat::RGBA32F	: return GL_RGBA32F ;
			case GraphicsLibrary::TextureBufferInternalPixelFormat::RGBA8I	: return GL_RGBA8I	;
			case GraphicsLibrary::TextureBufferInternalPixelFormat::RGBA16I	: return GL_RGBA16I ;
			case GraphicsLibrary::TextureBufferInternalPixelFormat::RGBA32I	: return GL_RGBA32I ;
			case GraphicsLibrary::TextureBufferInternalPixelFormat::RGBA8UI	: return GL_RGBA8UI ;
			case GraphicsLibrary::TextureBufferInternalPixelFormat::RGBA16UI	: return GL_RGBA16UI;
			case GraphicsLibrary::TextureBufferInternalPixelFormat::RGBA32UI: return GL_RGBA32UI; 			
		}
		result += BLAZE_ERROR_LOG("Blaze Engine", "Invalid TextureBufferInternalPixelFormat enum value. The integer value was: " + StringParsing::Convert(ToInteger(format)));
		return std::numeric_limits<GLenum>::max();
	}
	
	ILenum DevILColorComponentType(BitmapColorComponentType type)
	{
		switch (type)
		{
		case Blaze::BitmapColorComponentType::Int8: return IL_BYTE;
		case Blaze::BitmapColorComponentType::Uint8:	return IL_UNSIGNED_BYTE;
		case Blaze::BitmapColorComponentType::Int16:	return IL_SHORT;
		case Blaze::BitmapColorComponentType::Uint16: return IL_UNSIGNED_SHORT;
		case Blaze::BitmapColorComponentType::Int32:	return IL_INT;
		case Blaze::BitmapColorComponentType::Uint32: return IL_UNSIGNED_INT;
		case Blaze::BitmapColorComponentType::Float:	return IL_FLOAT;
		case Blaze::BitmapColorComponentType::Double: return IL_DOUBLE;		
		default:
			Debug::Logger::LogError("Blaze Engine", "Invalid BitmapColorComponentType enum value. The integer value was: " + StringParsing::Convert(ToInteger(type)));
			return IL_UNSIGNED_BYTE;
		}
	}
	
	GLenum OpenGLPixelType(BitmapColorComponentType type, Result& result)
	{
		switch (type)
		{
		case Blaze::BitmapColorComponentType::Int8: return GL_BYTE;
		case Blaze::BitmapColorComponentType::Uint8:	return GL_UNSIGNED_BYTE;
		case Blaze::BitmapColorComponentType::Int16:	return GL_SHORT;
		case Blaze::BitmapColorComponentType::Uint16: return GL_UNSIGNED_SHORT;
		case Blaze::BitmapColorComponentType::Int32:	return GL_INT;
		case Blaze::BitmapColorComponentType::Uint32: return GL_UNSIGNED_INT;
		case Blaze::BitmapColorComponentType::Float:	return GL_FLOAT;
		case Blaze::BitmapColorComponentType::Double: return GL_DOUBLE;		
		}

		result += BLAZE_ERROR_LOG("Blaze Engine", "Invalid BitmapColorComponentType enum value. The integer value was: " + StringParsing::Convert(ToInteger(type)));
		return std::numeric_limits<GLenum>::max();
	}
	
	BitmapColorComponentType DevILToBlazePixelType(ILenum type)
	{
		switch (type)
		{
		case IL_BYTE: return Blaze::BitmapColorComponentType::Int8;
		case IL_UNSIGNED_BYTE: return Blaze::BitmapColorComponentType::Uint8;
		case IL_SHORT: return Blaze::BitmapColorComponentType::Int16;
		case IL_UNSIGNED_SHORT: return Blaze::BitmapColorComponentType::Uint16;
		case IL_INT: return Blaze::BitmapColorComponentType::Int32;
		case IL_UNSIGNED_INT: return Blaze::BitmapColorComponentType::Uint32;
		case IL_FLOAT: return Blaze::BitmapColorComponentType::Float;
		case IL_DOUBLE: return Blaze::BitmapColorComponentType::Double;		
		default:
			Debug::Logger::LogError("Blaze Engine", "Unsupported/Invalid DevIL pixel type enum value. The integer value was: " + StringParsing::Convert(static_cast<uint>(type)));
			return BitmapColorComponentType::Uint8;
		}
	}
	
	BitmapColorComponentType OpenGLToBlazePixelType(GLenum type, Result& result)
	{
		switch (type)
		{
		case GL_BYTE: return Blaze::BitmapColorComponentType::Int8;
		case GL_UNSIGNED_BYTE: return Blaze::BitmapColorComponentType::Uint8;
		case GL_SHORT: return Blaze::BitmapColorComponentType::Int16;
		case GL_UNSIGNED_SHORT: return Blaze::BitmapColorComponentType::Uint16;
		case GL_INT: return Blaze::BitmapColorComponentType::Int32;
		case GL_UNSIGNED_INT: return Blaze::BitmapColorComponentType::Uint32;
		case GL_FLOAT: return Blaze::BitmapColorComponentType::Float;
		case GL_DOUBLE: return Blaze::BitmapColorComponentType::Double;		
		}
		result += BLAZE_ERROR_LOG("Blaze Engine", "Unsupported/Invalid OpenGL pixel type enum value. The integer value was: " + StringParsing::Convert(static_cast<uint>(type)));
		return std::numeric_limits<BitmapColorComponentType>::max();
	}
	GLenum OpenGLTextureMinSampling(GraphicsLibrary::TextureSampling min, GraphicsLibrary::TextureSampling mip, bool mipmaps, Result& result)
	{
		if (mipmaps)
		{
			if (mip == GraphicsLibrary::TextureSampling::Nearest)
				if (min == GraphicsLibrary::TextureSampling::Nearest)
					return GL_NEAREST_MIPMAP_NEAREST;
				else if (min == GraphicsLibrary::TextureSampling::Linear)
					return GL_LINEAR_MIPMAP_NEAREST;
				else
				{
					result += BLAZE_ERROR_LOG("Blaze Engine", "Invalid TextureSampling enum value. The integer value was: " + StringParsing::Convert(ToInteger(min)));
					return std::numeric_limits<GLenum>::max();
				}
			else if (mip == GraphicsLibrary::TextureSampling::Linear)
				if (min == GraphicsLibrary::TextureSampling::Nearest)
					return GL_NEAREST_MIPMAP_LINEAR;
				else if (min == GraphicsLibrary::TextureSampling::Linear)
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
			if (min == GraphicsLibrary::TextureSampling::Nearest)
				return GL_NEAREST;
			else if (min == GraphicsLibrary::TextureSampling::Linear)
				return GL_LINEAR;
			else
			{
				result += BLAZE_ERROR_LOG("Blaze Engine", "Invalid TextureSampling enum value. The integer value was: " + StringParsing::Convert(ToInteger(min)));
				return std::numeric_limits<GLenum>::max();
			}
	}
	GLenum OpenGLTextureMagSampling(GraphicsLibrary::TextureSampling sampling, Result& result)
	{
		switch (sampling)
		{
		case Blaze::GraphicsLibrary::TextureSampling::Nearest: return GL_NEAREST;
		case Blaze::GraphicsLibrary::TextureSampling::Linear: return GL_LINEAR;		
		}
		result += BLAZE_ERROR_LOG("Blaze Engine", "Invalid TextureSampling enum value. The integer value was: " + StringParsing::Convert(ToInteger(sampling)));
		return std::numeric_limits<GLenum>::max();
	}	
	GLenum OpenGLTextureWrapping(GraphicsLibrary::TextureWrapping wrapping, Result& result)
	{
		switch (wrapping)
		{
		case Blaze::GraphicsLibrary::TextureWrapping::ClampToBorder: return GL_CLAMP_TO_BORDER;			
		case Blaze::GraphicsLibrary::TextureWrapping::ClampToEdge: return GL_CLAMP_TO_EDGE;			
		case Blaze::GraphicsLibrary::TextureWrapping::MirroredRepeat: return GL_MIRRORED_REPEAT;			
		case Blaze::GraphicsLibrary::TextureWrapping::Repeat: return GL_REPEAT;					
		}
		result += BLAZE_ERROR_LOG("Blaze Engine", "Invalid TextureWrapping enum value. The integer value was: " + StringParsing::Convert(ToInteger(wrapping)));
		return std::numeric_limits<GLenum>::max();
	}
	GraphicsLibrary::TextureInternalPixelFormat MapInternalTexturePixelFormat(BitmapColorFormat format, Result& result)
	{		
		switch (format)
		{
		case BitmapColorFormat::Red: return GraphicsLibrary::TextureInternalPixelFormat::R32F;
		case BitmapColorFormat::RG: return GraphicsLibrary::TextureInternalPixelFormat::RG32F;
		case BitmapColorFormat::RGB: return GraphicsLibrary::TextureInternalPixelFormat::RGB32F;
		case BitmapColorFormat::RGBA: return GraphicsLibrary::TextureInternalPixelFormat::RGBA32F;
		case BitmapColorFormat::BGR: return GraphicsLibrary::TextureInternalPixelFormat::RGB32F;
		case BitmapColorFormat::BGRA: return GraphicsLibrary::TextureInternalPixelFormat::RGBA32F;		
		}
		result += BLAZE_ERROR_LOG("Blaze Engine", "Invalid BitmapColorFormat enum value. The integer value was: " + StringParsing::Convert(ToInteger(format)));
		return std::numeric_limits<GraphicsLibrary::TextureInternalPixelFormat>::max();
	}
	BitmapColorFormat MapInternalTexturePixelFormat(GraphicsLibrary::TextureInternalPixelFormat format, Result& result)
	{
		switch (format)
		{
		case Blaze::OpenGL::TextureInternalPixelFormat::RGBA: return BitmapColorFormat::RGBA;
		case Blaze::OpenGL::TextureInternalPixelFormat::RGB:  return BitmapColorFormat::RGB;			
		case Blaze::OpenGL::TextureInternalPixelFormat::RG:   return BitmapColorFormat::RG;			
		case Blaze::OpenGL::TextureInternalPixelFormat::R:    return BitmapColorFormat::Red;
		case Blaze::OpenGL::TextureInternalPixelFormat::RGBA8:  return BitmapColorFormat::RGBA;
		case Blaze::OpenGL::TextureInternalPixelFormat::RGB8:	return BitmapColorFormat::RGB;
		case Blaze::OpenGL::TextureInternalPixelFormat::RG8:	return BitmapColorFormat::RG;
		case Blaze::OpenGL::TextureInternalPixelFormat::R8:		return BitmapColorFormat::Red;
		case Blaze::OpenGL::TextureInternalPixelFormat::RGBA16:	return BitmapColorFormat::RGBA;
		case Blaze::OpenGL::TextureInternalPixelFormat::RGB16:	return BitmapColorFormat::RGB;
		case Blaze::OpenGL::TextureInternalPixelFormat::RG16:	return BitmapColorFormat::RG;
		case Blaze::OpenGL::TextureInternalPixelFormat::R16:	return BitmapColorFormat::Red;
		case Blaze::OpenGL::TextureInternalPixelFormat::RGBA16F:return BitmapColorFormat::RGBA;
		case Blaze::OpenGL::TextureInternalPixelFormat::RGB16F:	return BitmapColorFormat::RGB;
		case Blaze::OpenGL::TextureInternalPixelFormat::RG16F:	return BitmapColorFormat::RG;
		case Blaze::OpenGL::TextureInternalPixelFormat::R16F:	return BitmapColorFormat::Red;
		case Blaze::OpenGL::TextureInternalPixelFormat::RGBA32F:return BitmapColorFormat::RGBA;
		case Blaze::OpenGL::TextureInternalPixelFormat::RGB32F:	return BitmapColorFormat::RGB;
		case Blaze::OpenGL::TextureInternalPixelFormat::RG32F:	return BitmapColorFormat::RG;
		case Blaze::OpenGL::TextureInternalPixelFormat::R32F:	return BitmapColorFormat::Red;
		case Blaze::OpenGL::TextureInternalPixelFormat::RGBA8I:	return BitmapColorFormat::RGBA;
		case Blaze::OpenGL::TextureInternalPixelFormat::RGB8I:	return BitmapColorFormat::RGB;
		case Blaze::OpenGL::TextureInternalPixelFormat::RG8I:	return BitmapColorFormat::RG;
		case Blaze::OpenGL::TextureInternalPixelFormat::R8I:	return BitmapColorFormat::Red;
		case Blaze::OpenGL::TextureInternalPixelFormat::RGBA16I:return BitmapColorFormat::RGBA;
		case Blaze::OpenGL::TextureInternalPixelFormat::RGB16I:	return BitmapColorFormat::RGB;
		case Blaze::OpenGL::TextureInternalPixelFormat::RG16I:	return BitmapColorFormat::RG;
		case Blaze::OpenGL::TextureInternalPixelFormat::R16I:	return BitmapColorFormat::Red;
		case Blaze::OpenGL::TextureInternalPixelFormat::RGBA32I:return BitmapColorFormat::RGBA;
		case Blaze::OpenGL::TextureInternalPixelFormat::RGB32I:	return BitmapColorFormat::RGB;
		case Blaze::OpenGL::TextureInternalPixelFormat::RG32I:	return BitmapColorFormat::RG;
		case Blaze::OpenGL::TextureInternalPixelFormat::R32I:	return BitmapColorFormat::Red;
		case Blaze::OpenGL::TextureInternalPixelFormat::RGBA8UI:return BitmapColorFormat::RGBA;
		case Blaze::OpenGL::TextureInternalPixelFormat::RGB8UI:	return BitmapColorFormat::RGB;
		case Blaze::OpenGL::TextureInternalPixelFormat::RG8UI:	return BitmapColorFormat::RG;
		case Blaze::OpenGL::TextureInternalPixelFormat::R8UI:	return BitmapColorFormat::Red;
		case Blaze::OpenGL::TextureInternalPixelFormat::RGBA16UI:return BitmapColorFormat::RGBA;
		case Blaze::OpenGL::TextureInternalPixelFormat::RGB16UI:return BitmapColorFormat::RGB;
		case Blaze::OpenGL::TextureInternalPixelFormat::RG16UI:	return BitmapColorFormat::RG;
		case Blaze::OpenGL::TextureInternalPixelFormat::R16UI:	 return BitmapColorFormat::Red;
		case Blaze::OpenGL::TextureInternalPixelFormat::RGBA32UI:return BitmapColorFormat::RGBA;
		case Blaze::OpenGL::TextureInternalPixelFormat::RGB32UI:return BitmapColorFormat::RGB;
		case Blaze::OpenGL::TextureInternalPixelFormat::RG32UI:	return BitmapColorFormat::RG;
		case Blaze::OpenGL::TextureInternalPixelFormat::R32UI:	return BitmapColorFormat::Red;
		case Blaze::OpenGL::TextureInternalPixelFormat::Depth16:			
		case Blaze::OpenGL::TextureInternalPixelFormat::Depth24:		
		case Blaze::OpenGL::TextureInternalPixelFormat::Depth32F:			
		case Blaze::OpenGL::TextureInternalPixelFormat::Depth24Stencil8:			
		case Blaze::OpenGL::TextureInternalPixelFormat::Depth32FStencil8:			
		case Blaze::OpenGL::TextureInternalPixelFormat::Stencil8: 
			Debug::Logger::LogError("Blaze Engine", "Unsupported TextueInternalPixelFormat enum value");
			return BitmapColorFormat::Red;
		case Blaze::OpenGL::TextureInternalPixelFormat::R11F_G11F_B10F:	return BitmapColorFormat::RGB;
		default:
			Debug::Logger::LogError("Blaze Engine", "Invalid TextueInternalPixelFormat enum value");
			return BitmapColorFormat::Red;
			break;
		}
	}
	GLenum OpenGLVertexAttributeType(GraphicsLibrary::VertexAttributeType type, Result& result)
	{		
		switch (type)
		{
		case Blaze::GraphicsLibrary::VertexAttributeType::Int8: return GL_BYTE;
		case Blaze::GraphicsLibrary::VertexAttributeType::Uint8: return GL_UNSIGNED_BYTE;
		case Blaze::GraphicsLibrary::VertexAttributeType::Int16: return GL_SHORT;
		case Blaze::GraphicsLibrary::VertexAttributeType::Uint16: return GL_UNSIGNED_SHORT;
		case Blaze::GraphicsLibrary::VertexAttributeType::Int32: return GL_INT;
		case Blaze::GraphicsLibrary::VertexAttributeType::Uint32: return GL_UNSIGNED_INT;
		case Blaze::GraphicsLibrary::VertexAttributeType::Float: return GL_FLOAT;
		case Blaze::GraphicsLibrary::VertexAttributeType::Double: return GL_DOUBLE;
		}
		result += BLAZE_FATAL_LOG("Blaze Engine", "Invalid VertexAttributeType enum value. The integer value was: " + StringParsing::Convert(ToInteger(type)));
		return std::numeric_limits<GLenum>::max();		
	}
	GLenum OpenGLFramebufferAttachment(GraphicsLibrary::FramebufferAttachment attachment, Result& result)
	{
		switch (attachment)
		{
		case Blaze::GraphicsLibrary::FramebufferAttachment::DepthStencil: return GL_DEPTH_STENCIL_ATTACHMENT;
		case Blaze::GraphicsLibrary::FramebufferAttachment::Stencil:	return GL_STENCIL_ATTACHMENT;
		case Blaze::GraphicsLibrary::FramebufferAttachment::Depth: return GL_DEPTH_ATTACHMENT;		
		}
		result += BLAZE_ERROR_LOG("Blaze Engine", "Invalid FramebufferAttachment enum value. The integer value was: " + StringParsing::Convert(ToInteger(attachment)));
		return std::numeric_limits<GLenum>::max();
	}
		
	GLenum OpenGLBufferMappingAccess(GraphicsLibrary::ImmutableGraphicsBufferMapAccess mapping, Result& result)
	{
		GLenum out;

		if (!BitRemap(mapping, out, {
				GraphicsLibrary::ImmutableGraphicsBufferMapAccess::Read,
				GraphicsLibrary::ImmutableGraphicsBufferMapAccess::Write
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
	GLenum OpenGLBufferMappingType(GraphicsLibrary::ImmutableGraphicsBufferMapType type, Result& result)
	{
		GLenum out;

		if (!BitRemap(type, out, {
				GraphicsLibrary::ImmutableGraphicsBufferMapType::PersistentCoherent,
				GraphicsLibrary::ImmutableGraphicsBufferMapType::PersistentUncoherent
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
	GLenum OpenGLBufferMappingOptions(GraphicsLibrary::ImmutableGraphicsBufferMapOptions options, Result& result)
	{
		GLenum out;

		if (!BitRemap(options, out, {
				GraphicsLibrary::ImmutableGraphicsBufferMapOptions::ExplicitFlush,
				GraphicsLibrary::ImmutableGraphicsBufferMapOptions::InvalidateBuffer,
				GraphicsLibrary::ImmutableGraphicsBufferMapOptions::InvalidateRange,
				GraphicsLibrary::ImmutableGraphicsBufferMapOptions::Unsynchronized,				
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
	GLenum OpenGLMutableBufferUsage(GraphicsLibrary::MutableGraphicsBufferUseFrequency frequency, MutableGraphicsBufferUseType use, Result& result)
	{
		switch (frequency)
		{
		case Blaze::GraphicsLibrary::MutableGraphicsBufferUseFrequency::Static:	switch (use) {
			case Blaze::MutableGraphicsBufferUseType::Draw: return GL_STATIC_DRAW;				
			case Blaze::MutableGraphicsBufferUseType::Read: return GL_STATIC_READ;				
			case Blaze::MutableGraphicsBufferUseType::Copy: return GL_STATIC_COPY; }
		case Blaze::GraphicsLibrary::MutableGraphicsBufferUseFrequency::Dynamic:	switch (use) {
			case Blaze::MutableGraphicsBufferUseType::Draw: return GL_DYNAMIC_DRAW;				
			case Blaze::MutableGraphicsBufferUseType::Read: return GL_DYNAMIC_READ;				
			case Blaze::MutableGraphicsBufferUseType::Copy: return GL_DYNAMIC_COPY; }
		case Blaze::GraphicsLibrary::MutableGraphicsBufferUseFrequency::Stream:	switch (use) {
			case Blaze::MutableGraphicsBufferUseType::Draw: return GL_STREAM_DRAW;				
			case Blaze::MutableGraphicsBufferUseType::Read: return GL_STREAM_READ;				
			case Blaze::MutableGraphicsBufferUseType::Copy: return GL_STREAM_COPY; }
		default:
			return BLAZE_ERROR_RESULT("Blaze Engine", "Invalid MutableGraphicsBufferUseFrequency enum value. The integer value was: " + StringParsing::Convert(ToInteger(frequency)));
			return std::numeric_limits<GLenum>::max();
			break;
		}		

		return BLAZE_ERROR_RESULT("Blaze Engine", "Engine error. Invalid MutableGraphicsBufferUseType enum value. The integer value was: " + StringParsing::Convert(ToInteger(use)));
		return std::numeric_limits<GLenum>::max();
	}
	*/

	DisplayPixelFormat BlazeDisplayPixelFormat(uint32 format, Result& result)
	{
		switch (format)
		{
		case SDL_PIXELFORMAT_RGB332:		return Blaze::DisplayPixelFormat::RGB332;
		case SDL_PIXELFORMAT_RGB444:		return Blaze::DisplayPixelFormat::RGB444;
		case SDL_PIXELFORMAT_RGB555:		return Blaze::DisplayPixelFormat::RGB555;
		case SDL_PIXELFORMAT_BGR555:		return Blaze::DisplayPixelFormat::BGR555;
		case SDL_PIXELFORMAT_ARGB4444:		return Blaze::DisplayPixelFormat::ARGB4444;
		case SDL_PIXELFORMAT_RGBA4444:		return Blaze::DisplayPixelFormat::RGBA4444;
		case SDL_PIXELFORMAT_ABGR4444:		return Blaze::DisplayPixelFormat::ABGR4444;
		case SDL_PIXELFORMAT_BGRA4444:		return Blaze::DisplayPixelFormat::BGRA4444;
		case SDL_PIXELFORMAT_ARGB1555:		return Blaze::DisplayPixelFormat::ARGB1555;
		case SDL_PIXELFORMAT_RGBA5551:		return Blaze::DisplayPixelFormat::RGBA5551;
		case SDL_PIXELFORMAT_ABGR1555:		return Blaze::DisplayPixelFormat::ABGR1555;
		case SDL_PIXELFORMAT_BGRA5551:		return Blaze::DisplayPixelFormat::BGRA5551;
		case SDL_PIXELFORMAT_RGB565:		return Blaze::DisplayPixelFormat::RGB565;
		case SDL_PIXELFORMAT_BGR565:		return Blaze::DisplayPixelFormat::BGR565;
		case SDL_PIXELFORMAT_RGB24:			return Blaze::DisplayPixelFormat::RGB24;
		case SDL_PIXELFORMAT_BGR24:			return Blaze::DisplayPixelFormat::BGR24;
		case SDL_PIXELFORMAT_RGB888:		return Blaze::DisplayPixelFormat::RGB888;
		case SDL_PIXELFORMAT_RGBX8888:		return Blaze::DisplayPixelFormat::RGBX8888;
		case SDL_PIXELFORMAT_BGR888:		return Blaze::DisplayPixelFormat::BGR888;
		case SDL_PIXELFORMAT_BGRX8888:		return Blaze::DisplayPixelFormat::BGRX8888;
		case SDL_PIXELFORMAT_ARGB8888:		return Blaze::DisplayPixelFormat::ARGB8888;
		case SDL_PIXELFORMAT_RGBA8888:		return Blaze::DisplayPixelFormat::RGBA8888;
		case SDL_PIXELFORMAT_ABGR8888:		return Blaze::DisplayPixelFormat::ABGR8888;
		case SDL_PIXELFORMAT_BGRA8888:		return Blaze::DisplayPixelFormat::BGRA8888;
		case SDL_PIXELFORMAT_ARGB2101010:	return Blaze::DisplayPixelFormat::ARGB2101010;
		}
		result += BLAZE_ERROR_LOG("Blaze Engine", "Unsupported/Invalid SDL pixel format enum value. The integer value was: " + StringParsing::Convert(format));
		return std::numeric_limits<DisplayPixelFormat>::max();
	}	
	
	SDL_PixelFormatEnum SDLDisplayPixelFormat(DisplayPixelFormat format, Result& result)
	{
		switch (format)
		{
		case Blaze::DisplayPixelFormat::RGB332:		return SDL_PIXELFORMAT_RGB332;
		case Blaze::DisplayPixelFormat::RGB444:		return SDL_PIXELFORMAT_RGB444;
		case Blaze::DisplayPixelFormat::RGB555:		return SDL_PIXELFORMAT_RGB555;
		case Blaze::DisplayPixelFormat::BGR555:		return SDL_PIXELFORMAT_BGR555;
		case Blaze::DisplayPixelFormat::ARGB4444:		return SDL_PIXELFORMAT_ARGB4444;
		case Blaze::DisplayPixelFormat::RGBA4444:		return SDL_PIXELFORMAT_RGBA4444;
		case Blaze::DisplayPixelFormat::ABGR4444:		return SDL_PIXELFORMAT_ABGR4444;
		case Blaze::DisplayPixelFormat::BGRA4444:		return SDL_PIXELFORMAT_BGRA4444;
		case Blaze::DisplayPixelFormat::ARGB1555:		return SDL_PIXELFORMAT_ARGB1555;
		case Blaze::DisplayPixelFormat::RGBA5551:		return SDL_PIXELFORMAT_RGBA5551;
		case Blaze::DisplayPixelFormat::ABGR1555:		return SDL_PIXELFORMAT_ABGR1555;
		case Blaze::DisplayPixelFormat::BGRA5551:		return SDL_PIXELFORMAT_BGRA5551;
		case Blaze::DisplayPixelFormat::RGB565:		return SDL_PIXELFORMAT_RGB565;
		case Blaze::DisplayPixelFormat::BGR565:		return SDL_PIXELFORMAT_BGR565;
		case Blaze::DisplayPixelFormat::RGB24:		return SDL_PIXELFORMAT_RGB24;
		case Blaze::DisplayPixelFormat::BGR24:		return SDL_PIXELFORMAT_BGR24;
		case Blaze::DisplayPixelFormat::RGB888:		return SDL_PIXELFORMAT_RGB888;
		case Blaze::DisplayPixelFormat::RGBX8888:		return SDL_PIXELFORMAT_RGBX8888;
		case Blaze::DisplayPixelFormat::BGR888:		return SDL_PIXELFORMAT_BGR888;
		case Blaze::DisplayPixelFormat::BGRX8888:		return SDL_PIXELFORMAT_BGRX8888;
		case Blaze::DisplayPixelFormat::ARGB8888:		return SDL_PIXELFORMAT_ARGB8888;
		case Blaze::DisplayPixelFormat::RGBA8888:		return SDL_PIXELFORMAT_RGBA8888;
		case Blaze::DisplayPixelFormat::ABGR8888:		return SDL_PIXELFORMAT_ABGR8888;
		case Blaze::DisplayPixelFormat::BGRA8888:		return SDL_PIXELFORMAT_BGRA8888;
		case Blaze::DisplayPixelFormat::ARGB2101010:	return SDL_PIXELFORMAT_ARGB2101010;
		}
		result += BLAZE_ERROR_LOG("Blaze Engine", "Invalid DisplayPixelFormat enum value. The integer value was: " + StringParsing::Convert(ToInteger(format)));
		return std::numeric_limits<SDL_PixelFormatEnum>::max();
	}	
}

/*
GL_RGBA8
GL_RGB8
GL_RG8
GL_R8

GL_RGBA16
GL_RGB16
GL_RG16
GL_R16

GL_RGBA16F
GL_RGB16F
GL_RG16F
GL_R16F

GL_RGBA32F
GL_RGB32F
GL_RG32F
GL_R32F

GL_RGBA8I
GL_RGB8I
GL_RG8I
GL_R8I

GL_RGBA16I
GL_RGB16I
GL_RG16I
GL_R16I

GL_RGBA32I
GL_RGB32I
GL_RG32I
GL_R32I

GL_RGBA8UI
GL_RGB8UI
GL_RG8UI
GL_R8UI

GL_RGBA16UI
GL_RGB16UI
GL_RG16UI
GL_R16UI

GL_RGBA32UI
GL_RGB32UI
GL_RG32UI
GL_R32UI

GL_DEPTH_COMPONENT16
GL_DEPTH_COMPONENT24
GL_DEPTH_COMPONENT32F

GL_DEPTH24_STENCIL8
GL_DEPTH32F_STENCIL8

GL_STENCIL_INDEX8
*/