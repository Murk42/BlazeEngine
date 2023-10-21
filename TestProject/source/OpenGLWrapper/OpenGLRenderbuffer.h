#pragma once
#include "Textures/OpenGLTextureEnums.h"

namespace OpenGLWrapper
{
	class Renderbuffer
	{
		unsigned id;
		Vec2i size;
	public:
		Renderbuffer();
		Renderbuffer(const Renderbuffer&) = delete;
		Renderbuffer(Renderbuffer&&) noexcept;
		~Renderbuffer();

		Result Create(Vec2i size, OpenGLWrapper::TextureInternalPixelFormat format);

		inline uint GetHandle() const { return id; }
		inline Vec2i GetSize() const { return size; }

		Renderbuffer& operator=(const Renderbuffer&) = delete;
		Renderbuffer& operator=(Renderbuffer&&)noexcept;
	};
}