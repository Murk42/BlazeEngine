#pragma once

class MainScreen : public UI::Screen
{	
public:
	UI::Text titleText;
	UI::Panel explanationPanel;
	UI::SelectableText explanationText;
	UI::Image image;
	UI::EditableText editableText;

	Graphics::OpenGLWrapper::Texture2D texture;	

	LambdaEventHandler<UI::Node::TransformUpdatedEvent> screenSizeChangedEventHandler;	

	MainScreen();
};