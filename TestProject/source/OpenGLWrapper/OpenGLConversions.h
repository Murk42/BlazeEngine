#pragma once
#include "OpenGLEnums.h"
#include "Textures/OpenGLTextureEnums.h"
#include "OpenGLVertexArray.h"
#include "OpenGLFramebuffer.h"
#include "GL/glew.h"

namespace OpenGLWrapper
{
	GLenum OpenGLPixelFormat(BitmapColorFormat format, Result& result);

	GLenum OpenGLFormatByInternalPixelFormat(TextureInternalPixelFormat format, Result& result);
	GLenum OpenGLInternalPixelFormat(TextureInternalPixelFormat format, Result& result);
	GLenum OpenGLBufferInternalPixelFormat(TextureBufferInternalPixelFormat format, Result& result);

	GLenum OpenGLPixelType(BitmapColorComponentType type, Result& result);	
	BitmapColorComponentType OpenGLToBlazePixelType(GLenum type, Result& result);

	GLenum OpenGLTextureMinSampling(TextureSampling min, TextureSampling mip, bool mipmaps, Result& result);
	GLenum OpenGLTextureMagSampling(TextureSampling sampling, Result& result);
	GLenum OpenGLTextureWrapping(TextureWrapping wrapping, Result& result);

	TextureInternalPixelFormat MapInternalTexturePixelFormat(BitmapColorFormat format, Result& result);
	BitmapColorFormat MapInternalTexturePixelFormat(TextureInternalPixelFormat format, Result& result);

	GLenum OpenGLVertexAttributeType(VertexAttributeType type, Result& result);

	GLenum OpenGLFramebufferAttachment(FramebufferAttachment attachment, Result& result);

	GLenum OpenGLBufferMappingAccess(ImmutableGraphicsBufferMapAccess mapping, Result& result);	
	GLenum OpenGLBufferMappingType(ImmutableGraphicsBufferMapType type, Result& result);
	GLenum OpenGLBufferMappingOptions(ImmutableGraphicsBufferMapOptions options, Result& result);

	enum class MutableGraphicsBufferUseType
	{
		Draw, Read, Copy
	};
	GLenum OpenGLMutableBufferUsage(MutableGraphicsBufferUseFrequency frequency, MutableGraphicsBufferUseType use, Result& result);	
}