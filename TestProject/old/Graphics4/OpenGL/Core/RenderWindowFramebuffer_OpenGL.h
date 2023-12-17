#pragma once
#include "Graphics4/Dynamic/Core/RenderWindowFramebuffer.h"
#include "Framebuffer_OpenGL.h"

namespace Graphics4::OpenGL
{		
	class RenderWindow_OpenGL;

	class RenderWindowFramebuffer_OpenGL : public Dynamic::RenderWindowFramebufferBase
	{
	public:
		RenderWindowFramebuffer_OpenGL();
		RenderWindowFramebuffer_OpenGL(RenderWindowFramebuffer_OpenGL&&) noexcept;
		~RenderWindowFramebuffer_OpenGL() override;

		bool GetFramebuffer(Dynamic::FramebufferBase*& framebuffer) override;
		bool GetFramebuffer(Framebuffer_OpenGL*& framebuffer);
		bool GetRenderWindow(Dynamic::RenderWindowBase*& renderWindow) const override;
		bool GetRenderWindow(RenderWindow_OpenGL*& renderWindow) const;

		RenderWindowFramebuffer_OpenGL& operator=(RenderWindowFramebuffer_OpenGL&&) noexcept;
	private:
		RenderWindow_OpenGL* renderWindow;
		Framebuffer_OpenGL framebuffer;

		friend class RenderWindow_OpenGL;
	};

}