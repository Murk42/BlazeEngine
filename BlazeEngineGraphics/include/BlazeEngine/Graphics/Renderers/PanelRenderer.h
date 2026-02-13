#pragma once
#include "BlazeEngine/Core/Common/Color.h"
#include "BlazeEngine/Core/Math/Vector.h"
#include "BlazeEngine/Graphics/Renderers/RendererBase.h"

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

	class PanelRenderer : public RendererBase
	{
	public:
		virtual void Render(const PanelRenderData& data, const RenderContext& context) = 0;

		RendererTypeID GetTypeID() const { return RendererBase::GetTypeID<PanelRenderer>(); }
	};
}