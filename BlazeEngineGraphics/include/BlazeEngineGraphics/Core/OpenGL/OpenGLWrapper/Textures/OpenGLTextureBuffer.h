#pragma once
#include "OpenGLTextureEnums.h"

namespace Blaze::Graphics::OpenGLWrapper
{	

	class TextureBuffer
	{
		unsigned id;		
	public:
		TextureBuffer();
		TextureBuffer(const TextureBuffer&) = delete;
		TextureBuffer(TextureBuffer&&) noexcept;
		~TextureBuffer();

		Result Associate(const GraphicsBuffer& buffer, TextureBufferInternalPixelFormat format);
		
		inline unsigned GetHandle() const { return id; }

		TextureBuffer& operator=(const TextureBuffer&) = delete;
		TextureBuffer& operator=(TextureBuffer&&) noexcept;
	};
}