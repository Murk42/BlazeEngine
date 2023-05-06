#pragma once
#include "BlazeEngine/Resources/Bitmap/Bitmap.h"
#include "BlazeEngine/Graphics/GraphicsLibrary.h"
#include "BlazeEngine/Graphics/GraphicsEnums.h"

#include <IL/il.h>
#include <GL/glew.h>

#include <SDL/SDL.h>

namespace Blaze
{
	unsigned GetFormatDepth(BitmapPixelFormat format, Result& result);
	unsigned PixelTypeSize(BitmapPixelType type, Result& result);

	ILenum DevILPixelFormat(BitmapPixelFormat format, Result& result);
	GLenum OpenGLPixelFormat(BitmapPixelFormat format, Result& result);
	SDL_PixelFormatEnum SDLPixelFormat(BitmapPixelFormat format, Result& result);
	BitmapPixelFormat DevILToBlazePixelFormat(ILenum format, Result& result);
	BitmapPixelFormat OepnGLToBlazePixelFormat(GLenum format, Result& result);
	GLenum OpenGLFormatByInternalPixelFormat(Graphics::Core::TextureInternalPixelFormat format, Result& result);
	GLenum OpenGLInternalPixelFormat(Graphics::Core::TextureInternalPixelFormat format, Result& result);
	GLenum OpenGLBufferInternalPixelFormat(Graphics::Core::TextureBufferInternalPixelFormat format, Result& result);

	ILenum DevILPixelType(BitmapPixelType type, Result& result);
	GLenum OpenGLPixelType(BitmapPixelType type, Result& result);
	BitmapPixelType DevILToBlazePixelType(ILenum type, Result& result);
	BitmapPixelType OpenGLToBlazePixelType(GLenum type, Result& result);

	GLenum OpenGLTextureMinSampling(Graphics::Core::TextureSampling min, Graphics::Core::TextureSampling mip, bool mipmaps, Result& result);
	GLenum OpenGLTextureMagSampling(Graphics::Core::TextureSampling sampling, Result& result);
	GLenum OpenGLTextureWrapping(Graphics::Core::TextureWrapping wrapping, Result& result);

	Graphics::Core::TextureInternalPixelFormat MapInternalTexturePixelFormat(BitmapPixelFormat format, Result& result);

	GLenum OpenGLVertexAttributeType(Graphics::Core::VertexAttributeType type, Result& result);

	GLenum OpenGLFramebufferAttachment(Graphics::Core::FramebufferAttachment attachment, Result& result);
		
	GLenum OpenGLBufferMappingAccess(Graphics::Core::ImmutableGraphicsBufferMapAccess mapping, Result& result);	
	GLenum OpenGLBufferMappingType(Graphics::Core::ImmutableGraphicsBufferMapType type, Result& result);
	GLenum OpenGLBufferMappingOptions(Graphics::Core::ImmutableGraphicsBufferMapOptions options, Result& result);

	enum class MutableGraphicsBufferUseType
	{
		Draw, Read, Copy
	};
	GLenum OpenGLMutableBufferUsage(Graphics::Core::MutableGraphicsBufferUseFrequency frequency, MutableGraphicsBufferUseType use, Result& result);
	//GLenum OpenGLBufferDynamicStorageHint(Graphics::Core::GraphicsBufferDynamicStorageHint hint, Result& result);
	//GLenum OpenGLBufferStaticStorageHint(Graphics::Core::GraphicsBufferStaticStorageHint hint, Result& result);

	DisplayPixelFormat BlazeDisplayPixelFormat(uint32 format, Result& result);
	SDL_PixelFormatEnum SDLDisplayPixelFormat(DisplayPixelFormat format, Result& result);
}