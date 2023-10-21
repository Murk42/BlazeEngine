#pragma once
#include "../Graphics3/API/API.h"

namespace Graphics3
{
	struct TextRenderData
	{		
		Array<ColorRGBAf> colors;
		float fontHeight;		
		const FontResolution* fontResolution;
		const Font* font;
		TextLayouterBase* textLayouter;
	};

	class TextRendererBase
	{
	public:
		virtual ~TextRendererBase() { }
		virtual void RenderText(Dynamic::RenderContextBase& context, Dynamic::FramebufferBase& target, TextRenderData renderData, std::initializer_list<Dynamic::Semaphore*> waitSemaphores, std::initializer_list<Dynamic::Semaphore*> signalSemaphores) = 0;
	};
}