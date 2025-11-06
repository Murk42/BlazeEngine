#pragma once
#include "BlazeEngine/Core/Common/Rect.h"
#include "BlazeEngine/Graphics/Renderers/PanelRenderer.h"
#include "BlazeEngine/UI/Graphics/UIRenderUnit.h"

namespace Blaze::UI
{
	class BLAZE_API PanelRenderUnit : public UIRenderUnit<Graphics::PanelRenderer>
	{
	public:
		Vec2f pos, right, up;
		ColorRGBAf fillColor = 0x444444ff;
		ColorRGBAf borderColor = 0x222222ff;
		float borderWidth = 1;
		float cornerRadius = 10;

		PanelRenderUnit();
		~PanelRenderUnit();

		void Render(const Node& node, Graphics::PanelRenderer& renderer, const UIRenderContext& renderContext) override;
	};
}