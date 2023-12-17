#pragma once
#include "UI/UINode.h"
#include "Resources/ResourceManager.h"

class TextBox : public UI2::Node, EventHandler<UI2::NodeTransformUpdatedEvent>
{
public:
	TextBox(UI2::Node* parent, const UI2::NodeTransform& transform)
		: Node(parent, transform), panel(this, { }), text(&panel, { })
	{
		transformUpdatedEventDipatcher.AddHandler(*this);		
		text.SetTextLayouter<SingleLineTextLayouter>();
		text.SetTextHeight(20.0f);

		panel.fillColor = ColorRGBAf(0, 28.0f / 255, 17.0f / 255);
		panel.borderColor = ColorRGBAf(5.0f / 255, 8.0 / 255, 13.0f / 255);
		panel.borderWidth = 2.0f;
		panel.cornerRadius = 10.0f;
	}

	void SetText(StringViewUTF8 text)
	{
		this->text.SetText(text);
	}
	void SetFont(Font& font)
	{
		this->text.SetFont(font);
	}
	void SetTextColors(Array<ColorRGBAf> colors)
	{
		text.SetTextColors(std::move(colors));
	}		

	String GetTypeName() const override { return "TextBox"; }
private:
	UI2::Panel panel;
	UI2::Text text;

	void OnEvent(UI2::NodeTransformUpdatedEvent event) override
	{
		auto transform = GetTransform();
		

		text.SetTransform({
			.pos = Vec2f(0, 0),
			.parentPivot = Vec2f(0.5f, 0.5f),
			.pivot = Vec2f(0.5f, 0.5f),
			.size = { 0.0f, 20.0f },
			.scale = Vec2f(1.0f, 1.0f),
			.rotation = 0.0f,
			});				
		panel.SetTransform({
			.pos = Vec2f(0, 0),
			.parentPivot = Vec2f(0.0f, 0.0f),
			.pivot = Vec2f(0.0f, 0.0f),
			.size = transform.size,
			.scale = Vec2f(1.0f, 1.0f),
			.rotation = 0.0f
			});
	}
};

class MainScreen : public UI2::Screen 
{
public:
	UI2::Panel panel;
	UI2::Image image;
	UI2::Text text;
	TextBox textBox;

	MainScreen(Resource::ResourceManager& manager)
		: panel(this, { }), image(&panel, { }), text(&image, { }), textBox(this, { })
	{
		panel.fillColor = ColorRGBAf(0, 28.0f / 255, 17.0f / 255);
		panel.borderColor = ColorRGBAf(5.0f / 255, 8.0 / 255, 13.0f / 255);
		panel.borderWidth = 2.0f;
		panel.cornerRadius = 10.0f;
		panel.SetTransform({
			.pos = { 30, -30 },
			.parentPivot = { 0.0f, 1.0f },
			.pivot = { 0.0f, 1.0f},
			.size = { 200, 400},
			.scale = {1.0f, 1.0f},
			.rotation = 0.0f,
			});
		image.SetTransform({
			.pos = { 10.0f, -10.0f },
			.parentPivot = { 0.0f, 1.0f },
			.pivot = { 0.0f, 1.0f },
			.size = { 180, 180},
			.scale = { 1.0f, 1.0f },
			.rotation = 0,
			});

		image.texture = &manager.GetTexture2DOpenGL("image");
		image.uv1 = Vec2f(0, 0);
		image.uv2 = Vec2f(1, 1);
		image.alpha = 1.0f;
		image.blend = 0.0f;
		image.color = 0xffffffff;

		SingleLineTextLayouter layouter;
		text.SetTextLayouter<SingleLineTextLayouter>();
		text.SetText("Ovo je neki tekst");
		text.SetTextColors({ 0xf5f5f5ff });
		text.SetFont(manager.GetFont("default"));
		text.SetTransform({
			.pos = { 0.0f, 0.0f },
			.parentPivot = { 0.0f, 0.0f },
			.pivot = { 0.0f, 1.0f},
			.size = { 0, 20.0f },
			.scale = { 1.0f, 1.0f },
			.rotation = 0
			});
		text.SetTextHeight(20);
		
		Font font;		
		textBox.SetText("Drugi tekst");
		textBox.SetTextColors({ 0xf5f5f5ff });
		textBox.SetFont(manager.GetFont("default"));
		textBox.SetTransform({
			.pos = { },
			.parentPivot = { 0.5f, 0.5f},
			.pivot = { 0.5f, 0.5f},
			.size = { 200, 30 },
			.scale = { 1.0f, 1.0f},
			.rotation = 0
			});
	}
	~MainScreen()
	{

	}
};