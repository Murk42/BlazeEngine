#pragma once
#include "TextRendererBase.h"

namespace Graphics3::Dynamic
{
	class TextRenderer_Dynamic : public DynamicWrapper<TextRendererBase>, public TextRendererBase
	{
	public:
		TextRenderer_Dynamic(GraphicsContext& graphicsContext, RenderContext& transferRenderContext);

		void RenderText(RenderContextBase& context, FramebufferBase& target, TextRenderData renderData, std::initializer_list<Semaphore*> waitSemaphores, std::initializer_list<Semaphore*> signalSemaphores) override;
	};

	using TextRenderer = TextRenderer_Dynamic;
}