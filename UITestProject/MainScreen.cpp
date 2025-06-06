#include "pch.h"
#include "MainScreen.h"
#include "Internet.h"

Array<Font> fonts;
Map<String, UI::FontStyle> fontStyles;

MainScreen::MainScreen(ResourceManager& resourceManager)
	: Screen(nullptr), resourceManager(resourceManager), replaceTextMenu(editableText)
{	
	explanationPanel.SetParent(this);
	explanationPanel.SetTransform({
		.pos = Vec2f(5, -5),
		.parentPivot = Vec2f(0.0f, 1.0f),
		.pivot = Vec2f(0.0f, 1.0f),
		.size = Vec2f(500, 400),
		});
	explanationPanel.SetFillColor(0x002000ff);
	explanationPanel.SetBorderWidth(0);
	explanationPanel.SetCornerRadius(5);

	explanationText.SetParent(&explanationPanel);
	explanationText.SetTransform({
		.pos = Vec2f(0, 0),
		.parentPivot = Vec2f(0.5f, 0.5f),
		.pivot = Vec2f(0.5f, 0.5f),
		.size = Vec2f(490, 390)
		});				
	explanationText.textRenderUnit.SetFontStyle(fontStyles.Find("default")->value);
	explanationText.SetText(
		"This is a project to test some UI elements, the key is to use as many features as possible. "
		"For example here were writing as long of a text as possible and trying a text with multiple lines. "
		"This paragraph is all in one line so they will be wrapped, non wrapped lines should be more spaced apart. \n\n"
		"Here is a new paragraph.\n"
		"And here is a new line\n"
		"And a another line"
		);
	explanationText.textRenderUnit.SetLayoutOptions({
		.wrappedLineAdvance = 0.9f,
		.lineHorizontalAlign = HorizontalAlign::Left,
		.lineVerticalAlign = VerticalAlign::Top,
		.horizontallyUnderfittedOption = UI::TextHorizontallyUnderfittedOptions::WordWrapSpread
		});
	explanationText.textRenderUnit.SetCullingNode(&explanationPanel);
					
	image.SetTexture(nullptr);
	image.SetSourceRect(Vec2f(0, 0), Vec2f(1, 1));		
	image.SetImageLayout(UI::ImageLayout::Fit);
	image.SetParent(&explanationPanel);
	image.SetTransform({
		.pos = Vec2f(5, 0),
		.parentPivot = Vec2f(1.0f, 1.0f),
		.pivot = Vec2f(0.0f, 1.0f),
		.size = Vec2f(500, 333),		
		});		

	this->LoadAPOD();
	
	reloadImageTextButton.SetParent(&image);
	reloadImageTextButton.SetTransform({
		.pos = Vec2f(5, 0),
		.parentPivot = Vec2f(1.0f, 1.0f),
		.pivot = Vec2f(0.0f, 1.0f),
		.size = Vec2f(200, 50)
		});	
	reloadImageTextButton.textRenderUnit.SetFontStyle(fontStyles.Find("default")->value);
	reloadImageTextButton.SetText("Reload");
	reloadImageTextButton.pressedEventCallback = [&](auto event) { this->LoadAPOD(); };
	
	editableText.SetParent(&explanationPanel);
	editableText.SetTransform({
		.pos = Vec2f(5, -10),
		.parentPivot = Vec2f(0.0f, 0.0f),
		.pivot = Vec2f(0.0f, 1.0f),
		.size = Vec2f(400, 300),
		});
	editableText.SetEmptyText("Enter text");
	editableText.textRenderUnit.SetFontStyle(fontStyles.Find("large")->value);
	editableText.textRenderUnit.SetLayoutOptions({
		.lineHorizontalAlign = HorizontalAlign::Left,
		.lineVerticalAlign = VerticalAlign::Top,
		.horizontallyUnderfittedOption = UI::TextHorizontallyUnderfittedOptions::WordWrapSpread,
		});
	editableText.SetMultilineInput(true);
	
	clearTextButton.SetParent(&editableText);
	clearTextButton.SetTransform({
		.pos = Vec2f(5, 0),
		.parentPivot = Vec2f(1.0f, 1.0f),
		.pivot = Vec2f(0.0f, 1.0f),
		.size = Vec2f(200, 50)
		});	
	clearTextButton.textRenderUnit.SetFontStyle(fontStyles.Find("default")->value);
	clearTextButton.SetText("Clear");
	clearTextButton.pressedEventCallback = [&](auto event) {
		editableText.SetText("");
		};

	replaceTextButton.SetParent(&editableText);
	replaceTextButton.SetTransform({
		.pos = Vec2f(5, -55),
		.parentPivot = Vec2f(1.0f, 1.0f),
		.pivot = Vec2f(0.0f, 1.0f),
		.size = Vec2f(200, 50)
		});
	replaceTextButton.textRenderUnit.SetFontStyle(fontStyles.Find("default")->value);
	replaceTextButton.SetText("Replace");
	replaceTextButton.pressedEventCallback = [&](auto event) {
		replaceTextMenu.Enable();
		};	
	
	replaceTextMenu.SetParent(this);
	replaceTextMenu.Disable();

}

void MainScreen::LoadAPOD()
{

	static std::mutex mutex;
	resourceManager.LoadResourceAsync<Bitmap>("APOD", [&](Resource<Bitmap>& resource) {
		Bitmap bitmap = ::LoadAPOD();

		std::unique_lock lk{ mutex };
		resource = std::move(bitmap);
		},
		[&](Resource<Bitmap>& resource) {						
			image.SetTexture(nullptr);

			APODTexture = Graphics::OpenGLWrapper::Texture2D();
			APODTexture.Create(*resource.Get(), Blaze::Graphics::OpenGLWrapper::Texture2DSettings{
				.min = Blaze::Graphics::OpenGLWrapper::TextureSampling::Linear,
				.mag = Blaze::Graphics::OpenGLWrapper::TextureSampling::Linear,
				.mip = Blaze::Graphics::OpenGLWrapper::TextureSampling::Linear,
				.textureLevelCount = 1
				});

			image.SetTexture(&APODTexture);
		});
}

ReplaceTextMenu::ReplaceTextMenu(UI::Nodes::EditableText& target)
	: target(target)
{
	backgroundPanel.SetParent(this);
	backgroundPanel.SetTransform({
		.size = Vec2f(10000)
		});
	backgroundPanel.SetFillColor(0x40404088);

	mainPanel.SetParent(this);
	mainPanel.SetTransform({
		.pos = Vec2f(0, 0),
		.parentPivot = Vec2f(0.5f, 0.5f),
		.pivot = Vec2f(0.5f, 0.5f),
		.size = Vec2f(400, 100),
		});
	mainPanel.SetFillColor(0x101010ff);
	mainPanel.SetBorderWidth(0);
	mainPanel.SetCornerRadius(10);

	targetTextPanel.SetParent(&mainPanel);
	targetTextPanel.SetTransform({
		.pos = Vec2f(10, 55),
		.parentPivot = Vec2f(0),
		.pivot = Vec2f(0),
		.size = Vec2f(290, 35),
		});
	targetTextPanel.SetFillColor(0x202020ff);
	targetTextPanel.SetBorderWidth(0);
	targetText.SetParent(&targetTextPanel);
	targetText.SetTransform({
		.pos = Vec2f(5, -5),
		.size = Vec2f(280, 25),
		});
	targetText.textRenderUnit.SetFontStyle(fontStyles.Find("default")->value);
	targetText.textRenderUnit.SetLayoutOptions({
		.lineHorizontalAlign = HorizontalAlign::Left,
		.lineVerticalAlign = VerticalAlign::Center,
		});
	targetText.SetMultilineInput(false);
	targetText.textRenderUnit.SetCullingNode(&targetText);
	targetText.SetEmptyText("Target text");

	replaceTextPanel.SetParent(&mainPanel);
	replaceTextPanel.SetTransform({
		.pos = Vec2f(10, 10),
		.parentPivot = Vec2f(0),
		.pivot = Vec2f(0),
		.size = Vec2f(290, 35),
		});
	replaceTextPanel.SetFillColor(0x202020ff);
	replaceTextPanel.SetBorderWidth(0);
	replaceText.SetParent(&replaceTextPanel);
	replaceText.SetTransform({
		.pos = Vec2f(5, -5),
		.size = Vec2f(280, 25),
		});
	replaceText.textRenderUnit.SetFontStyle(fontStyles.Find("default")->value);
	replaceText.textRenderUnit.SetLayoutOptions({		
		.lineHorizontalAlign = HorizontalAlign::Left,
		.lineVerticalAlign = VerticalAlign::Center,		
		});
	replaceText.SetMultilineInput(false);
	replaceText.textRenderUnit.SetCullingNode(&replaceText);
	replaceText.SetEmptyText("Replace text");

	replaceButton.SetParent(&mainPanel);
	replaceButton.SetTransform({
		.pos = Vec2f(310, 55),
		.parentPivot = Vec2f(0),
		.pivot = Vec2f(0),
		.size = Vec2f(80, 35),
		});
	replaceButton.SetText("Replace");
	replaceButton.textRenderUnit.SetFontStyle(fontStyles.Find("small")->value);
	replaceButton.textRenderUnit.SetLayoutOptions({
		.lineHorizontalAlign = HorizontalAlign::Center,
		.lineVerticalAlign = VerticalAlign::Center,
		});
	replaceButton.pressedEventCallback = [&](const UI::ButtonBase::PressedEvent& event) {		
		StringUTF8 text = target.GetText();
	
		auto it = StringParsing::Find(text, targetText.GetText());
		uintMem offset = (byte*)it.Ptr() - text.Buffer();

		if (!it.IsNull())
		{
			text.EraseSubString(offset, targetText.GetText().CharacterCount());
			text.Insert(offset, replaceText.GetText());
			target.SetText(text);
			Disable();
		}
		};

	cancelButton.SetParent(&mainPanel);
	cancelButton.SetTransform({
		.pos = Vec2f(310, 10),
		.parentPivot = Vec2f(0),
		.pivot = Vec2f(0),
		.size = Vec2f(80, 35),
		});
	cancelButton.SetText("Cancel");
	cancelButton.textRenderUnit.SetFontStyle(fontStyles.Find("small")->value);
	cancelButton.textRenderUnit.SetLayoutOptions({
		.lineHorizontalAlign = HorizontalAlign::Center,
		.lineVerticalAlign = VerticalAlign::Center,
		});
	cancelButton.pressedEventCallback = [&](const UI::ButtonBase::PressedEvent& event) {
		Disable();
		};	
}

ReplaceTextMenu::~ReplaceTextMenu()
{
}

Slider::Slider()
	: labelRenderUnit(labelTextContainer, this), valueRenderUnit(valueTextContainer, this), barRenderUnit(this), sliderRenderUnit(this)
{	
}

void Slider::SetBarColor(ColorRGBAf color)
{
}

void Slider::SetSliderColor(ColorRGBAf color)
{
}

Graphics::RenderUnit* Slider::GetRenderUnit(uint index)
{
	return nullptr;
}

void Slider::SetLabel(StringViewUTF8 label)
{
}

void Slider::SetLabelFontStyle(const UI::FontStyle& fontStyle)
{
}

void Slider::SetLabelColor(ColorRGBAf color)
{
}

void Slider::SetValue(float value)
{
}

void Slider::SetValueFontStyle(const UI::FontStyle& fontStyle)
{
}

void Slider::SetValueColor(ColorRGBAf color)
{
}
