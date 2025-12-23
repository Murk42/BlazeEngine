#include "pch.h"
#include "MainScreen.h"
#include "SDL3/SDL.h"

MainScreen::MainScreen(ResourceManager& resourceManager)
	: resourceManager(resourceManager)
{
}

LabeledSlider::LabeledSlider()
{
}

LabeledSlider::LabeledSlider(Node& parent, const UI::NodeTransform& transform)
	: EmptyNode(parent, transform)
{
	slider.SetParent(this);
	slider.SetTransform({ .pos = { 2, 2 }, .parentPivot = {0, 0}, .pivot = {0, 0}, .size = {196, 12} });

	label.SetParent(&slider);
	label.SetTransform({ .pos = { 3, 3 }, .parentPivot = {0, 1}, .pivot = {0, 0} });
}

LabeledSlider::LabeledSlider(Node& parent, const UI::NodeTransform& transform, u8StringView label, const UI::FontAtlas& atlas)
	: LabeledSlider(parent, transform)
{
	this->label.BuildText(label, atlas);
}