#pragma once
#include "BlazeEngineGraphics/UI/Common/ButtonBase.h"
#include "BlazeEngineGraphics/UI/Graphics/RenderUnits/Text/TextRenderUnit.h"
#include "BlazeEngineGraphics/UI/Graphics/RenderUnits/PanelRenderUnit.h"
#include "BlazeEngineGraphics/RenderScene/RenderObject.h"

namespace Blaze::UI::Nodes
{		
	class BLAZE_GRAPHICS_API TextButton :
		public ButtonBase,
		public Graphics::RenderObject
	{
	public:
		PanelRenderUnit panelRenderUnit;
		TextRenderUnit textRenderUnit;

		TextButton();
		~TextButton() override;		
		
		void SetNormalColor(ColorRGBAf panelColor);
		void SetHighlightedColor(ColorRGBAf panelColor);
		void SetPressedColor(ColorRGBAf panelColor);

		Graphics::RenderUnit* GetRenderUnit(uint index) override;
	private:						
		bool hovered : 1;
		ColorRGBAf normalColor = 0x333333ff;
		ColorRGBAf highlightedColor = 0x404040ff;
		ColorRGBAf pressedColor = 0x282828ff;

		void UpdateColor();
		
		void OnEvent(MouseEnterEvent event) override;
		void OnEvent(MouseExitEvent event) override;
		void OnEvent(MousePressedEvent event) override;
		void OnEvent(MouseReleasedEvent event) override;		
		void OnEvent(DeselectedEvent event) override;		
	};
}