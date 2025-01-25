#pragma once

class ReplaceTextMenu : 
	public UI::Node
{
public:
	UI::Nodes::Panel mainPanel;
	UI::Nodes::Panel targetTextPanel;
	UI::Nodes::EditableText targetText;
	UI::Nodes::Panel replaceTextPanel;
	UI::Nodes::EditableText replaceText;
	UI::Nodes::TextButton replaceButton;
	UI::Nodes::TextButton cancelButton;

	LambdaEventHandler<UI::Nodes::TextButton::PressedEvent> replaceButtonPressed;
	LambdaEventHandler<UI::Nodes::TextButton::PressedEvent> cancelButtonPressed;

	ReplaceTextMenu();
	~ReplaceTextMenu();
};

class MainScreen : public UI::Screen
{	
public:
	UI::Nodes::Text titleText;
	UI::Nodes::Panel explanationPanel;
	UI::Nodes::SelectableText explanationText;
	UI::Nodes::Image image;
	UI::Nodes::EditableText editableText;			
	UI::Nodes::TextButton clearTextButton;
	UI::Nodes::TextButton replaceTextButton;

	Graphics::OpenGLWrapper::Texture2D texture;	
	
	LambdaEventHandler<UI::Nodes::TextButton::PressedEvent> clearTextButtonPressedEventHandler;
	LambdaEventHandler<UI::Nodes::TextButton::PressedEvent> replaceTextButtonPressedEventHandler;

	ReplaceTextMenu replaceTextMenu;

	MainScreen();
};

/*
TextButton
Button
IconButton
*/