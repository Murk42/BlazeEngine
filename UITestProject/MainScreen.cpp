#include "pch.h"
#include "MainScreen.h"

extern Array<Font> fonts;


MainScreen::MainScreen()	 
{
	titleText.SetParent(this);
	titleText.SetTransform({
		.pos = Vec2f(0, -5),
		.parentPivot = Vec2f(0.5f, 1.0f),
		.pivot = Vec2f(0.5f, 1.0f),
		});
	titleText.SetFont(fonts[0]);
	titleText.SetFontHeight(32);
	titleText.SetText("UI Test Project");
	titleText.SetTextCharactersColor(Array<ColorRGBAf>({
		0xff0000ff, 0xffa500ff, 0x0, 
		0xffff00ff, 0x008000ff, 0x0000ffff, 0x4b0082ff, 0x0,
		0xee82eeff, 0xff0000ff, 0xffa500ff, 0xffff00ff, 0x008000ff, 0x0000ffff, 0xee82eeff
		}));
	titleText.SetLayoutOptions({		
		.horizontallyUnderfittedOption = UI::TextHorizontallyUnderfittedOptions::ResizeToFit,
		.verticallyUnderfittedOption = UI::TextVerticallyUnderfittedOptions::ResizeToFit,
		});

	explanationPanel.SetParent(this);
	explanationPanel.SetTransform({
		.pos = Vec2f(5, -40),
		.parentPivot = Vec2f(0.0f, 1.0f),
		.pivot = Vec2f(0.0f, 1.0f),
		.size = Vec2f(500, 400),
		});
	explanationPanel.SetFillColor(0x002000ff);
	explanationPanel.SetBorderWidth(0);
	explanationPanel.SetCornerRadius(5);	
	transformUpdatedEventDispatcher.AddHandler(screenSizeChangedEventHandler);

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
		"This is a project to test some UI elements, the key is to use as many features as possible."
		"For example here were writing as long of a text as possible and trying a text with multiple lines."
		"This paragraph is all in one line so they will be wrapped, non wrapped lines should be more spaced apart.\n\n"
		"Here is a new paragraph.\n"
		"And here is a new line\n"
		"And a another line"
		);
	explanationText.textRenderUnit.SetLayoutOptions({
		.lineHorizontalAlign = UI::TextLineHorizontalAlign::Left,
		.lineVerticalAlign = UI::TextLineVerticalAlign::Top,
		.horizontallyUnderfittedOption = UI::TextHorizontallyUnderfittedOptions::WordWrap,
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
		.horizontallyUnderfittedOption = UI::TextHorizontallyUnderfittedOptions::WordWrap,
		});
	editableText.SetMultilineInput(true);
	//editableText.textEnteredEventDispatcher.AddHandler(
	//	BoundLambdaEventHandler<UI::EditableText::TextEnteredEvent>()
	//);
}