#pragma once
#include "BlazeEngine/Common/Bitmap.h"
#include "BlazeEngine/Graphics/Core/OpenGL/OpenGLWrapper/OpenGLEnums.h"
#include <GL/glew.h>

namespace Blaze::Graphics::OpenGL
{
	bool OpenGLPixelFormat(BitmapColorFormat format, GLenum& result);

	bool OpenGLFormatByInternalPixelFormat(TextureInternalPixelFormat format, GLenum& result);
	bool OpenGLInternalPixelFormat(TextureInternalPixelFormat format, GLenum& result);
	bool OpenGLBufferInternalPixelFormat(TextureBufferInternalPixelFormat format, GLenum& result);

	bool OpenGLPixelType(BitmapColorComponentType type, GLenum& result);
	bool OpenGLToBlazePixelType(GLenum type, BitmapColorComponentType& result);

	bool OpenGLTextureMinSampling(TextureSampling min, TextureSampling mip, bool mipmaps, GLenum& result);
	bool OpenGLTextureMagSampling(TextureSampling sampling, GLenum& result);
	bool OpenGLTextureWrapping(TextureWrapping wrapping, GLenum& result);

	bool BlazeInternalTexturePixelFormat(BitmapColorFormat format, TextureInternalPixelFormat& result);
	bool BlazeBitmapColorFormat(TextureInternalPixelFormat format, BitmapColorFormat& result);

	bool OpenGLIntegerVertexAttributeType(IntegerVertexAttributeType type, GLenum& result);
	bool OpenGLFloatVertexAttributeType(FloatVertexAttributeType type, GLenum& result);
	bool OpenGLPackedVertexAttributeType(PackedVertexAttributeType type, GLenum& result);
	bool OpenGLBGRAVertexAttributeType(BGRAVertexAttributeType type, GLenum& result);

	bool OpenGLFramebufferAttachment(FramebufferAttachment attachment, GLenum& result);
	bool BlazeFramebufferStatus(GLenum status, FramebufferStatus& result);

	bool OpenGLRenderWindowFramebufferBufferTargets(RenderWindowFramebufferBufferTargets value, GLenum& result);
	bool OpenGLRenderWindowFramebufferBufferTarget(RenderWindowFramebufferBufferTarget value, GLenum& result);

	bool OpenGLBufferMappingAccess(GraphicsBufferMapAccessFlags mapping, GLenum& result);	
	bool OpenGLBufferMappingType(GraphicsBufferMapType type, GLenum& result);
	bool OpenGLBufferMappingOptions(GraphicsBufferMapOptions options, GLenum& result);

	bool BlazeUniformType(GLenum value, UniformType& result);

	enum class GraphicsBufferUseType
	{
		Draw, Read, Copy
	};
	bool OpenGLMutableBufferUsage(GraphicsBufferUseFrequency frequency, GraphicsBufferUseType use, GLenum& result);
}