#pragma once
#include "BlazeEngine/Core/Common/Color.h"
#include "BlazeEngine/Core/Math/Vector.h"
#include "BlazeEngine/Graphics/Core/RendererBase.h"

namespace Blaze::Graphics
{
	struct PanelRenderData
	{
		Vec2f pos, right, up;
		ColorRGBAf fillColor;
		ColorRGBAf borderColor;
		float borderWidth;
		float cornerRadius;
	};

	class PanelRenderer : public virtual RendererBase
	{
	public:
		virtual void Render(const PanelRenderData& data) = 0;

		RendererTypeID GetTypeID() const override final { return RendererBase::GetTypeIDFor<PanelRenderer>(); }
	};
}