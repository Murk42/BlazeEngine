#pragma once
#include "OpenGLEnums.h"
#include "Textures/OpenGLTextureEnums.h"
#include "OpenGLVertexArray.h"
#include "OpenGLFramebuffer.h"
#include "GL/glew.h"

namespace Blaze::Graphics::OpenGLWrapper
{
	BLAZE_GRAPHICS_API GLenum OpenGLPixelFormat(BitmapColorFormat format, Result& result);

	BLAZE_GRAPHICS_API GLenum OpenGLFormatByInternalPixelFormat(TextureInternalPixelFormat format, Result& result);
	BLAZE_GRAPHICS_API GLenum OpenGLInternalPixelFormat(TextureInternalPixelFormat format, Result& result);
	BLAZE_GRAPHICS_API GLenum OpenGLBufferInternalPixelFormat(TextureBufferInternalPixelFormat format, Result& result);

	BLAZE_GRAPHICS_API GLenum OpenGLPixelType(BitmapColorComponentType type, Result& result);	
	BLAZE_GRAPHICS_API BitmapColorComponentType OpenGLToBlazePixelType(GLenum type, Result& result);

	BLAZE_GRAPHICS_API GLenum OpenGLTextureMinSampling(TextureSampling min, TextureSampling mip, bool mipmaps, Result& result);
	BLAZE_GRAPHICS_API GLenum OpenGLTextureMagSampling(TextureSampling sampling, Result& result);
	BLAZE_GRAPHICS_API GLenum OpenGLTextureWrapping(TextureWrapping wrapping, Result& result);

	BLAZE_GRAPHICS_API TextureInternalPixelFormat MapInternalTexturePixelFormat(BitmapColorFormat format, Result& result);
	BLAZE_GRAPHICS_API BitmapColorFormat MapInternalTexturePixelFormat(TextureInternalPixelFormat format, Result& result);

	BLAZE_GRAPHICS_API GLenum OpenGLVertexAttributeType(VertexAttributeType type, Result& result);

	BLAZE_GRAPHICS_API GLenum OpenGLFramebufferAttachment(FramebufferAttachment attachment, Result& result);

	BLAZE_GRAPHICS_API GLenum OpenGLBufferMappingAccess(ImmutableGraphicsBufferMapAccess mapping, Result& result);	
	BLAZE_GRAPHICS_API GLenum OpenGLBufferMappingType(ImmutableGraphicsBufferMapType type, Result& result);
	BLAZE_GRAPHICS_API GLenum OpenGLBufferMappingOptions(ImmutableGraphicsBufferMapOptions options, Result& result);

	enum class MutableGraphicsBufferUseType
	{
		Draw, Read, Copy
	};
	BLAZE_GRAPHICS_API GLenum OpenGLMutableBufferUsage(MutableGraphicsBufferUseFrequency frequency, MutableGraphicsBufferUseType use, Result& result);
}