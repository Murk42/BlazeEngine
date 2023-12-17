#pragma once
#include "BlazeEngineCore/Graphics/OpenGL/Textures/OpenGLTextureEnums.h"

namespace Blaze::OpenGL
{
	class BLAZE_CORE_API Renderbuffer
	{
		unsigned id;
		Vec2i size;
	public:
		Renderbuffer();
		Renderbuffer(const Renderbuffer&) = delete;
		Renderbuffer(Renderbuffer&&) noexcept;
		~Renderbuffer();

		Result Create(Vec2i size, Blaze::OpenGL::TextureInternalPixelFormat format);

		inline uint GetHandle() const { return id; }
		inline Vec2i GetSize() const { return size; }

		Renderbuffer& operator=(const Renderbuffer&) = delete;
		Renderbuffer& operator=(Renderbuffer&&)noexcept;
	};
}