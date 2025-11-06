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
	ResourceManager resourceManager;
	ResourceRef<UI::FontFace> fontFace = resourceManager.LoadResource<UI::FontFace>("font face", UI::FontFace::LoadDefault());;
	ResourceRef<UI::FontAtlas> fontAtlas0 = resourceManager.LoadResource<UI::FontAtlas>("", *fontFace, 32.0f, fontFace->GetAllGlyphsInidices(), UI::FontGlyphRenderers::AntialiasedFontGlyphRenderer(), resourceManager);
	ResourceRef<UI::FontAtlas> fontAtlas1 = resourceManager.LoadResource<UI::FontAtlas>("", *fontFace, 20.0f, fontFace->GetAllGlyphsInidices(), UI::FontGlyphRenderers::AntialiasedFontGlyphRenderer(), resourceManager);

	ResourceRef< Graphics::OpenGL::Texture2D> texture = resourceManager.LoadResource<Graphics::OpenGL::Texture2D>("", "assets/images/Mountains.jpg");

	UI::Layout::HorizontalDivisonNode divisionNode{ *this, 50 };
	UI::Layout::ParentSizeBinding divisionNodeSizeBinding{ &divisionNode };

		UI::Nodes::EmptyNode headerNode{ divisionNode, {} };

			UI::Nodes::Label text{ headerNode, {.pos = { 5, 0 }, .parentPivot = {0, .5}, .pivot = {0, .5}}, "Hello, BlazeEngineUI!", *fontAtlas0 };
			UI::Nodes::PanelButton button{ text, {.pos = { 5, 0 }, .parentPivot = { 1, 0.5 }, .pivot = { 0, 0.5 }, .size = { 200, 40 } }, {} };
			UI::Nodes::Label buttonText{ button, { }, "AV lii Ti", *fontAtlas0 };

	UI::Nodes::Panel contentNode{ divisionNode, {}, { .borderWidth = 0, .cornerRadius = 0 } };

		UI::Layout::VerticalArrayLayoutNode slidersLayout{ contentNode, 5.0f, HorizontalAlign::Left };
		UI::Layout::ParentSizeBinding slidersLayoutSizeBinding{ &slidersLayout };

			LabeledSlider sliders1 { slidersLayout, "Slider label A", *fontAtlas1 };
			LabeledSlider sliders2 { slidersLayout, "Slider label B", *fontAtlas1 };
			LabeledSlider sliders3 { slidersLayout, "Slider label C", *fontAtlas1 };
			LabeledSlider sliders4 { slidersLayout, "Slider label D", *fontAtlas1 };

		UI::Nodes::Panel panel{ contentNode, {.pos = { -5, 5 }, .parentPivot = { 1, 0}, .pivot = { 1, 0}, .size = {210, 210}} , {} };
		UI::Nodes::Image image{ panel, {.size = { 200, 200} }, texture, {0,0}, {1,1}, {.layout = UI::Nodes::ImageLayout::Fit } };

	MainScreen(Window* window, ResourceManager& resourceManager);
};