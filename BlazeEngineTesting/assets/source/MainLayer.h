#pragma once
#include "BlazeEngine/BlazeEngine.h"
using namespace Blaze;

class MainLayer : public UI::Layer
{  
public:
	struct {
		UI::Text text;
	} fpsText;

	struct {
		UI::ButtonState button;
		UI::Text text;
		UI::Panel panel;
	} exitButton;	

	struct {
		UI::TextFieldState textField;
		UI::Text text;
		UI::Panel panel;
	} textField;	
private:
	void Setup() override;
	void Update() override;
};