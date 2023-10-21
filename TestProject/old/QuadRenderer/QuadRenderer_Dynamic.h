#pragma once
#include "QuadRendererBase.h"

namespace Graphics3::Dynamic
{
	class QuadRenderer_Dynamic : public DynamicWrapper<QuadRendererBase>, public QuadRendererBase
	{
	public:
		QuadRenderer_Dynamic(GraphicsContext& graphicsContext, RenderContext& transferRenderContext);

		void RenderQuad(RenderContextBase& context, FramebufferBase& target, std::initializer_list<Semaphore*> waitSemaphores, std::initializer_list<Semaphore*> signalSemaphores) override;
	};

	using QuadRenderer = QuadRenderer_Dynamic;
}