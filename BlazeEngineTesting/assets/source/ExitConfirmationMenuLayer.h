#pragma once
#include "BlazeEngine/BlazeEngine.h"
using namespace Blaze;

class ExitConfirmationMenuLayer : public UI::Layer
{
public:
	UI::Panel backgroundPanel;
	UI::Panel menuPanel;
	UI::Text confirmationText;

	struct {
		UI::Panel panel;
		UI::Text text;
		UI::ButtonState button;
	} exitButton;
	struct {
		UI::Panel panel;
		UI::Text text;
		UI::ButtonState button;
	} backButton;

private:
	void Setup() override;
	void Update() override;
	void NewViewport(Vec2i size) override;
};