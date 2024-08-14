#pragma once

namespace Blaze::Graphics::OpenGL
{
	class GraphicsContext_OpenGL;

	struct SDLOpenGLContext_OpenGL
	{
		void* handle;
	};

	void SetActiveOpenGLGraphicsContext(GraphicsContext_OpenGL& graphicsContext);
	void SetActiveOpenGLGraphicsContextForced(GraphicsContext_OpenGL& graphicsContext);

	GraphicsContext_OpenGL* GetActiveOpenGLGraphicsContext();	
}