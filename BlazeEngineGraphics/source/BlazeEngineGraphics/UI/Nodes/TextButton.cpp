#include "pch.h"
#include "BlazeEngineGraphics/UI/Nodes/TextButton.h"
#include "BlazeEngineGraphics/UI/Input/InputManager.h"

namespace Blaze::UI::Nodes
{
	TextButton::TextButton()
		: textRenderUnit(this), panelRenderUnit(this), hovered(false)
	{
		dataMap.SetTypeName("TextButton");		

		UpdatePanelColor();
				
		panelRenderUnit.SetBorderWidth(0.0f);
		panelRenderUnit.SetCornerRadius(5.0f);

		textRenderUnit.SetCullingNode(this);		
		textRenderUnit.SetTextColor(normalTextColor);
		textRenderUnit.SetFontHeight(16);
		textRenderUnit.SetLayoutOptions(TextLayoutOptions{			
			.lineHorizontalAlign = TextLineHorizontalAlign::Center,
			.lineVerticalAlign = TextLineVerticalAlign::Center			
			});		

		pressableFlagChangedEventDispatcher.AddHandler(*this);
	}
	TextButton::~TextButton()
	{
		pressableFlagChangedEventDispatcher.RemoveHandler(*this);
	}
	void TextButton::SetNormalColor(ColorRGBAf color)
	{ 		
		normalPanelColor = color;
		UpdatePanelColor();
	}
	void TextButton::SetHighlightedColor(ColorRGBAf color)
	{
		highlightedPanelColor = color;
		UpdatePanelColor();
	}
	void TextButton::SetPressedColor(ColorRGBAf color)
	{
		pressedPanelColor = color;
		UpdatePanelColor();
	}
	void TextButton::UpdatePanelColor()
	{
		if (!IsPressable())		
			panelRenderUnit.SetFillColor(unpressablePanelColor);					
		else if (IsPressed())
			panelRenderUnit.SetFillColor(pressedPanelColor);
		else if (hovered)
			panelRenderUnit.SetFillColor(highlightedPanelColor);
		else
			panelRenderUnit.SetFillColor(normalPanelColor);		
	}
	Graphics::RenderUnit* TextButton::GetRenderUnit(uint index)
	{
		switch (index)
		{
		case 0:
			return &panelRenderUnit;
		case 1:
			return &textRenderUnit;
		default:
			return nullptr;
		}
	}	
	void TextButton::OnEvent(const MouseEnterEvent& event)
	{
		hovered = true;
		UpdatePanelColor();
	}
	void TextButton::OnEvent(const MouseExitEvent& event)
	{
		hovered = false;
		UpdatePanelColor();
	}
	void TextButton::OnEvent(const MouseButtonDownEvent& event)
	{		
		ButtonBase::OnEvent(event);			
		UpdatePanelColor();		
	}
	void Nodes::TextButton::OnEvent(const MouseButtonUpEvent& event)
	{
		ButtonBase::OnEvent(event);
		UpdatePanelColor();		
	}
	void TextButton::OnEvent(const DeselectedEvent& event)
	{
		ButtonBase::OnEvent(event);		
		UpdatePanelColor();
	}
	void TextButton::OnEvent(const PressableFlagChangedEvent& event)
	{
		if (IsPressable())
			textRenderUnit.SetTextColor(normalTextColor);
		else
			textRenderUnit.SetTextColor(unpressableTextColor);

		UpdatePanelColor();
	}

}