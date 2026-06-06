#pragma once
#include "BlazeEngine/Core/Math/Vector.h"
#include "BlazeEngine/Core/Common/Color.h"
#include "BlazeEngine/Graphics/Core/RendererBase.h"

namespace Blaze::Graphics
{
	struct Line2DRenderData
	{
		Vec2f p1, p2;
		ColorRGBAf color = 0xffffffff_rgba;
		float width = 1.0f;
	};

	class BLAZE_API Line2DRenderer : public virtual RendererBase
	{
	public:
		virtual void Render(const Line2DRenderData& data) = 0;

		RendererTypeID GetTypeID() const override final { return RendererBase::GetTypeIDFor<Line2DRenderer>(); }
	};
}