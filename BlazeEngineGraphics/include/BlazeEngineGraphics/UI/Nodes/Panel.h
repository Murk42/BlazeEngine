#pragma once
#include "BlazeEngineGraphics/UI/Core/UINode.h"
#include "BlazeEngineGraphics/UI/Elements/PanelRenderUnit.h"
#include "BlazeEngineGraphics/RenderScene/RenderObject.h"

namespace Blaze::UI
{
	class Panel : 
		public Node,
		public Graphics::RenderObject		
	{
	public:		
		Panel();

		inline void SetFillColor(ColorRGBAf color) { renderUnit.SetFillColor(color); }
		inline void SetBorderColor(ColorRGBAf color) { renderUnit.SetBorderColor(color); }
		inline void SetCornerRadius(float radius) { renderUnit.SetCornerRadius(radius); }
		inline void SetBorderWidth(float width) { renderUnit.SetBorderWidth(width); }

		inline ColorRGBAf GetFillColor() const { return renderUnit.GetFillColor(); }
		inline ColorRGBAf GetBorderColor() const { return renderUnit.GetBorderColor(); }
		inline float GetCornerRadius() const { return renderUnit.GetCornerRadius(); }
		inline float GetBorderWidth() const { return renderUnit.GetBorderWidth(); }

		Graphics::RenderUnit* GetRenderUnit(uint index) override;
	private:
		UIGraphics::PanelNodeRenderUnit renderUnit;
	};
}