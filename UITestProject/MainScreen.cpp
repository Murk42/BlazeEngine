#include "pch.h"
#include "MainScreen.h"

extern Array<Font> fonts;
MainScreen::MainScreen()	
	: Screen(nullptr), replaceTextMenu(editableText)
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
	explanationText.textRenderUnit.SetFont(fonts[0]);
	explanationText.textRenderUnit.SetFontHeight(20);
	explanationText.textRenderUnit.SetText(
		"This is a project to test some UI elements, the key is to use as many features as possible. "
		"For example here were writing as long of a text as possible and trying a text with multiple lines. "
		"This paragraph is all in one line so they will be wrapped, non wrapped lines should be more spaced apart. \n\n"
		"Here is a new paragraph.\n"
		"And here is a new line\n"
		"And a another line"
		);
	explanationText.textRenderUnit.SetLayoutOptions({
		.lineHorizontalAlign = UI::TextLineHorizontalAlign::Left,
		.lineVerticalAlign = UI::TextLineVerticalAlign::Top,
		.horizontallyUnderfittedOption = UI::TextHorizontallyUnderfittedOptions::WordWrapSpread,
		.wrappedLineAdvance = 0.9f
		});
	explanationText.textRenderUnit.SetCullingNode(&explanationPanel);
				
	texture.Load("assets/images/Mountains.jpg", Graphics::OpenGLWrapper::Texture2DSettings{
		.min = Graphics::OpenGLWrapper::TextureSampling::Linear,
		.mip = Graphics::OpenGLWrapper::TextureSampling::Linear,
		.textureLevelCount = 1
		});
	image.SetTexture(&texture);
	image.SetSourceRect(Vec2f(0, 0), Vec2f(1, 1));		
	image.SetImageLayout(UI::ImageLayout::Stretch);
	image.SetParent(&explanationPanel);
	image.SetTransform({
		.pos = Vec2f(5, 0),
		.parentPivot = Vec2f(1.0f, 1.0f),
		.pivot = Vec2f(0.0f, 1.0f),
		.size = Vec2f(500, 333),		
		});		
	
	editableText.SetParent(&explanationPanel);
	editableText.SetTransform({
		.pos = Vec2f(5, -10),
		.parentPivot = Vec2f(0.0f, 0.0f),
		.pivot = Vec2f(0.0f, 1.0f),
		.size = Vec2f(400, 300),
		});
	editableText.SetEmptyText("Enter text");
	editableText.textRenderUnit.SetFont(fonts[1]);
	editableText.textRenderUnit.SetFontHeight(32);
	editableText.textRenderUnit.SetLayoutOptions({
		.lineHorizontalAlign = UI::TextLineHorizontalAlign::Left,
		.lineVerticalAlign = UI::TextLineVerticalAlign::Top,
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
	clearTextButton.textRenderUnit.SetFont(fonts[0]);
	clearTextButton.textRenderUnit.SetFontHeight(20);	
	clearTextButton.textRenderUnit.SetText("Clear");
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
	replaceTextButton.textRenderUnit.SetFont(fonts[0]);
	replaceTextButton.textRenderUnit.SetFontHeight(20);
	replaceTextButton.textRenderUnit.SetText("Replace");
	replaceTextButton.pressedEventCallback = [&](auto event) {
		replaceTextMenu.Enable();
		};	
	
	replaceTextMenu.SetParent(this);
	replaceTextMenu.Disable();

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
	targetText.textRenderUnit.SetFontHeight(20);
	targetText.textRenderUnit.SetFont(fonts[0]);
	targetText.textRenderUnit.SetLayoutOptions({
		.lineHorizontalAlign = UI::TextLineHorizontalAlign::Left,
		.lineVerticalAlign = UI::TextLineVerticalAlign::Center,
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
	replaceText.textRenderUnit.SetFontHeight(20);
	replaceText.textRenderUnit.SetFont(fonts[0]);
	replaceText.textRenderUnit.SetLayoutOptions({		
		.lineHorizontalAlign = UI::TextLineHorizontalAlign::Left,
		.lineVerticalAlign = UI::TextLineVerticalAlign::Center,		
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
	replaceButton.textRenderUnit.SetText("Replace");
	replaceButton.textRenderUnit.SetFontHeight(12);
	replaceButton.textRenderUnit.SetFont(fonts[0]);
	replaceButton.textRenderUnit.SetLayoutOptions({
		.lineHorizontalAlign = UI::TextLineHorizontalAlign::Center,
		.lineVerticalAlign = UI::TextLineVerticalAlign::Center,
		});
	replaceButton.pressedEventCallback = [&](auto event) {		

		Disable();
		};

	cancelButton.SetParent(&mainPanel);
	cancelButton.SetTransform({
		.pos = Vec2f(310, 10),
		.parentPivot = Vec2f(0),
		.pivot = Vec2f(0),
		.size = Vec2f(80, 35),
		});
	cancelButton.textRenderUnit.SetText("Cancel");
	cancelButton.textRenderUnit.SetFontHeight(12);
	cancelButton.textRenderUnit.SetFont(fonts[0]);
	cancelButton.textRenderUnit.SetLayoutOptions({
		.lineHorizontalAlign = UI::TextLineHorizontalAlign::Center,
		.lineVerticalAlign = UI::TextLineVerticalAlign::Center,
		});
	cancelButton.pressedEventCallback = [&](auto event) {
		Disable();
		};	
}

ReplaceTextMenu::~ReplaceTextMenu()
{
}
