#pragma once
#include <UI/Nodes.h>

using namespace UI::Nodes;

class MainScreen : public UI::Screen
{
public:
	ResourceManager& resourceManager;
	UI::FontManager& fontManager;

	ResourceRef< Graphics::OpenGL::Texture2D> texture = resourceManager.LoadNamedResource<Graphics::OpenGL::Texture2D>("", "assets/images/Mountains.jpg", Graphics::OpenGL::TextureInternalPixelFormat::SRGB24);

	TextStyle textStyle32{ &fontManager, "default", 32, 0x551111ff_rgba };
	TextStyle textStyle20{ &fontManager, "default", 18 };
	TextStyle textStyleInputedText { &fontManager, "default", 18 };
	TextStyle textStyle12{ &fontManager, "default", 12 };
	TextStyle textStyle6{ &fontManager, "default", 6 };
	PanelStyle linePanelStyle{
		.fillColor = 0x080808ff_rgba,
		.borderColor = 0x080808ff_rgba,
		.borderWidth = 0.0f,
		.cornerRadius = 0.0f
	};
	SliderStyle sliderStyle{ };
	PanelButtonStyle panelButtonStyle{ .panelStyle = { .borderWidth = 0, .cornerRadius = 3 } };	

	Label titleText { *this, {.position = { 10, -2 }, .parentPivot = {0, 1}, .pivot = {0, 1}}, textStyle32, "Hello, BlazeEngineUI!" };

	Label explanationText{ *this, {.position = { 10, -45 }, .parentPivot = {0, 1}, .pivot = {0, 1}, .size = { 800, 200 } }, textStyle12,
		u8"Hello, this is a long text, it's testing the UI multiline and text wrapping capability.\n"
		u8"The previous sentance is in one line but could be wrapped because it is long. In this line there will be two senteances so maybe it will get wrapped multiple times\n"
		u8"\n"
		u8"This is a separate paragraph\n"
		u8"\n"
		u8"Може да се пише и текст на ћирилици, ови карактери заузимају по два бајта сваки и захтевају посебно третирање.\n"
		u8"Баш сам хтео да подржавам Unicode и ћирилицу.\n"
	};
	
	Image image{ explanationText, { .position = { 5, 0 }, .parentPivot = { 1, 1}, .pivot = { 0, 1 }, .size = {240, 240/16*9}}, {.texture = texture }, ImageLayout::Fit};
	
	
	//Label tinyLabel{ titleText, UI::NodeTransform{.position = { 0, -5}, .parentPivot = {0.5, 0}, .pivot = {0.5, 1}, .size = {200, 40} }, textStyle3, "This is a really small text :D" };

	Label windowTitleLabel{ *this, {.position = { 10, -245 }, .parentPivot = { 0, 1 }, .pivot = { 0, 1 }, .size = { 200, 20} }, textStyle20, "Window title:" };
	Panel windowTitleDecoration{ windowTitleLabel, {.position = { 4, -5 }, .parentPivot = { 1, 0 }, .pivot = { 0, 1 }, .size = { 206, 2 } }, linePanelStyle };
	TextInput windowTitleTextInput{ windowTitleLabel, {.position = { 5, 0 }, .parentPivot = { 1, 0.5 }, .pivot = { 0, 0.5f }, .size = { 200, 20} }, textStyleInputedText, "Enter text"};
	
	//Label label{ *this, {.position = { 0, 30 }, .parentPivot = { 0.5f, 0.5 }, .pivot = { 0.5f, 0.5f }, .size = { 200, 20} }, textStyle1, "Enter a float value:" };
	//Panel textInputPanel{ *this, {.position = { 0, -30 }, .parentPivot = { 0.5f, 0.5 }, .pivot = { 0.5f, 0.5f }, .size = { 206, 26} }, contentPanelStyle };
	//FloatInput numberInput{ textInputPanel, {.position = { 0, 2 }, .parentPivot = { 0.5f, 0.5 }, .pivot = { 0.5f, 0.5f }, .size = { 200, 20} }, textStyle1, 1.032f };

	MainScreen(ResourceManager& resourceManager, UI::FontManager& fontManager);
};