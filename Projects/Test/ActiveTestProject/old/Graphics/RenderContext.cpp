#include "RenderContext.h"
#include "RenderContext.h"
#include "API Specific/Vulkan/Vulkan_RenderContext.h"
#include "RenderContext.h"

namespace Experimental::Graphics
{
	using namespace Vulkan;	

	void InitializeGraphics()
	{
		CreateVulkanInstance(true);
	}
	void DeinitializeGraphics()
	{
		FreeVulkanInstance();
	}    

	RenderContext::RenderContext()
	{
		this->impl = new RenderContextImpl();        
	}
	RenderContext::~RenderContext()
	{       
		delete (RenderContextImpl*)impl;
	}	
}
