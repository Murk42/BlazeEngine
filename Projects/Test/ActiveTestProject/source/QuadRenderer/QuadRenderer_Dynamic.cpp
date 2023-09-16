#include "QuadRenderer_Dynamic.h"
#include "QuadRenderer_OpenGL.h"
#include "QuadRenderer_Vulkan.h"

namespace Graphics3::Dynamic
{
	QuadRenderer_Dynamic::QuadRenderer_Dynamic(GraphicsContext& graphicsContext, RenderContext& transferRenderContext)
		: DynamicWrapper(nullptr, graphicsContext.GetImplementationInfo())
	{
		switch (info->api)
		{
		case ImplementationAPI::Vulkan:
			base = new Vulkan::QuadRenderer((Vulkan::GraphicsContext&)*graphicsContext.GetPtr(), (Vulkan::RenderContext&)*transferRenderContext.GetPtr());
			break;
		case ImplementationAPI::OpenGL:
			base = new OpenGL::QuadRenderer((OpenGL::GraphicsContext&)*graphicsContext.GetPtr(), (OpenGL::RenderContext&)*transferRenderContext.GetPtr());
			break;
		default:
			break;
		}
	}
	void QuadRenderer_Dynamic::RenderQuad(RenderContextBase& context, FramebufferBase& target, std::initializer_list<Semaphore*> waitSemaphores, std::initializer_list<Semaphore*> signalSemaphores)
	{
		base->RenderQuad(context, target, waitSemaphores, signalSemaphores);
	}
}