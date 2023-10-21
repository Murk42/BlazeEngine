#include "TextRenderer_Dynamic.h"
#include "TextRenderer_OpenGL.h"
#include "TextRenderer_Vulkan.h"

namespace Graphics3::Dynamic
{
	TextRenderer_Dynamic::TextRenderer_Dynamic(GraphicsContext& graphicsContext, RenderContext& transferRenderContext)
		: DynamicWrapper(nullptr, graphicsContext.GetImplementationInfo())
	{
		switch (info->api)
		{
		case ImplementationAPI::Vulkan:
			base = new Vulkan::TextRenderer((Vulkan::GraphicsContext&)*graphicsContext.GetPtr(), (Vulkan::RenderContext&)*transferRenderContext.GetPtr());
			break;
		case ImplementationAPI::OpenGL:
			base = new OpenGL::TextRenderer((OpenGL::GraphicsContext&)*graphicsContext.GetPtr(), (OpenGL::RenderContext&)*transferRenderContext.GetPtr());
			break;
		default:
			break;
		}
	}
	void TextRenderer_Dynamic::RenderText(RenderContextBase& context, FramebufferBase& target, TextRenderData renderData, std::initializer_list<Semaphore*> waitSemaphores, std::initializer_list<Semaphore*> signalSemaphores)
	{
		base->RenderText(context, target, renderData, waitSemaphores, signalSemaphores);
	}
}