#pragma once
#include "Graphics4/Dynamic/Core/Framebuffer.h"

namespace Graphics4::OpenGL
{
	class RenderWindowFramebuffer_OpenGL;

	class Framebuffer_OpenGL : public Dynamic::FramebufferBase
	{
	public:
		Framebuffer_OpenGL();
		Framebuffer_OpenGL(Framebuffer_OpenGL&&) noexcept;
		~Framebuffer_OpenGL() override;

		void Destroy();

		inline Vec2u GetSize() const override { return size; }
		inline uint GetID() const { return id; };

		void MakeActive() const;

		inline RenderWindowFramebuffer_OpenGL* GetRenderWindowFramebuffer() { return renderWindowFramebuffer; }

		Framebuffer_OpenGL& operator=(Framebuffer_OpenGL&&) noexcept;
	private:
		uint id;
		Vec2u size;
		RenderWindowFramebuffer_OpenGL* renderWindowFramebuffer;

		friend class RenderWindow_OpenGL;
		friend class RenderWindowFramebuffer_OpenGL;
	};
}