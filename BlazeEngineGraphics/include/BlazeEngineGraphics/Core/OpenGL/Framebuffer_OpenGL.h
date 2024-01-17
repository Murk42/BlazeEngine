#pragma once

namespace Blaze::Graphics::OpenGL
{
	class RenderWindowFramebuffer_OpenGL;

	class BLAZE_GRAPHICS_API Framebuffer_OpenGL
	{
	public:
		/*Parity*/~Framebuffer_OpenGL();

		/*Parity*/inline Vec2u GetSize() const { return size; }
		/*Parity*/inline RenderWindowFramebuffer_OpenGL* GetRenderWindowFramebuffer() { return renderWindowFramebuffer; }


		inline uint GetID() const { return id; };		
	private:
		uint id;
		Vec2u size;
		RenderWindowFramebuffer_OpenGL* renderWindowFramebuffer;

		Framebuffer_OpenGL(RenderWindowFramebuffer_OpenGL* renderWindowFramebuffer);		
		
		friend class RenderWindowFramebuffer_OpenGL;
		friend class RenderWindow_OpenGL;
	};
}