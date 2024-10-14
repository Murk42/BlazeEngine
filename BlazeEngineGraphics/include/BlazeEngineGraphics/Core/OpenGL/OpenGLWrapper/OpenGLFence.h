#pragma once
#include "BlazeEngineGraphics/Core/OpenGL/OpenGLWrapper/OpenGLEnums.h"

namespace Blaze::Graphics::OpenGLWrapper
{	
	class BLAZE_GRAPHICS_API Fence
	{
		void* id;
	public:
		Fence();
		Fence(void* id);
		~Fence();
		
		//Creates a new unsignaled fence
		void Clear();
		//When the GPU executes this command the fence will be signaled
		void SetFence();

		//Blocks the GPU until this fence is signaled
		void BlockServer();
		//Blocks the CPU until this fence is signaled. The timeout is in seconds
		FenceReturnState BlockClient(double timeout);

		bool IsSet();
		bool IsSignaled();

		inline void* GetHandle() const { return id; }
	};
}