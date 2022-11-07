#include "BlazeEngine/Application/UI System/Core Elements/ImageButton.h"

namespace Blaze::UI
{

	ImageButton::ImageButton()
	{
		TieElement(&panel);
		TieElement(&image);
		TieElement(&button);

		panel.SetAnchor(this);
		image.SetAnchor(&panel);
		button.SetAnchor(&panel);

		image.SetSolidFlag(false);
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

		panel.fillColor = normalColor;

		SetProperties({});
	}

	ImageButton::~ImageButton()
	{
	}

	void ImageButton::SetSize(Vec2f size)
	{
		panel.SetSize(size);
		button.SetSize(size);
		UIElement::SetSize(size);
	}	

	void ImageButton::SetActiveFlag(bool active)
	{
		panel.SetActiveFlag(active);
		button.SetActiveFlag(active);
		image.SetActiveFlag(active);
		UIElement::SetActiveFlag(active);
	}

	void ImageButton::SetDepth(float value)
	{
		panel.SetDepth(value);
		button.SetDepth(value);
		image.SetDepth(value);
		UIElement::SetDepth(value);
	}

	void ImageButton::SetNormalColor(ColorRGBAf color)
	{ 
		normalColor = color; 

		if (button.GetState() == ButtonState::None)
			panel.fillColor = color;
	}
	void ImageButton::SetPressedColor(ColorRGBAf color)
	{ 
		pressedColor = color; 

		if (button.GetState() == ButtonState::Down)
			panel.fillColor = color;
	}
	void ImageButton::SetHoveredColor(ColorRGBAf color)
	{ 
		hoveredColor = color; 

		if (button.GetState() == ButtonState::Hovered)
			panel.fillColor = color;
	}	

	void ImageButton::SetProperties(const ImageButtonProperties& p)
	{
		SetPos(p.pos);
		SetSize(p.size);
		SetAnchor(p.anchor);
		SetLocalAlignment(p.localAlign);
		SetAnchorAlignment(p.anchorAlign);
		SetClickableFlag(p.clickable);
		SetActiveFlag(p.active);
		SetBorderWidth(p.borderWidth);
		SetCornerRadius(p.cornerRadius);
		SetNormalColor(p.normalColor);
		SetHoveredColor(p.hoveredColor);
		SetPressedColor(p.pressedColor);
		SetBorderColor(p.borderColor);
		SetImageTexture(p.texture);
		SetImageSoruceRect(p.sourceRect);
		SetImageAnchorAlignment(p.imageAnchorAlign);
		SetImageLocalAlignment(p.imageLocalAlign);
		SetImageOffset(p.imageOffset);
		SetImageSize(p.imageSize);
		SetImageMask(p.imageMask);
		button.pressed += p.pressed;
		button.released += p.released;
		button.entered += p.entered;
		button.left += p.left;		
	}
	void ImageButtonManager::Render(UIElement* el)
	{
		ImageButton& imageButton = *(ImageButton*)el;
		GetManager()->GetElementManager<Panel>().Render(&imageButton.panel);
		GetManager()->GetElementManager<Image>().Render(&imageButton.image);
		GetManager()->GetElementManager<Button>().Render(&imageButton.button);
	}
	void ImageButtonManager::Update(UIElement* el)
	{
		ImageButton& imageButton = *(ImageButton*)el;
		GetManager()->GetElementManager<Panel>().Update(&imageButton.panel);
		GetManager()->GetElementManager<Image>().Update(&imageButton.image);
		GetManager()->GetElementManager<Button>().Update(&imageButton.button);
	}
}