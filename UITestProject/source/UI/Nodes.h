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

class NumberTextInput : public TextInput
{
public:
	NumberTextInput();
	NumberTextInput(Node& parent, const UI::NodeTransform& transform);
	NumberTextInput(Node& parent, const UI::NodeTransform& transform, const TextStyle& textStyle, u8String text);

	float GetValue() const { return value; }
private:
	float value;

	bool FilterEnteredText(u8StringView text) override;
};