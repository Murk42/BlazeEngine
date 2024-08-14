#pragma once
#include "BlazeEngineGraphics/Core/OpenGL/OpenGLWrapper/OpenGLEnums.h"

namespace Blaze::Graphics::OpenGLWrapper
{	
	class BLAZE_GRAPHICS_API Fence
	{
		void* id;
	public:
		Fence();
		~Fence();

		void SetFence();

		void BlockServer();
		//The timeout is in seconds
		FenceReturnState BlockClient(double timeout);

		bool IsSet();
		bool IsSignaled();
	};
}