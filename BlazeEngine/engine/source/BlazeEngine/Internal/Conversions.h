#pragma once
#include "BlazeEngine/Resources/Bitmap/Bitmap.h"
#include "BlazeEngine/Graphics/GraphicsLibrary.h"

#include <IL/il.h>
#include <GL/glew.h>

#include <SDL/SDL.h>

namespace Blaze
{
	unsigned GetFormatDepth(BitmapPixelFormat format);
	unsigned PixelTypeSize(BitmapPixelType type);

	ILenum DevILPixelFormat(BitmapPixelFormat format);
	GLenum OpenGLPixelFormat(BitmapPixelFormat format);
	SDL_PixelFormatEnum SDLPixelFormat(BitmapPixelFormat format);
	BitmapPixelFormat DevILToBlazePixelFormat(ILenum format);
	BitmapPixelFormat OepnGLToBlazePixelFormat(GLenum format);
	GLenum OpenGLFormatByInternalPixelFormat(Graphics::Core::TextureInternalPixelFormat format);
	GLenum OpenGLInternalPixelFormat(Graphics::Core::TextureInternalPixelFormat format);
	GLenum OpenGLBufferInternalPixelFormat(Graphics::Core::TextureBufferInternalPixelFormat format);

	ILenum DevILPixelType(BitmapPixelType type);
	GLenum OpenGLPixelType(BitmapPixelType type);
	BitmapPixelType DevILToBlazePixelType(ILenum type);
	BitmapPixelType OpenGLToBlazePixelType(GLenum type);

	GLenum OpenGLTextureMinSampling(Graphics::Core::TextureSampling min, Graphics::Core::TextureSampling mip, bool mipmaps);
	GLenum OpenGLTextureMagSampling(Graphics::Core::TextureSampling sampling);
	GLenum OpenGLTextureWrapping(Graphics::Core::TextureWrapping wrapping);

	Graphics::Core::TextureInternalPixelFormat MapInternalTexturePixelFormat(BitmapPixelFormat format);

	GLenum OpenGLFramebufferAttachment(Graphics::Core::FramebufferAttachment attachment);

	GLenum OpenGLBufferDynamicStorageHint(Graphics::Core::GraphicsBufferDynamicStorageHint hint);
	GLenum OpenGLBufferStaticStorageHint(Graphics::Core::GraphicsBufferStaticStorageHint hint);
}