#pragma once
#include "BlazeEngine/Core/Math/Vector.h"
#include "BlazeEngine/Core/Common/Color.h"
#include "BlazeEngine/Graphics/Renderers/RendererBase.h"

namespace Blaze::Graphics
{
	struct Quad2DRenderData
	{
		Vec2f p1, p2, p3;
		ColorRGBAf color = 0xffffffff;
	};

	class BLAZE_API Quad2DRenderer : public RendererBase
	{
	public:
		virtual void Render(const Quad2DRenderData& data, const RenderContext& context) = 0;
		void Render(Vec2f pos, Vec2f size, float rotation, ColorRGBAf color, const RenderContext& context);
		void Render(Vec2f pos, Vec2f size, Vec2f right, ColorRGBAf color, const RenderContext& context);

		RendererTypeID GetTypeID() const { return RendererBase::GetTypeID<Quad2DRenderer>(); }
	};
}