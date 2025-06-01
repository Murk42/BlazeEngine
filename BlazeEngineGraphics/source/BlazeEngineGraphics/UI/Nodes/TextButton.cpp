#include "pch.h"
#include "BlazeEngineGraphics/UI/Nodes/TextButton.h"
#include "BlazeEngineGraphics/UI/Input/InputManager.h"

namespace Blaze::UI::Nodes
{
	TextButton::TextButton()
		: textRenderUnit(textContainer, this), panelRenderUnit(this), hovered(false)
	{
		dataMap.SetTypeName("TextButton");		

		UpdatePanelColor();
				
		panelRenderUnit.SetBorderWidth(0.0f);
		panelRenderUnit.SetCornerRadius(5.0f);

		textRenderUnit.SetCullingNode(this);		
		textRenderUnit.SetTextColor(normalTextColor);
		textRenderUnit.SetLayoutOptions(TextLayoutOptions{			
			.lineHorizontalAlign = HorizontalAlign::Center,
			.lineVerticalAlign = VerticalAlign::Center			
			});		

		mouseHitStatusChangedEventDispatcher.AddHandler<&TextButton::MouseHitStatusChangedEvent>(*this);
		mouseButtonDownEventDispatcher.AddHandler < &TextButton::MouseButtonDownEvent>(*this);
		mouseButtonUpEventDispatcher.AddHandler < &TextButton::MouseButtonUpEvent>(*this);
		pressableFlagChangedEventDispatcher.AddHandler < &TextButton::PressableFlagChangedEvent>(*this);		
	}
	TextButton::~TextButton()
	{
		mouseHitStatusChangedEventDispatcher.RemoveHandler<&TextButton::MouseHitStatusChangedEvent>(*this);
		mouseButtonDownEventDispatcher.RemoveHandler < &TextButton::MouseButtonDownEvent>(*this);
		mouseButtonUpEventDispatcher.RemoveHandler < &TextButton::MouseButtonUpEvent>(*this);
		pressableFlagChangedEventDispatcher.RemoveHandler < &TextButton::PressableFlagChangedEvent>(*this);
	}
	void TextButton::SetText(StringViewUTF8 text)
	{
		textContainer.SetString(text);
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
	void TextButton::MouseHitStatusChangedEvent(const InputNode::MouseHitStatusChangedEvent& event)
	{
		if (event.newHitStatus != 0 && event.oldHitStatus == 0)
		{
			hovered = true;
			UpdatePanelColor();
		}
		else if (event.newHitStatus == 0 && event.oldHitStatus != 0)
		{
			hovered = false;
			UpdatePanelColor(); //TODO hovered might not be needed, use GetHitStatus()
		}
	}	
	void TextButton::MouseButtonDownEvent(const InputNode::MouseButtonDownEvent& event)
	{					
		UpdatePanelColor();		
	}
	void Nodes::TextButton::MouseButtonUpEvent(const InputNode::MouseButtonUpEvent& event)
	{		
		UpdatePanelColor();		
	}
	void TextButton::PressableFlagChangedEvent(const ButtonBase::PressableFlagChangedEvent& event)
	{
		if (IsPressable())
			textRenderUnit.SetTextColor(normalTextColor);
		else
			textRenderUnit.SetTextColor(unpressableTextColor);

		UpdatePanelColor();
	}	
}