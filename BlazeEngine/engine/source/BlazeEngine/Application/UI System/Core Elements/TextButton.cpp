#include "BlazeEngine/Application/UI System/Core Elements/TextButton.h"

namespace Blaze::UI
{	

	TextButton::TextButton()
	{
		TieElement(&panel);
		TieElement(&text);
		TieElement(&button);

		normalColor = 0x888888ff;
		hoveredColor = 0x808080ff;
		pressedColor = 0x707070ff;
		panel.SetAnchor(this);
		text.SetAnchor(&panel);
		button.SetAnchor(&panel);

		text.SetSolidFlag(false);
		button.SetSolidFlag(false);
		panel.SetSolidFlag(false);

		button.pressed += [this]() {
			panel.fillColor = pressedColor;
		};
		button.released += [this]() {
			panel.fillColor = hoveredColor;
		};
		button.entered += [this]() {
			panel.fillColor = hoveredColor;
		};
		button.left += [this]() {
			panel.fillColor = normalColor;
		};

		text.SetClipElement( this);
		panel.SetClipElement(this);

		panel.fillColor = normalColor;		
	}

	TextButton::~TextButton()
	{
	}

	void TextButton::SetSize(Vec2f size)
	{
		panel.SetSize(size);
		button.SetSize(size);
		UIElement::SetSize(size);
	}	

	void TextButton::SetActiveFlag(bool active)
	{
		panel.SetActiveFlag(active);
		button.SetActiveFlag(active);
		text.SetActiveFlag(active);
		UIElement::SetActiveFlag(active);
	}

	void TextButton::SetDepth(float value)
	{
		panel.SetDepth(value);
		button.SetDepth(value);
		text.SetDepth(value);
		UIElement::SetDepth(value);
	}

	void TextButton::SetNormalColor(ColorRGBAf color)
	{ 
		normalColor = color; 

		if (button.GetState() == ButtonState::None)
			panel.fillColor = color;
	}
	void TextButton::SetPressedColor(ColorRGBAf color)
	{ 
		pressedColor = color; 

		if (button.GetState() == ButtonState::Down)
			panel.fillColor = color;
	}
	void TextButton::SetHoveredColor(ColorRGBAf color)
	{ 
		hoveredColor = color; 

		if (button.GetState() == ButtonState::Hovered)
			panel.fillColor = color;
	}

	void TextButton::SetTextAlignment(Align local, Align anchor, Vec2f offset)
	{
		text.SetLocalAlignment(local); 
		text.SetAnchorAlignment(anchor);
		text.SetPos(offset);
	}

	void TextButton::SetTextLocalAlignment(Align align)
	{
		text.SetLocalAlignment(align);
	}

	void TextButton::SetTextAnchorAlignment(Align align)
	{
		text.SetAnchorAlignment(align);
	}

	void TextButton::SetTextOffset(Vec2f offset)
	{
		text.SetPos(offset);
	}	

	void TextButton::SetProperties(const TextButtonProperties& p)
	{
		button.SetProperties(p.buttonProperties);
		text.SetProperties(p.textProperties);
		panel.SetProperties(p.panelProperties);
		
		if (p.normalColor.edited) normalColor = p.normalColor;
		if (p.hoveredColor.edited) hoveredColor = p.hoveredColor;
		if (p.pressedColor.edited) pressedColor = p.pressedColor;

		if (p.textLocalAlign.edited) text.SetLocalAlignment(p.textLocalAlign);
		if (p.textAnchorAlign.edited) text.SetAnchorAlignment(p.textAnchorAlign);
		if (p.textOffset.edited) text.SetPos(p.textOffset);
	}

	void TextButtonManager::Render(UIElement* el)
	{
		TextButton& textButton = *(TextButton*)el;
		GetManager()->GetElementManager<Panel>().Render(&textButton.panel);
		GetManager()->GetElementManager<Text>().Render(&textButton.text);
		GetManager()->GetElementManager<Button>().Render(&textButton.button);
	}
	void TextButtonManager::Update(UIElement* el)
	{
		TextButton& textButton = *(TextButton*)el;
		GetManager()->GetElementManager<Button>().Update(&textButton.button);
		GetManager()->GetElementManager<Text>().Update(&textButton.text);
		GetManager()->GetElementManager<Panel>().Update(&textButton.panel);
	}
}