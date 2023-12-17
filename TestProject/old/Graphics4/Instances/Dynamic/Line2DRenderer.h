#pragma once
#include "Graphics4/Dynamic/Line2DRenderTask.h"

namespace Graphics4
{
	struct Line2DRenderData
	{
		Vec2f p1, p2;
		ColorRGBAf color;
		float width;
	};
}

namespace Graphics4::Dynamic
{
	class Line2DRendererBase : public DynamicImplementation<Line2DRendererBase>
	{
	public:
		Line2DRendererBase(Graphics4::Dynamic::GraphicsContextBase& graphicsConLine2D);

		virtual bool CreateTask(const Array<Line2DRenderData>& renderData, Line2DRenderTaskBase& task);
	protected:
		Line2DRendererBase();
	};
}