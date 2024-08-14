#pragma once
#include "BlazeEngineGraphics/Core/OpenGL/OpenGLWrapper/OpenGLEnums.h"

namespace Blaze::Graphics::OpenGLWrapper
{
	class BLAZE_GRAPHICS_API Renderbuffer
	{
		unsigned id;
		Vec2i size;
	public:
		Renderbuffer();
		Renderbuffer(const Renderbuffer&) = delete;
		Renderbuffer(Renderbuffer&&) noexcept;
		~Renderbuffer();

		void Create(Vec2i size, Blaze::Graphics::OpenGLWrapper::TextureInternalPixelFormat format);

		inline uint GetHandle() const { return id; }
		inline Vec2i GetSize() const { return size; }

		Renderbuffer& operator=(const Renderbuffer&) = delete;
		Renderbuffer& operator=(Renderbuffer&&)noexcept;
	};
}