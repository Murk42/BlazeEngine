#pragma once
#include "BlazeEngineCore/Graphics/OpenGL/OpenGLGraphicsBuffer.h"
#include "BlazeEngineCore/Graphics/OpenGL/Textures/OpenGLTextureEnums.h"

namespace Blaze::OpenGL
{	

	class BLAZE_CORE_API TextureBuffer
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