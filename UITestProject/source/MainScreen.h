#pragma once
#include "LayoutControl.h"

class LabeledSlider : public UI::Nodes::EmptyNode
{
public:
	UI::Nodes::Label label;
	UI::Nodes::Slider slider;

	LabeledSlider();
	LabeledSlider(Node& parent, u8StringView label, const UI::FontAtlas& atlas);
};

class MainScreen : public UI::Screen
{
public:
	ResourceManager& resourceManager;
	ResourceRef<UI::FontAtlas> fontAtlas0 = resourceManager.GetResource<UI::FontAtlas>("fontAtlas32");
	ResourceRef<UI::FontAtlas> fontAtlas1 = resourceManager.GetResource<UI::FontAtlas>("fontAtlas20");

	ResourceRef< Graphics::OpenGL::Texture2D> texture = resourceManager.LoadResource<Graphics::OpenGL::Texture2D>("", "assets/images/Mountains.jpg");

	UI::Nodes::Panel::Style headerPanelStyle{
		.fillColor = 0x003000ff,
		.borderColor = 0xffffffff,
		.borderWidth = 0.0f,
		.cornerRadius = 0.0f
	};
	UI::Nodes::Panel::Style contentPanelStyle{
		.fillColor = 0x222222ff,
		.borderColor = 0xffffffff,
		.borderWidth = 0.0f,
		.cornerRadius = 0.0f
	};
	UI::Nodes::Panel::Style sideBarPanelStyle{
		.fillColor = 0x111111ff,
		.borderColor = 0xffffffff,
		.borderWidth = 0.0f,
		.cornerRadius = 0.0f
	};

	UI::Layout::HorizontalDivisonNode divisionNode{ *this, 1 };
	UI::Layout::ParentSizeBinding divisionNodeSizeBinding{ &divisionNode };

		UI::Nodes::Panel headerNode{ divisionNode, {.size = { 0, 50}}, headerPanelStyle };

			UI::Nodes::Label text{ headerNode, {.pos = { 5, 0 }, .parentPivot = {0, .5}, .pivot = {0, .5}}, "Hello, BlazeEngineUI!", *fontAtlas0 };
			UI::Nodes::PanelButton button{ text, {.pos = { 5, 0 }, .parentPivot = { 1, 0.5 }, .pivot = { 0, 0.5 }, .size = { 200, 40 } }, {} };


	UI::Layout::VerticalDivisonNode sidebarDivisionNode{ divisionNode, 1 };

		UI::Nodes::Panel sidebar{ sidebarDivisionNode, {}, sideBarPanelStyle };
		UI::Nodes::Panel contentNode{ sidebarDivisionNode, {}, contentPanelStyle };
			UI::Layout::VerticalArrayLayoutNode slidersLayout{ contentNode, {.pos = { 0, 0}, .parentPivot = {0, 1}, .pivot = {0, 1}, .size = { 100, 300 }},  5.0f, Align::BottomLeft };
			UI::Layout::ParentSizeBinding  slidersLayoutSizeBinding{ &slidersLayout };

					LabeledSlider sliders1 { slidersLayout, "Slider label A", *fontAtlas1 };
					LabeledSlider sliders2 { slidersLayout, "Slider label B", *fontAtlas1 };
					LabeledSlider sliders3 { slidersLayout, "Slider label C", *fontAtlas1 };
					LabeledSlider sliders4 { slidersLayout, "Slider label D", *fontAtlas1 };


			UI::Nodes::Panel panel{ contentNode, {.pos = { -5, 5 }, .parentPivot = { 1, 0}, .pivot = { 1, 0}, .size = {210, 210}} , {} };
			UI::Nodes::Image image{ panel, {.size = { 200, 200} }, texture, {0,0}, {1,1}, {.layout = UI::Nodes::ImageLayout::Fit } };


	MainScreen(ResourceManager& resourceManager);
};