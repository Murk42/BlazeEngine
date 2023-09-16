#include "Semaphore.h"
#include "API Specific/Vulkan/Vulkan_Semaphore.h"
#include "RenderContext.h"

namespace Experimental::Graphics
{
	using namespace Vulkan;

	Semaphore::Semaphore(RenderContext& renderContext)
	{
		impl = new SemaphoreImpl((RenderContextImpl*)renderContext.GetHandle());
	}
	Semaphore::~Semaphore()
	{
		delete (SemaphoreImpl*)impl;
	}	
}