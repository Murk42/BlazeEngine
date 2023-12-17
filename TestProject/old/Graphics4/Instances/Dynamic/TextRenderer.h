#pragma once
#include "Graphics4/Dynamic/API_dynamic.h"
#include "Graphics4/Dynamic/TextRenderTask.h"

namespace Graphics4
{
	struct TextTransformData
	{
		Vec2f pos;
		float rotation;

		Vec2f pivot;
		Vec2f rotationPivot;
		Vec2f anchorPivot;
	};
	struct TextRenderData
	{
		StringViewUTF8 text;
		Array<ColorRGBAf> colors;
		TextTransformData transform;
		uint height;
		Font* font;
		FontResolution* fontResolution;
		TextLayouterBase* textLayouter;
	};
}

namespace Graphics4::Dynamic
{	

	class TextRendererBase : public DynamicImplementation<TextRendererBase>
	{
	public:
		TextRendererBase(Graphics4::Dynamic::GraphicsContextBase& graphicsContext);

		virtual bool CreateTask(const TextRenderData& renderData, TextRenderTaskBase& task);
	protected:
		TextRendererBase();
	};
}