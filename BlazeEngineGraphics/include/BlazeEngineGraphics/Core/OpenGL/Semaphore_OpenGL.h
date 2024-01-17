#pragma once

namespace Blaze::Graphics::OpenGL
{
	class GraphicsContext_OpenGL;

	class BLAZE_GRAPHICS_API Semaphore_OpenGL
	{
	public:
		/*Parity*/Semaphore_OpenGL(GraphicsContext_OpenGL& graphicsContext);
		/*Parity*/~Semaphore_OpenGL();
	};
}