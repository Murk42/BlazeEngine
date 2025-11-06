#pragma once
#include "BlazeEngine/Core/BlazeEngineCoreDefines.h"

namespace Blaze::Graphics::OpenGL
{
	class GraphicsContext_OpenGL;

	class BLAZE_API Semaphore_OpenGL
	{
	public:
		/*Parity*/Semaphore_OpenGL(GraphicsContext_OpenGL& graphicsContext);
		/*Parity*/~Semaphore_OpenGL();
	};
}