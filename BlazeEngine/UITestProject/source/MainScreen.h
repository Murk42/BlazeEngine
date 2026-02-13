#pragma once

using namespace UI::Nodes;

class LabeledSlider : public EmptyNode
{
public:
	Label label;
	Slider slider;

	LabeledSlider(Node& parent, const UI::NodeTransform& transform);
	LabeledSlider(Node& parent, const UI::NodeTransform& transform, u8StringView label, const UI::TextStyle& textStyle, const Slider::Style& sliderStyle);
};

class LabeledButton : public EmptyNode
{
public:
	Label label;
	PanelButton button;

	LabeledButton();
	LabeledButton(Node& parent, const UI::NodeTransform& transform);
	LabeledButton(Node& parent, const UI::NodeTransform& transform, u8StringView label, const UI::TextStyle& textStyle, const PanelButton::Style& buttonStyle);
};

class MainScreen : public UI::Screen
{
public:
	ResourceManager& resourceManager;

	ResourceRef< Graphics::OpenGL::Texture2D> texture = resourceManager.LoadResource<Graphics::OpenGL::Texture2D>("", "assets/images/Mountains.jpg", Graphics::OpenGL::TextureInternalPixelFormat::SRGB24);

	UI::TextStyle textStyle0{ "default", 32.0f, 0x551111ff };
	UI::TextStyle textStyle1{ "default", 20.0f };
	UI::TextStyle textStyle2{ "default", 6.0f };
	Panel::Style headerPanelStyle{
		.fillColor = 0x003000ff,
		.borderColor = 0xffffffff,
		.borderWidth = 0.0f,
		.cornerRadius = 0.0f
	};
	Panel::Style contentPanelStyle{
		.fillColor = 0x222222ff,
		.borderColor = 0xffffffff,
		.borderWidth = 0.0f,
		.cornerRadius = 0.0f
	};
	Panel::Style sideBarPanelStyle{
		.fillColor = 0x111111ff,
		.borderColor = 0xffffffff,
		.borderWidth = 0.0f,
		.cornerRadius = 0.0f
	};
	Slider::Style sliderStyle{ };
	PanelButton::Style panelButtonStyle{ .panelStyle = { .borderWidth = 0, .cornerRadius = 3 } };

	//UI::Layout::HorizontalDivisonNode divisionNode{ *this, 1 };
	//UI::Layout::ParentSizeBinding divisionNodeSizeBinding{ &divisionNode };

	Label text{ *this, {.pos = { 5, -5 }, .parentPivot = {0, 1}, .pivot = {0, 1}}, textStyle0, "Hello, BlazeEngineUI!" };
	PanelButton button{ text, {.pos = { 5, 0 }, .parentPivot = { 1, 0.5 }, .pivot = { 0, 0.5 }, .size = { 200, 40 } }, {} };
	
	LabeledSlider slider{ *this, {.size = { 200, 50 } }, "Slider label A", textStyle1, sliderStyle };
	
	Panel panel{ *this, {.pos = { -5, 5 }, .parentPivot = { 1, 0}, .pivot = { 1, 0}, .size = {210, 210}} };
	Image image{ panel, {.size = { 200, 200} }, texture, {0,0}, {1,1}, {.layout = ImageLayout::Fit } };
	
	PanelButton outsideWindowButton{ *this, {.parentPivot = { 0, 0.5}, .pivot = { 0.5, 0.5}, .size = { 200, 40 } } };
	Slider outsideWindowSlider{ *this, { .pos = { 0, -30}, .parentPivot = {0, 0.5}, .pivot = {0.5, 0.5}, .size = {200, 12}}};
	
	PanelButton bottomButton{ *this, { .pos = {100, 20}, .parentPivot = {0.5, 0}, .pivot = {0.5, 0}, .size = {200, 40}}};
	
	Label tinyLabel{ text, UI::NodeTransform{.pos = { 0, -5}, .parentPivot = {0.5, 0}, .pivot = {0.5, 1}, .size = {200, 40} }, textStyle2, "This is a really small text :D" };
	Label blinkingLabel{ outsideWindowButton, UI::NodeTransform{.pos = { -40, 0}, .parentPivot = {1, 0}, .pivot = {0.5, 0.5}, .size = {200, 40}, .rotation = Math::PI / 2}, textStyle1, "This label is blinking " };

	MainScreen(ResourceManager& resourceManager);
};