#include "pch.h"
#include "MainScreen.h"
#include "SDL3/SDL.h"

MainScreen::MainScreen(Window* window, ResourceManager& resourceManager)
	: Screen(window)
{
	button.SetPressedEventCallback([&](auto event) {
		sliders2.SetEnabledFlag(!sliders2.IsEnabled());
		sliders4.SetEnabledFlag(!sliders4.IsEnabled());
		});
}

LabeledSlider::LabeledSlider()
{
	SetTransform({ .size = { 200, 30 } });
	slider.SetParent(this);
	slider.SetTransform({ .parentPivot = { 0, 0.5 }, .pivot = { 0, 0.5 }, .size = { 200, 10 } });

	label.SetParent(&slider);
	label.SetTransform({ .pos = { 5, 5}, .parentPivot = {0, 1}, .pivot = {0, 0}});
}

LabeledSlider::LabeledSlider(Node& parent, u8StringView label, const UI::FontAtlas& atlas)
	: LabeledSlider()
{
	SetParent(&parent);
	this->label.BuildText(label, atlas);
}