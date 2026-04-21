#pragma once
#include <UI/Nodes.h>

using namespace UI::Nodes;

class MainScreen : public UI::Screen
{
public:
	ResourceManager& resourceManager;
	UI::FontManager& fontManager;

	ResourceRef< Graphics::OpenGL::Texture2D> texture = resourceManager.LoadNamedResource<Graphics::OpenGL::Texture2D>("", "assets/images/Mountains.jpg", Graphics::OpenGL::TextureInternalPixelFormat::SRGB24);

	TextStyle textStyle0{ &fontManager, "default", 32.0f, 0x551111ff };
	TextStyle textStyle1{ &fontManager, "default", 20.0f };
	TextStyle textStyle2{ &fontManager, "default", 12.0f };
	TextStyle textStyle3{ &fontManager, "default", 6.0f };
	PanelStyle headerPanelStyle{
		.fillColor = 0x003000ff,
		.borderColor = 0xffffffff,
		.borderWidth = 0.0f,
		.cornerRadius = 0.0f
	};
	PanelStyle contentPanelStyle{
		.fillColor = 0x222222ff,
		.borderColor = 0x606060ff,
		.borderWidth = 1.0f,
		.cornerRadius = 2.0f
	};
	PanelStyle sideBarPanelStyle{
		.fillColor = 0x111111ff,
		.borderColor = 0xffffffff,
		.borderWidth = 0.0f,
		.cornerRadius = 0.0f
	};
	SliderStyle sliderStyle{ };
	PanelButtonStyle panelButtonStyle{ .panelStyle = { .borderWidth = 0, .cornerRadius = 3 } };	

	Label text{ *this, {.position = { 5, -5 }, .parentPivot = {0, 1}, .pivot = {0, 1}}, textStyle0, "Hello, BlazeEngineUI!" };
	PanelButton button{ text, {.position = { 5, 0 }, .parentPivot = { 1, 0.5 }, .pivot = { 0, 0.5 }, .size = { 200, 40 } }, {} };

	Label longText{ *this, {.position = { 150, -100 }, .parentPivot = {0, 1}, .pivot = {0, 1}}, textStyle2, 
		u8"Hello, this is a long text, it's testing the UI multiline and text wrapping capability.\n"
		u8"The previous sentance is in one line but could be wrapped because it is long. In this line there will be two senteances so maybe it will get wrapped multiple times\n"
		u8"\n"
		u8"This is a separate paragraph\n"
		u8"\n"
		u8"Може да се пише и текст на ћирилици, ови карактери заузимају по два бајта сваки и захтевају посебно третирање.\n"
		u8"Баш сам хтео да подржавам Unicode и ћирилицу.\n"
	};
	
	LabeledSlider slider{ *this, {.parentPivot { 1, 0.5}, .pivot = { 1, 0.5 }, .size = {200, 50}}, "Slider label A", textStyle1, sliderStyle };
	
	Panel panel{ *this, {.position = { -5, 5 }, .parentPivot = { 1, 0}, .pivot = { 1, 0}, .size = {210, 210}} };
	Image image{ panel, {.size = { 200, 200} }, { .texture = texture }, ImageLayout::Fit };
	
	PanelButton outsideWindowButton{ *this, {.parentPivot = { 0, 0.5}, .pivot = { 0.5, 0.5}, .size = { 200, 40 } } };
	Slider outsideWindowSlider{ *this, { .position = { 0, -30}, .parentPivot = {0, 0.5}, .pivot = {0.5, 0.5}, .size = {200, 12}}};
	
	PanelButton bottomButton{ *this, { .position = {100, 20}, .parentPivot = {0.5, 0}, .pivot = {0.5, 0}, .size = {200, 40}}};
	
	Label tinyLabel{ text, UI::NodeTransform{.position = { 0, -5}, .parentPivot = {0.5, 0}, .pivot = {0.5, 1}, .size = {200, 40} }, textStyle3, "This is a really small text :D" };
	Label blinkingLabel{ outsideWindowButton, UI::NodeTransform{.position = { -40, 0}, .parentPivot = {1, 0}, .pivot = {0.5, 0.5}, .size = {200, 40}, .rotation = (float)Math::PI / 2}, textStyle1, "This label is blinking " };

	Panel textInputPanel{ *this, {.position = { 0, 0 }, .parentPivot = { 0.5f, 0.5 }, .pivot = { 0.5f, 0.5f }, .size = { 210, 30} }, contentPanelStyle };
	NumberTextInput textInput{ textInputPanel, {.position = { 0, 2 }, .parentPivot = { 0.5f, 0.5 }, .pivot = { 0.5f, 0.5f }, .size = { 200, 20} }, textStyle1, "Text input" };

	MainScreen(ResourceManager& resourceManager, UI::FontManager& fontManager);
};