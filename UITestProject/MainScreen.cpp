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
	titleText.SetLayoutOptions(
		UI::TextHorizontalLayout::ChangeSize,
		UI::TextVerticalLayout::ChangeSize,
		UI::TextLineAlign::Center,
		false
	);	

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
		"This is a project to test some UI elements, the key is to use as many features as possible. "
		"For example here were writing as long of a text as possible and trying the multiline feature."
		"In the input boxes enter any font name to change to that font: \"Default\", \"Malboro\", "
		"\"Vogue\", \"African\""
		);
	explanationText.textRenderUnit.SetLayoutOptions(
		UI::TextHorizontalLayout::Wrap,
		UI::TextVerticalLayout::AlignTop,
		UI::TextLineAlign::Left,
		true
	);		
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
	editableText.textRenderUnit.SetLayoutOptions(UI::TextHorizontalLayout::Wrap, UI::TextVerticalLayout::AlignCenter, UI::TextLineAlign::Left, true);
	editableText.SetMultilineInput(true);
	//editableText.textEnteredEventDispatcher.AddHandler(
	//	BoundLambdaEventHandler<UI::EditableText::TextEnteredEvent>()
	//);
}