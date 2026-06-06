#include "pch.h"
#include "Nodes.h"

LabeledSlider::LabeledSlider(Node& parent, const UI::NodeTransform& transform)
	: EmptyNode(parent, transform)
{
}

LabeledSlider::LabeledSlider(Node& parent, const UI::NodeTransform& transform, u8StringView labelText, const TextStyle& textStyle, const SliderStyle& sliderStyle)
	: LabeledSlider(parent, transform)
{
	slider.SetParent(this);
	slider.SetTransform({ .position = { 2, 2 }, .parentPivot = {0, 0}, .pivot = {0, 0}, .size = {196, 12} });
	slider.SetStyle(sliderStyle);

	label.SetParent(this);
	label.SetTransform({ .position = { 3, -3 }, .parentPivot = {0, 1}, .pivot = {0, 1} });
	label.SetText(labelText);
	label.SetTextStyle(textStyle);
}

LabeledButton::LabeledButton()
{
	button.SetParent(this);
	button.SetTransform({ .position = { 0, 0 }, .parentPivot = {0.5, 0.5}, .pivot = {0.5, 0.5} });
	label.SetParent(this);
	label.SetTransform({ .position = { 0, 3}, .parentPivot = {0.5, 0.5}, .pivot = {0.5, 0.5} });
	label.SetBlocksHitTestFlag(false);
}

LabeledButton::LabeledButton(Node& parent, const UI::NodeTransform& transform)
	: EmptyNode(parent, transform)
{
	button.SetParent(this);
	button.SetTransform({ .position = { 0, 0 }, .parentPivot = {0.5, 0.5}, .pivot = {0.5, 0.5}, .size = transform.size });
	label.SetParent(this);
	label.SetTransform({ .position = { 0, 3}, .parentPivot = {0.5, 0.5}, .pivot = {0.5, 0.5} });
	label.SetBlocksHitTestFlag(false);
}

LabeledButton::LabeledButton(Node& parent, const UI::NodeTransform& transform, u8StringView labelText, const TextStyle& textStyle, const PanelButtonStyle& buttonStyle)
	: LabeledButton(parent, transform)
{
	button.SetStyle(buttonStyle);
	label.SetText(labelText);
	label.SetTextStyle(textStyle);
}