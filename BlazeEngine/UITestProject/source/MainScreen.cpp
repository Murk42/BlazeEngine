#include "pch.h"
#include "MainScreen.h"

MainScreen::MainScreen(ResourceManager& resourceManager)
	: Screen(resourceManager), resourceManager(resourceManager)
{
	//slider.slider.SetValueChangedCallback([&](float value) {
	//	auto textStyle = slider.label.GetTextStyle();		
	//	textStyle.fontHeight = 20 * (1 + value * 2);
	//	slider.label.SetTextStyle(textStyle);
	//	});
	//
	//slider.label.SetWrapWidth(200);
}

LabeledSlider::LabeledSlider(Node& parent, const UI::NodeTransform& transform)
	: EmptyNode(parent, transform)
{
}

LabeledSlider::LabeledSlider(Node& parent, const UI::NodeTransform& transform, u8StringView labelText, const UI::TextStyle& textStyle, const Slider::Style& sliderStyle)
	: LabeledSlider(parent, transform)
{
	slider.SetParent(this);
	slider.SetTransform({ .pos = { 2, 2 }, .parentPivot = {0, 0}, .pivot = {0, 0}, .size = {196, 12} });
	slider.SetStyle(sliderStyle);

	label.SetParent(this);
	label.SetTransform({ .pos = { 3, -3 }, .parentPivot = {0, 1}, .pivot = {0, 1} });
	label.SetText(labelText);
	label.SetTextStyle(textStyle);
}

LabeledButton::LabeledButton()
{
	button.SetParent(this);
	button.SetTransform({ .pos = { 0, 0 }, .parentPivot = {0.5, 0.5}, .pivot = {0.5, 0.5} });
	label.SetParent(this);
	label.SetTransform({ .pos = { 0, 3}, .parentPivot = {0.5, 0.5}, .pivot = {0.5, 0.5} });
	label.SetBlocksHitTestFlag(false);
}

LabeledButton::LabeledButton(Node& parent, const UI::NodeTransform& transform)
	: EmptyNode(parent, transform)
{
	button.SetParent(this);
	button.SetTransform({ .pos = { 0, 0 }, .parentPivot = {0.5, 0.5}, .pivot = {0.5, 0.5}, .size = transform.size });
	label.SetParent(this);
	label.SetTransform({ .pos = { 0, 3}, .parentPivot = {0.5, 0.5}, .pivot = {0.5, 0.5} });
	label.SetBlocksHitTestFlag(false);
}

LabeledButton::LabeledButton(Node& parent, const UI::NodeTransform& transform, u8StringView labelText, const UI::TextStyle& textStyle, const PanelButton::Style& buttonStyle)
	: LabeledButton(parent, transform)
{
	button.SetStyle(buttonStyle);
	label.SetText(labelText);
	label.SetTextStyle(textStyle);
}