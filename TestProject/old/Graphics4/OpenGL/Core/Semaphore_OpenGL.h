#pragma once
#include "Graphics4/Dynamic/Core/Semaphore.h"

namespace Graphics4::OpenGL
{
	class GraphicsContext_OpenGL;

	class Semaphore_OpenGL : public Dynamic::SemaphoreBase
	{
	public:
		Semaphore_OpenGL(GraphicsContext_OpenGL& graphicsContext);
	};
}