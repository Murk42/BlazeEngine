#include "RenderContext.h"

namespace Graphics2
{
	uint64 renderBackendID_OpenGL;
	uint64 renderBackendID_Vulkan;

	List<RenderContext::BackendCreateData> RenderContext::createData;	
	uint64 RenderContext::lastBackendID;

	RenderContext::RenderContext()
		: backendID(lastBackendID)
	{
		lastBackendID = ~(uint64)0;
	}
	

	RenderContext::~RenderContext()
	{
	}	

	uint64 RenderContext::GetOpenGLBackendID()
	{
		return renderBackendID_OpenGL;
	}

	uint64 RenderContext::GetVulkanBackendID()
	{
		return renderBackendID_Vulkan;
	}
}