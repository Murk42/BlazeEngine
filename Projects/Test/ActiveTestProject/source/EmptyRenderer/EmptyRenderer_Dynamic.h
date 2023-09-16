#pragma once
#include "EmptyRendererBase.h"

namespace Graphics3::Dynamic
{
	class EmptyRenderer_Dynamic : public DynamicWrapper<EmptyRendererBase>, public EmptyRendererBase
	{
	public:
		EmptyRenderer_Dynamic(GraphicsContext& graphicsContext, RenderContext& transferRenderContext);

		void RenderEmpty(RenderContextBase& context, FramebufferBase& target, Recti renderArea, std::initializer_list<Semaphore*> waitSemaphores, std::initializer_list<Semaphore*> signalSemaphores) override;
	};

	using EmptyRenderer = EmptyRenderer_Dynamic;
}