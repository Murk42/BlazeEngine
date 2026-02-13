#pragma once
#include "BlazeEngine/Core/Math/Vector.h"
#include "BlazeEngine/Core/Common/Color.h"
#include "BlazeEngine/Graphics/Renderers/RendererBase.h"

namespace Blaze::Graphics
{
	struct Line2DRenderData
	{
		Vec2f p1, p2;
		ColorRGBAf color = 0xffffffff;
		float width = 1.0f;
	};

	class Line2DRenderer : public RendererBase
	{
	public:
		virtual void Render(const Line2DRenderData& data, const RenderContext& context) = 0;

		RendererTypeID GetTypeID() const { return RendererBase::GetTypeID<Line2DRenderer>(); }
	};
}