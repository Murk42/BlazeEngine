#pragma once
#include "Resources/ResourceManager.h"

class MainScreen : public UI::Screen
{
	UI::Text explanationText;
	UI::Panel panel;
	UI::Text title;
	UI::Text label;
	UI::InputBox inputBox;
	UI::TextButton button;
	LambdaEventHandler<UI::TextButton::PressedEvent> buttonPressedEventHandler;
public:
	void Update();

	MainScreen(Resource::ResourceManager& manager);
	~MainScreen();
};
