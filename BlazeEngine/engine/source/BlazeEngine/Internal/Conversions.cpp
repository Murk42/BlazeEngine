#include "Conversions.h"

namespace Blaze
{
	unsigned GetFormatDepth(BitmapPixelFormat format)
	{
		switch (format)
		{
		case Blaze::BitmapPixelFormat::Red: return 1;
		case Blaze::BitmapPixelFormat::RG: return 2;
		case Blaze::BitmapPixelFormat::RGB:	return 3;
		case Blaze::BitmapPixelFormat::RGBA: return 4;
		case Blaze::BitmapPixelFormat::BGR:	return 3;
		case Blaze::BitmapPixelFormat::BGRA: return 4;
		default: return 0;
		}
	}

	unsigned PixelTypeSize(BitmapPixelType type)
	{
		switch (type)
		{
		case Blaze::BitmapPixelType::Int8: return sizeof(int8);
		case Blaze::BitmapPixelType::Uint8: return sizeof(uint8);			
		case Blaze::BitmapPixelType::Int16:	return sizeof(int);
		case Blaze::BitmapPixelType::Uint16: return sizeof(uint16);
		case Blaze::BitmapPixelType::Int32:	return sizeof(int32);
		case Blaze::BitmapPixelType::Uint32: return sizeof(uint32);
		case Blaze::BitmapPixelType::Float:	return sizeof(float);
		case Blaze::BitmapPixelType::Double: return sizeof(double);
		default: return 0;
		}
	}

	ILenum DevILPixelFormat(BitmapPixelFormat format)
	{
		switch (format)
		{
		case BitmapPixelFormat::Red: return IL_LUMINANCE;
		case BitmapPixelFormat::RG:	return IL_LUMINANCE_ALPHA;
		case BitmapPixelFormat::RGB: return IL_RGB;
		case BitmapPixelFormat::RGBA: return IL_RGBA;
		case BitmapPixelFormat::BGR: return IL_BGR;
		case BitmapPixelFormat::BGRA: return IL_BGRA;
		default: return 0;
		}
	}
	GLenum OpenGLPixelFormat(BitmapPixelFormat format)
	{
		switch (format)
		{
		case BitmapPixelFormat::Red: return GL_RED;
		case BitmapPixelFormat::RG:	return GL_RG;
		case BitmapPixelFormat::RGB: return GL_RGB;
		case BitmapPixelFormat::RGBA: return GL_RGBA;
		case BitmapPixelFormat::BGR: return GL_BGR;
		case BitmapPixelFormat::BGRA: return GL_BGRA;
		default: return 0;
		}
	}
	BitmapPixelFormat DevILToBlazePixelFormat(ILenum format)
	{
		switch (format)
		{
		case IL_LUMINANCE: return BitmapPixelFormat::Red;
		case IL_LUMINANCE_ALPHA: return BitmapPixelFormat::RG;
		case IL_RGB: return BitmapPixelFormat::RGB;
		case IL_RGBA: return BitmapPixelFormat::RGBA;
		case IL_BGR: return BitmapPixelFormat::BGR;
		case IL_BGRA: return BitmapPixelFormat::BGRA;
		}
	}
	BitmapPixelFormat OepnGLToBlazePixelFormat(GLenum format)
	{
		switch (format)
		{
		case GL_LUMINANCE: return BitmapPixelFormat::Red;
		case GL_LUMINANCE_ALPHA: return BitmapPixelFormat::RG;
		case GL_RGB: return BitmapPixelFormat::RGB;
		case GL_RGBA: return BitmapPixelFormat::RGBA;
		case GL_BGR: return BitmapPixelFormat::BGR;
		case GL_BGRA: return BitmapPixelFormat::BGRA;
		}
	}

	GLenum OpenGLInternalPixelFormat(Graphics::Core::TextureInternalPixelFormat format)
	{
		switch (format)
		{
		case Blaze::OpenGL::TextureInternalPixelFormat::RGBA8:				 return GL_RGBA8;
		case Blaze::OpenGL::TextureInternalPixelFormat::RGB8:				 return GL_RGB8;
		case Blaze::OpenGL::TextureInternalPixelFormat::RG8:				 return GL_RG8;
		case Blaze::OpenGL::TextureInternalPixelFormat::R8:					 return GL_R8;
		case Blaze::OpenGL::TextureInternalPixelFormat::RGBA16:				 return GL_RGBA16;
		case Blaze::OpenGL::TextureInternalPixelFormat::RGB16:				 return GL_RGB16;
		case Blaze::OpenGL::TextureInternalPixelFormat::RG16:				 return GL_RG16;
		case Blaze::OpenGL::TextureInternalPixelFormat::R16:				 return GL_R16;
		case Blaze::OpenGL::TextureInternalPixelFormat::RGBA16F:			 return GL_RGBA16F;
		case Blaze::OpenGL::TextureInternalPixelFormat::RGB16F:				 return GL_RGB16F;
		case Blaze::OpenGL::TextureInternalPixelFormat::RG16F:				 return GL_RG16F;
		case Blaze::OpenGL::TextureInternalPixelFormat::R16F:				 return GL_R16F;
		case Blaze::OpenGL::TextureInternalPixelFormat::RGBA32F:			 return GL_RGBA32F;
		case Blaze::OpenGL::TextureInternalPixelFormat::RGB32F:				 return GL_RGB32F;
		case Blaze::OpenGL::TextureInternalPixelFormat::RG32F:				 return GL_RG32F;
		case Blaze::OpenGL::TextureInternalPixelFormat::R32F:				 return GL_R32F;
		case Blaze::OpenGL::TextureInternalPixelFormat::RGBA8I:				 return GL_RGBA8I;
		case Blaze::OpenGL::TextureInternalPixelFormat::RGB8I:				 return GL_RGB8I;
		case Blaze::OpenGL::TextureInternalPixelFormat::RG8I:				 return GL_RG8I;
		case Blaze::OpenGL::TextureInternalPixelFormat::R8I:				 return GL_R8I;
		case Blaze::OpenGL::TextureInternalPixelFormat::RGBA16I:			 return GL_RGBA16I;
		case Blaze::OpenGL::TextureInternalPixelFormat::RGB16I:				 return GL_RGB16I;
		case Blaze::OpenGL::TextureInternalPixelFormat::RG16I:				 return GL_RG16I;
		case Blaze::OpenGL::TextureInternalPixelFormat::R16I:				 return GL_R16I;
		case Blaze::OpenGL::TextureInternalPixelFormat::RGBA32I:			 return GL_RGBA32I;
		case Blaze::OpenGL::TextureInternalPixelFormat::RGB32I:				 return GL_RGB32I;
		case Blaze::OpenGL::TextureInternalPixelFormat::RG32I:				 return GL_RG32I;
		case Blaze::OpenGL::TextureInternalPixelFormat::R32I:				 return GL_R32I;
		case Blaze::OpenGL::TextureInternalPixelFormat::RGBA8UI:			 return GL_RGBA8UI;
		case Blaze::OpenGL::TextureInternalPixelFormat::RGB8UI:				 return GL_RGB8UI;
		case Blaze::OpenGL::TextureInternalPixelFormat::RG8UI:				 return GL_RG8UI;
		case Blaze::OpenGL::TextureInternalPixelFormat::R8UI:				 return GL_R8UI;
		case Blaze::OpenGL::TextureInternalPixelFormat::RGBA16UI:			 return GL_RGBA16UI;
		case Blaze::OpenGL::TextureInternalPixelFormat::RGB16UI:			 return GL_RGB16UI;
		case Blaze::OpenGL::TextureInternalPixelFormat::RG16UI:				 return GL_RG16UI;
		case Blaze::OpenGL::TextureInternalPixelFormat::R16UI:				 return GL_R16UI;
		case Blaze::OpenGL::TextureInternalPixelFormat::RGBA32UI:			 return GL_RGBA32UI;
		case Blaze::OpenGL::TextureInternalPixelFormat::RGB32UI:			 return GL_RGB32UI;
		case Blaze::OpenGL::TextureInternalPixelFormat::RG32UI:				 return GL_RG32UI;
		case Blaze::OpenGL::TextureInternalPixelFormat::R32UI:				 return GL_R32UI;
		case Blaze::OpenGL::TextureInternalPixelFormat::Depth16:			 return GL_DEPTH_COMPONENT16;
		case Blaze::OpenGL::TextureInternalPixelFormat::Depth24:			 return GL_DEPTH_COMPONENT24;
		case Blaze::OpenGL::TextureInternalPixelFormat::Depth32F:			 return GL_DEPTH_COMPONENT32F;
		case Blaze::OpenGL::TextureInternalPixelFormat::Depth24Stencil8:	 return GL_DEPTH24_STENCIL8;
		case Blaze::OpenGL::TextureInternalPixelFormat::Depth32FStencil8:	 return GL_DEPTH32F_STENCIL8;
		case Blaze::OpenGL::TextureInternalPixelFormat::Stencil8:			 return GL_STENCIL_INDEX;
		default: return -1;
		}
	}

	ILenum DevILPixelType(BitmapPixelType type)
	{
		switch (type)
		{
		case Blaze::BitmapPixelType::Int8: return IL_BYTE;
		case Blaze::BitmapPixelType::Uint8:	return IL_UNSIGNED_BYTE;
		case Blaze::BitmapPixelType::Int16:	return IL_SHORT;
		case Blaze::BitmapPixelType::Uint16: return IL_UNSIGNED_SHORT;
		case Blaze::BitmapPixelType::Int32:	return IL_INT;
		case Blaze::BitmapPixelType::Uint32: return IL_UNSIGNED_INT;
		case Blaze::BitmapPixelType::Float:	return IL_FLOAT;
		case Blaze::BitmapPixelType::Double: return IL_DOUBLE;
		default: return 0;
		}
	}
	GLenum OpenGLPixelType(BitmapPixelType type)
	{
		switch (type)
		{
		case Blaze::BitmapPixelType::Int8: return GL_BYTE;
		case Blaze::BitmapPixelType::Uint8:	return GL_UNSIGNED_BYTE;
		case Blaze::BitmapPixelType::Int16:	return GL_SHORT;
		case Blaze::BitmapPixelType::Uint16: return GL_UNSIGNED_SHORT;
		case Blaze::BitmapPixelType::Int32:	return GL_INT;
		case Blaze::BitmapPixelType::Uint32: return GL_UNSIGNED_INT;
		case Blaze::BitmapPixelType::Float:	return GL_FLOAT;
		case Blaze::BitmapPixelType::Double: return GL_DOUBLE;
		default: return 0;
		}
	}
	BitmapPixelType DevILToBlazePixelType(ILenum type)
	{
		switch (type)
		{
		case IL_BYTE: return Blaze::BitmapPixelType::Int8;
		case IL_UNSIGNED_BYTE: return Blaze::BitmapPixelType::Uint8;
		case IL_SHORT: return Blaze::BitmapPixelType::Int16;
		case IL_UNSIGNED_SHORT: return Blaze::BitmapPixelType::Uint16;
		case IL_INT: return Blaze::BitmapPixelType::Int32;
		case IL_UNSIGNED_INT: return Blaze::BitmapPixelType::Uint32;
		case IL_FLOAT: return Blaze::BitmapPixelType::Float;
		case IL_DOUBLE: return Blaze::BitmapPixelType::Double;
		}
	}
	BitmapPixelType OpenGLToBlazePixelType(GLenum type)
	{
		switch (type)
		{
		case GL_BYTE: return Blaze::BitmapPixelType::Int8;
		case GL_UNSIGNED_BYTE: return Blaze::BitmapPixelType::Uint8;
		case GL_SHORT: return Blaze::BitmapPixelType::Int16;
		case GL_UNSIGNED_SHORT: return Blaze::BitmapPixelType::Uint16;
		case GL_INT: return Blaze::BitmapPixelType::Int32;
		case GL_UNSIGNED_INT: return Blaze::BitmapPixelType::Uint32;
		case GL_FLOAT: return Blaze::BitmapPixelType::Float;
		case GL_DOUBLE: return Blaze::BitmapPixelType::Double;
		}
	}
	GLenum OpenGLTextureMinSampling(Graphics::Core::TextureSampling min, Graphics::Core::TextureSampling mip, bool mipmaps)
	{
		if (mipmaps)
		{
			if (mip == Graphics::Core::TextureSampling::Nearest)
				if (min == Graphics::Core::TextureSampling::Nearest)
					return GL_NEAREST_MIPMAP_NEAREST;
				else
					return GL_LINEAR_MIPMAP_NEAREST;
			else
				if (min == Graphics::Core::TextureSampling::Nearest)
					return GL_NEAREST_MIPMAP_LINEAR;
				else
					return GL_LINEAR_MIPMAP_LINEAR;
		}
		else
			if (min == Graphics::Core::TextureSampling::Nearest)
				return GL_NEAREST;
			else
				return GL_LINEAR;
	}
	GLenum OpenGLTextureMagSampling(Graphics::Core::TextureSampling sampling)
	{
		switch (sampling)
		{
		case Blaze::OpenGL::TextureSampling::Nearest: return GL_NEAREST;
		case Blaze::OpenGL::TextureSampling::Linear: return GL_LINEAR;
		}
	}	
	GLenum OpenGLTextureWrapping(Graphics::Core::TextureWrapping wrapping)
	{
		switch (wrapping)
		{
		case Blaze::OpenGL::TextureWrapping::ClampToBorder: return GL_CLAMP_TO_BORDER;			
		case Blaze::OpenGL::TextureWrapping::ClampToEdge: return GL_CLAMP_TO_EDGE;			
		case Blaze::OpenGL::TextureWrapping::MirroredRepeat: return GL_MIRRORED_REPEAT;			
		case Blaze::OpenGL::TextureWrapping::Repeat: return GL_REPEAT;			
		}
	}
	Graphics::Core::TextureInternalPixelFormat MapInternalTexturePixelFormat(BitmapPixelFormat format)
	{		
		switch (format)
		{
		case BitmapPixelFormat::Red: return Graphics::Core::TextureInternalPixelFormat::R32F;
		case BitmapPixelFormat::RG: return Graphics::Core::TextureInternalPixelFormat::RG32F;
		case BitmapPixelFormat::RGB: return Graphics::Core::TextureInternalPixelFormat::RGB32F;
		case BitmapPixelFormat::RGBA: return Graphics::Core::TextureInternalPixelFormat::RGBA32F;
		case BitmapPixelFormat::BGR: return Graphics::Core::TextureInternalPixelFormat::RGB32F;
		case BitmapPixelFormat::BGRA: return Graphics::Core::TextureInternalPixelFormat::RGBA32F;
		}
	}
	GLenum OpenGLFramebufferAttachment(Graphics::Core::FramebufferAttachment attachment)
	{
		switch (attachment)
		{
		case Blaze::OpenGL::FramebufferAttachment::DepthStencil: return GL_DEPTH_STENCIL_ATTACHMENT;
		case Blaze::OpenGL::FramebufferAttachment::Stencil:	return GL_STENCIL_ATTACHMENT;
		case Blaze::OpenGL::FramebufferAttachment::Depth: return GL_DEPTH_ATTACHMENT;
		}
	}
	GLenum OpenGLBufferDynamicStorageHint(Graphics::Core::GraphicsBufferDynamicStorageHint hint)
	{
		switch (hint)
		{
		case Blaze::OpenGL::GraphicsBufferDynamicStorageHint::StreamDraw: return GL_STREAM_DRAW;
		case Blaze::OpenGL::GraphicsBufferDynamicStorageHint::StreamRead: return GL_STREAM_READ;
		case Blaze::OpenGL::GraphicsBufferDynamicStorageHint::StreamCopy: return GL_STREAM_COPY;
		case Blaze::OpenGL::GraphicsBufferDynamicStorageHint::StaticDraw: return GL_STATIC_DRAW;
		case Blaze::OpenGL::GraphicsBufferDynamicStorageHint::StaticRead: return GL_STATIC_READ;
		case Blaze::OpenGL::GraphicsBufferDynamicStorageHint::StaticCopy: return GL_STATIC_COPY;
		case Blaze::OpenGL::GraphicsBufferDynamicStorageHint::DynamicDraw: return GL_DYNAMIC_DRAW;
		case Blaze::OpenGL::GraphicsBufferDynamicStorageHint::DynamicRead: return GL_DYNAMIC_READ;
		case Blaze::OpenGL::GraphicsBufferDynamicStorageHint::DynamicCopy: return GL_DYNAMIC_COPY;		
		}
	}
	GLenum OpenGLBufferStaticStorageHint(Graphics::Core::GraphicsBufferStaticStorageHint hint)
	{
		switch (hint)
		{
		case Blaze::OpenGL::GraphicsBufferStaticStorageHint::StaticStorage: return 0;
		case Blaze::OpenGL::GraphicsBufferStaticStorageHint::DynamicStorage: return GL_DYNAMIC_STORAGE_BIT;			
		case Blaze::OpenGL::GraphicsBufferStaticStorageHint::MapRead: return GL_MAP_READ_BIT;			
		case Blaze::OpenGL::GraphicsBufferStaticStorageHint::MapWrite: return GL_MAP_WRITE_BIT;
		case Blaze::OpenGL::GraphicsBufferStaticStorageHint::MapPersistant: return GL_MAP_PERSISTENT_BIT;
		case Blaze::OpenGL::GraphicsBufferStaticStorageHint::MapCoherent: return GL_MAP_COHERENT_BIT;
		case Blaze::OpenGL::GraphicsBufferStaticStorageHint::ClientStorage: return GL_CLIENT_STORAGE_BIT;		
		}
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