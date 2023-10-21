#pragma once
#include "BlazeEngine/Core/WindowSDL.h"

#include <SDL2/SDL_pixels.h>

namespace Blaze
{	
	//SDL_PixelFormatEnum SDLPixelFormat(BitmapColorFormat format, Result& result);	
	//BitmapColorFormat OepnGLToBlazePixelFormat(GLenum format, Result& result);
	//GLenum OpenGLFormatByInternalPixelFormat(GraphicsLibrary::TextureInternalPixelFormat format, Result& result);
	//GLenum OpenGLInternalPixelFormat(GraphicsLibrary::TextureInternalPixelFormat format, Result& result);
	//GLenum OpenGLBufferInternalPixelFormat(GraphicsLibrary::TextureBufferInternalPixelFormat format, Result& result);
	
	//GLenum OpenGLPixelType(BitmapColorComponentType type, Result& result);	
	//BitmapColorComponentType OpenGLToBlazePixelType(GLenum type, Result& result);

	//GLenum OpenGLTextureMinSampling(GraphicsLibrary::TextureSampling min, GraphicsLibrary::TextureSampling mip, bool mipmaps, Result& result);
	//GLenum OpenGLTextureMagSampling(GraphicsLibrary::TextureSampling sampling, Result& result);
	//GLenum OpenGLTextureWrapping(GraphicsLibrary::TextureWrapping wrapping, Result& result);

	//GraphicsLibrary::TextureInternalPixelFormat MapInternalTexturePixelFormat(BitmapColorFormat format, Result& result);
	//BitmapColorFormat MapInternalTexturePixelFormat(GraphicsLibrary::TextureInternalPixelFormat format, Result& result);

	//GLenum OpenGLVertexAttributeType(GraphicsLibrary::VertexAttributeType type, Result& result);

	//GLenum OpenGLFramebufferAttachment(GraphicsLibrary::FramebufferAttachment attachment, Result& result);
		
	//GLenum OpenGLBufferMappingAccess(GraphicsLibrary::ImmutableGraphicsBufferMapAccess mapping, Result& result);	
	//GLenum OpenGLBufferMappingType(GraphicsLibrary::ImmutableGraphicsBufferMapType type, Result& result);
	//GLenum OpenGLBufferMappingOptions(GraphicsLibrary::ImmutableGraphicsBufferMapOptions options, Result& result);

	//enum class MutableGraphicsBufferUseType
	//{
	//	Draw, Read, Copy
	//};
	//GLenum OpenGLMutableBufferUsage(GraphicsLibrary::MutableGraphicsBufferUseFrequency frequency, MutableGraphicsBufferUseType use, Result& result);
	//GLenum OpenGLBufferDynamicStorageHint(GraphicsLibrary::GraphicsBufferDynamicStorageHint hint, Result& result);
	//GLenum OpenGLBufferStaticStorageHint(GraphicsLibrary::GraphicsBufferStaticStorageHint hint, Result& result);

	DisplayPixelFormat BlazeDisplayPixelFormat(uint32 format, Result& result);
	SDL_PixelFormatEnum SDLDisplayPixelFormat(DisplayPixelFormat format, Result& result);
}