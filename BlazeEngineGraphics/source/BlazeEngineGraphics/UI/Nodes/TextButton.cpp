#include "pch.h"
#include "BlazeEngineGraphics/UI/Nodes/TextButton.h"
#include "BlazeEngineGraphics/UI/Input/UIInputManager.h"

namespace Blaze::UI
{
	TextButton::TextButton() :
		panel(), text()
	{
		dataMap.SetTypeName("Button");
		finalTransformUpdatedEventDispatcher.AddHandler(*this);		

		panel.SetFillColor(ColorRGBAf(0x333333ff));
		panel.SetBorderColor(ColorRGBAf(0x111111ff));
		panel.SetBorderWidth(0.0f);
		panel.SetCornerRadius(5.0f);

		text.SetFontHeight(16);
		text.SetTransform({
			.pos = Vec2f(0, 0),
			.parentPivot = Vec2f(0.5f, 0.5f),
			.pivot = Vec2f(0.5f, 0.5f),
			//.size = transform.size,
			.scale = 1.0f,
			.rotation = 0.0f
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
		if (pressed)
			panel.SetFillColor(pressedColor);
		else if (hovered)
			panel.SetFillColor(highlightedColor);
		else
			panel.SetFillColor(normalColor);
	}
	Graphics::RenderUnit* TextButton::GetRenderUnit(uint index)
	{
		switch (index)
		{
		case 0:
			return panel.GetRenderUnit(0);
		case 1:
			return text.GetRenderUnit(0);
		default:
			return nullptr;
		}
	}
	void TextButton::OnEvent(Node::FinalTransformUpdatedEvent event)
	{
		auto transform = GetTransform();

		panel.SetTransform({
			.pos = Vec2f(0, 0),
			.parentPivot = Vec2f(0.5f, 0.5f),
			.pivot = Vec2f(0.5f, 0.5f),
			.size = transform.size,
			.scale = 1.0f,
			.rotation = 0.0f
			});		
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
		pressed = true;
		event.inputManager->SelectNode(this);
		GetScreen()->GetWindow()->keyReleasedDispatcher.AddHandler(*this);
		UpdateColor();

		pressedEventDispatcher.Call({ event.inputManager });
	}
	void TextButton::OnEvent(SelectedEvent event)
	{
		UpdateColor();
	}
	void TextButton::OnEvent(DeselectedEvent event)
	{
		pressed = false;
		UpdateColor();
	}
	void TextButton::OnEvent(Input::Events::KeyReleased event)
	{
		pressed = false;
		UpdateColor();

		EventHandler<Input::Events::KeyReleased>::UnsubscribeFromDispatcher();
	}
}