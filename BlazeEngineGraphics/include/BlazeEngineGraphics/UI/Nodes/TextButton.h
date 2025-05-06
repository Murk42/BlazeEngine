#pragma once
#include "BlazeEngineGraphics/UI/Common/ButtonBase.h"
#include "BlazeEngineGraphics/UI/Graphics/RenderUnits/Text/TextRenderUnit.h"
#include "BlazeEngineGraphics/UI/Graphics/RenderUnits/PanelRenderUnit.h"
#include "BlazeEngineGraphics/RenderScene/RenderObject.h"

namespace Blaze::UI::Nodes
{		
	class BLAZE_GRAPHICS_API TextButton :
		public ButtonBase,
		public Graphics::RenderObject,
		EventHandler<ButtonBase::PressableFlagChangedEvent>
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

		ColorRGBAf normalPanelColor = 0x333333ff;
		ColorRGBAf highlightedPanelColor = 0x404040ff;
		ColorRGBAf pressedPanelColor = 0x282828ff;
		ColorRGBAf unpressablePanelColor = 0x181818ff;

		ColorRGBAf normalTextColor = 0xf0f0f0ff;
		ColorRGBAf unpressableTextColor = 0xbbbbbbff;

		void UpdatePanelColor();
		
		void OnEvent(const MouseEnterEvent& event) override;
		void OnEvent(const MouseExitEvent& event) override;
		void OnEvent(const MouseButtonDownEvent& event) override;
		void OnEvent(const MouseButtonUpEvent& event) override;		
		void OnEvent(const DeselectedEvent& event) override;		
		void OnEvent(const PressableFlagChangedEvent& event) override;
	};
}