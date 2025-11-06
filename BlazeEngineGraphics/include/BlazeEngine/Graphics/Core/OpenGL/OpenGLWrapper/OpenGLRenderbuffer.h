#pragma once
#include "BlazeEngine/Core/Math/Vector.h"
#include "BlazeEngine/Graphics/Core/OpenGL/OpenGLWrapper/OpenGLEnums.h"

namespace Blaze::Graphics::OpenGL
{
	class BLAZE_API Renderbuffer
	{
	public:
		Renderbuffer();
		Renderbuffer(const Renderbuffer&) = delete;
		Renderbuffer(Renderbuffer&&) noexcept;
		~Renderbuffer();

		void Create(Vec2u size, Blaze::Graphics::OpenGL::TextureInternalPixelFormat format);

		inline uint32 GetHandle() const { return id; }
		inline Vec2u GetSize() const { return size; }

		Renderbuffer& operator=(const Renderbuffer&) = delete;
		Renderbuffer& operator=(Renderbuffer&&)noexcept;
	private:
		uint32 id;
		Vec2u size;
	};
}