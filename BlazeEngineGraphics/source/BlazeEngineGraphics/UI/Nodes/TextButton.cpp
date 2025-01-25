#include "pch.h"
#include "BlazeEngineGraphics/UI/Nodes/TextButton.h"
#include "BlazeEngineGraphics/UI/Input/UIInputManager.h"

namespace Blaze::UI::Nodes
{
	TextButton::TextButton()
		: textRenderUnit(this), panelRenderUnit(this), hovered(false)
	{
		dataMap.SetTypeName("TextButton");		

		panelRenderUnit.SetFillColor(normalColor);		
		panelRenderUnit.SetBorderWidth(0.0f);
		panelRenderUnit.SetCornerRadius(5.0f);

		textRenderUnit.SetCullingNode(this);		
		textRenderUnit.SetFontHeight(16);
		textRenderUnit.SetLayoutOptions(TextLayoutOptions{			
			.lineHorizontalAlign = TextLineHorizontalAlign::Center,
			.lineVerticalAlign = TextLineVerticalAlign::Center			
			});		
	}
	TextButton::~TextButton()
	{
	}
	void TextButton::SetNormalColor(ColorRGBAf color)
	{ 		
		normalColor = color;
		UpdateColor();
	}
	void TextButton::SetHighlightedColor(ColorRGBAf color)
	{
		highlightedColor = color;
		UpdateColor();
	}
	void TextButton::SetPressedColor(ColorRGBAf color)
	{
		pressedColor = color;
		UpdateColor();
	}
	void TextButton::UpdateColor()
	{
		if (IsPressed())
			panelRenderUnit.SetFillColor(pressedColor);
		else if (hovered)
			panelRenderUnit.SetFillColor(highlightedColor);
		else
			panelRenderUnit.SetFillColor(normalColor);
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
	void TextButton::OnEvent(MouseEnterEvent event)
	{
		hovered = true;
		UpdateColor();
	}
	void TextButton::OnEvent(MouseExitEvent event)
	{
		hovered = false;
		UpdateColor();
	}
	void TextButton::OnEvent(MousePressedEvent event)
	{		
		ButtonBase::OnEvent(event);			
		UpdateColor();		
	}
	void Nodes::TextButton::OnEvent(MouseReleasedEvent event)
	{
		ButtonBase::OnEvent(event);
		UpdateColor();		
	}
	void TextButton::OnEvent(DeselectedEvent event)
	{
		ButtonBase::OnEvent(event);		
		UpdateColor();
	}	
}