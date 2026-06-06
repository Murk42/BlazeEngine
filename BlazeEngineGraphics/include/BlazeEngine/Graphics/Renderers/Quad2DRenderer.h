#pragma once
#include "BlazeEngine/Core/Math/Vector.h"
#include "BlazeEngine/Core/Common/Color.h"
#include "BlazeEngine/Graphics/Core/RendererBase.h"

namespace Blaze::Graphics
{
	struct Quad2DRenderData
	{
		Vec2f p1, p2, p3;
		ColorRGBAf color = 0xffffffff_rgba;
	};

	class BLAZE_API Quad2DRenderer : public virtual RendererBase
	{
	public:
		virtual void Render(const Quad2DRenderData& data) = 0;
		void Render(Vec2f pos, Vec2f size, float rotation, ColorRGBAf color);
		void Render(Vec2f pos, Vec2f size, Vec2f right, ColorRGBAf color);

		RendererTypeID GetTypeID() const override final { return RendererBase::GetTypeIDFor<Quad2DRenderer>(); }
	};
}