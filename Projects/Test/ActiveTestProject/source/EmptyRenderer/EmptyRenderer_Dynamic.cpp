#include "EmptyRenderer_Dynamic.h"
#include "EmptyRenderer_OpenGL.h"
#include "EmptyRenderer_Vulkan.h"

namespace Graphics3::Dynamic
{
	EmptyRenderer_Dynamic::EmptyRenderer_Dynamic(GraphicsContext& graphicsContext, RenderContext& transferRenderContext)
		: DynamicWrapper(nullptr, graphicsContext.GetImplementationInfo())
	{
		switch (info->api)
		{
		case ImplementationAPI::Vulkan:
			base = new Vulkan::EmptyRenderer((Vulkan::GraphicsContext&)*graphicsContext.GetPtr(), (Vulkan::RenderContext&)*transferRenderContext.GetPtr());
			break;
		case ImplementationAPI::OpenGL:
			base = new OpenGL::EmptyRenderer((OpenGL::GraphicsContext&)*graphicsContext.GetPtr(), (OpenGL::RenderContext&)*transferRenderContext.GetPtr());
			break;
		default:
			break;
		}
	}
	void EmptyRenderer_Dynamic::RenderEmpty(RenderContextBase& context, FramebufferBase& target, Recti renderArea, std::initializer_list<Semaphore*> waitSemaphores, std::initializer_list<Semaphore*> signalSemaphores)
	{
		base->RenderEmpty(context, target, renderArea, waitSemaphores, signalSemaphores);
	}
}