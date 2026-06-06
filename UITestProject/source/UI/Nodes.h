#pragma once

using namespace UI::Nodes;

class LabeledSlider : public EmptyNode
{
public:
	Label label;
	Slider slider;

	LabeledSlider(Node& parent, const UI::NodeTransform& transform);
	LabeledSlider(Node& parent, const UI::NodeTransform& transform, u8StringView label, const TextStyle& textStyle, const SliderStyle& sliderStyle);
};

class LabeledButton : public EmptyNode
{
public:
	Label label;
	PanelButton button;

	LabeledButton();
	LabeledButton(Node& parent, const UI::NodeTransform& transform);
	LabeledButton(Node& parent, const UI::NodeTransform& transform, u8StringView label, const TextStyle& textStyle, const PanelButtonStyle& buttonStyle);
};

