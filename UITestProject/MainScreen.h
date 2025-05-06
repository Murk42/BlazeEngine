#pragma once
#include "RegionSplitter.h"

class ReplaceTextMenu : 
	public UI::Node
{
public:
	UI::Nodes::Panel backgroundPanel;
	UI::Nodes::Panel mainPanel;
	UI::Nodes::Panel targetTextPanel;
	UI::Nodes::EditableText targetText;
	UI::Nodes::Panel replaceTextPanel;
	UI::Nodes::EditableText replaceText;
	UI::Nodes::TextButton replaceButton;
	UI::Nodes::TextButton cancelButton;	

	UI::Nodes::EditableText& target;

	ReplaceTextMenu(UI::Nodes::EditableText& target);
	~ReplaceTextMenu();
};

class MainScreen : public UI::Screen
{	
public:		
	UI::Nodes::Panel explanationPanel;
	UI::Nodes::SelectableText explanationText;
	UI::Nodes::Image image;
	UI::Nodes::EditableText editableText;			
	UI::Nodes::TextButton clearTextButton;
	UI::Nodes::TextButton replaceTextButton;	

	Graphics::OpenGLWrapper::Texture2D texture;		

	ReplaceTextMenu replaceTextMenu;

	MainScreen();	
};