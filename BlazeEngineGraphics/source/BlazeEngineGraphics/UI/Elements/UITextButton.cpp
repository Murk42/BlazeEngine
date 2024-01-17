#include "pch.h"
#include "BlazeEngineGraphics/UI/Elements/UITextButton.h"
#include "BlazeEngineGraphics/UI/Input/UIInputManager.h"

namespace Blaze::UI
{
	TextButton::TextButton(Node* parent, const NodeTransform& transform)
		: UIInputNode(parent, transform), panel(this, {}), text(&panel, {})
	{
		transformUpdatedEventDispatcher.AddHandler(*this);		

		panel.fillColor = ColorRGBAf(0x333333ff);
		panel.borderColor = ColorRGBAf(0x111111ff);
		panel.borderWidth = 0.0f;
		panel.cornerRadius = 5.0f;

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
			panel.fillColor = pressedColor;
		else if (hovered)
			panel.fillColor = highlightedColor;
		else
			panel.fillColor = normalColor;
	}
	void TextButton::OnEvent(NodeTransformUpdatedEvent event)
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
		event.inputManager->GetWindow()->keyReleasedDispatcher.AddHandler(*this);
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