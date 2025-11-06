#pragma once
#include "BlazeEngine/Core/Debug/Result.h"
#include "BlazeEngine/Common/Bitmap.h"
#include "BlazeEngine/Graphics/Core/OpenGL/OpenGLWrapper/OpenGLEnums.h"
#include <GL/glew.h>

namespace Blaze::Graphics::OpenGL
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

	GLenum ConvertToOpenGLEnum(IntegerVertexAttributeType type);
	GLenum ConvertToOpenGLEnum(FloatVertexAttributeType type);
	GLenum ConvertToOpenGLEnum(PackedVertexAttributeType type);	
	GLenum ConvertToOpenGLEnum(BGRAVertexAttributeType type);

	GLenum OpenGLFramebufferAttachment(FramebufferAttachment attachment, Result& result);
	FramebufferStatus MapFramebufferStatus(GLenum status, Result& result);

	GLenum OpenGLBufferMappingAccess(GraphicsBufferMapAccessFlags mapping, Result& result);	
	GLenum OpenGLBufferMappingType(GraphicsBufferMapType type, Result& result);
	GLenum OpenGLBufferMappingOptions(GraphicsBufferMapOptions options, Result& result);

	UniformType MapOpenGLUniformType(GLenum value);

	enum class GraphicsBufferUseType
	{
		Draw, Read, Copy
	};
	GLenum OpenGLMutableBufferUsage(GraphicsBufferUseFrequency frequency, GraphicsBufferUseType use, Result& result);
}