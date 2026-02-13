#pragma once
#include "Framebuffer_OpenGL.h"

namespace Blaze::Graphics::OpenGL
{		
	class RenderWindow_OpenGL;

	class BLAZE_API RenderWindowFramebuffer_OpenGL
	{
	public:
		/*Parity*/~RenderWindowFramebuffer_OpenGL();

		/*Parity*/inline Framebuffer_OpenGL& GetFramebuffer() { return framebuffer; }
		/*Parity*/inline RenderWindow_OpenGL& GetRenderWindow() { return renderWindow; }

		RenderWindowFramebuffer_OpenGL(RenderWindow_OpenGL& renderWindow);			
	private:
		RenderWindow_OpenGL& renderWindow;
		Framebuffer_OpenGL framebuffer;		

		void ChangeSize(Vec2u size);

		friend class RenderWindow_OpenGL;
	};

}