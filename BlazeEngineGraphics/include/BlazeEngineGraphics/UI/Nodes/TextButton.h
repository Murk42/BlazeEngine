#pragma once
#include "BlazeEngineGraphics/UI/Common/ButtonBase.h"
#include "BlazeEngineGraphics/UI/Common/StringUTF8TextContainer.h"
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
		TextButton();
		~TextButton() override;		
		
		void SetText(StringViewUTF8 text);
		void SetNormalColor(ColorRGBAf panelColor);
		void SetHighlightedColor(ColorRGBAf panelColor);
		void SetPressedColor(ColorRGBAf panelColor);

		Graphics::RenderUnit* GetRenderUnit(uint index) override;

		inline StringViewUTF8 GetText() const { return textContainer.GetString(); }
	private:						
		StringUTF8TextContainer textContainer;		

		bool hovered : 1;

		ColorRGBAf normalPanelColor = 0x333333ff;
		ColorRGBAf highlightedPanelColor = 0x404040ff;
		ColorRGBAf pressedPanelColor = 0x282828ff;
		ColorRGBAf unpressablePanelColor = 0x181818ff;

		ColorRGBAf normalTextColor = 0xf0f0f0ff;
		ColorRGBAf unpressableTextColor = 0xbbbbbbff;

		void UpdatePanelColor();
		
		void MouseHitStatusChangedEvent(const InputNode::MouseHitStatusChangedEvent& event);		
		void MouseButtonDownEvent(const InputNode::MouseButtonDownEvent& event);
		void MouseButtonUpEvent(const InputNode::MouseButtonUpEvent& event);		
		void PressableFlagChangedEvent(const ButtonBase::PressableFlagChangedEvent& event);
	public:
		PanelRenderUnit panelRenderUnit;
		TextRenderUnit textRenderUnit;
	};
}