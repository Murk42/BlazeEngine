#pragma once

namespace Graphics4::OpenGL
{
	class GraphicsContext_OpenGL;

	class BLAZE_GRAPHICS_API Semaphore_OpenGL
	{
	public:
		/*Parity*/Semaphore_OpenGL(GraphicsContext_OpenGL& graphicsContext);
		/*Parity*/~Semaphore_OpenGL();
	};
}