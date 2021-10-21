#include "ExitConfirmationMenuLayer.h"
#include "App.h"

void ExitConfirmationMenuLayer::Setup()
{
	SetActive(false);	
	
	backgroundPanel.transform.parentAlign = UI::Align::TopLeft;
	backgroundPanel.transform.align = UI::Align::TopLeft;
	backgroundPanel.transform.pos = { -5, 5 };
	backgroundPanel.transform.depth = -0.1f;
	backgroundPanel.color = ColorRGBA(0, 0, 0, 64);
	Add(backgroundPanel);

	menuPanel.transform.parentAlign = UI::Align::Center;
	menuPanel.transform.align = UI::Align::Center;
	menuPanel.transform.size = Vec2i(300, 100);	
	menuPanel.transform.depth = -0.05f;
	menuPanel.color = ColorRGBA(86, 86, 86, 255);
	menuPanel.outlineColor = panelOutlineColor;
	menuPanel.thickness = 5;
	menuPanel.radius = 10;
	Add(menuPanel);

	confirmationText.SetParent(menuPanel);
	confirmationText.transform.parentAlign = UI::Align::Top;
	confirmationText.transform.align = UI::Align::Top;
	confirmationText.transform.pos = { 0, -15 };
	confirmationText.transform.depth = 0.1f;
	confirmationText.SetColor(Colors::White);
	confirmationText.SetFont(&app->font);
	confirmationText.SetScale(18);
	confirmationText.SetString("Are you sure you want to exit?");
	Add(confirmationText);	
	
	exitButton.button.StateChangedEvent = [&]()
	{
		exitButton.panel.color = GetButtonColor(exitButton.button);
	};
	exitButton.button.PressedEvent = [&]()
	{
		Application::Stop();
	};
	exitButton.button.parent = &exitButton.panel;	
	Add(exitButton.button);
	exitButton.panel.SetParent(menuPanel);
	exitButton.panel.transform.parentAlign = UI::Align::Bottom;
	exitButton.panel.transform.align = UI::Align::BottomRight;
	exitButton.panel.transform.pos = { -20, 20 };	
	exitButton.panel.transform.size = { 50, 25 };
	exitButton.panel.color = buttonUpColor;
	exitButton.panel.outlineColor = panelOutlineColor;
	Add(exitButton.panel);
	exitButton.text.SetParent(exitButton.panel);
	exitButton.text.transform.parentAlign = UI::Align::Center;
	exitButton.text.transform.align = UI::Align::Center;
	exitButton.text.transform.depth = 0.1f;
	exitButton.text.SetColor(Colors::White);
	exitButton.text.SetFont(&app->font);
	exitButton.text.SetScale(16);
	exitButton.text.SetString("Exit");
	Add(exitButton.text);

	backButton.button.StateChangedEvent = [&]()
	{
		backButton.panel.color = GetButtonColor(backButton.button);
	};
	backButton.button.PressedEvent = [&]()
	{
		app->mainLayer.SetInteractable(true);		
		SetActive(false);
	};
	backButton.button.parent = &backButton.panel;
	Add(backButton.button);
	backButton.panel.SetParent(menuPanel);
	backButton.panel.transform.parentAlign = UI::Align::Bottom;
	backButton.panel.transform.align = UI::Align::BottomLeft;
	backButton.panel.transform.pos = { 20, 20 };
	backButton.panel.transform.size = { 50, 25 };
	backButton.panel.color = buttonUpColor;
	backButton.panel.outlineColor = panelOutlineColor;
	Add(backButton.panel);
	backButton.text.SetParent(backButton.panel);
	backButton.text.transform.parentAlign = UI::Align::Center;
	backButton.text.transform.align = UI::Align::Center;
	backButton.text.transform.depth = 0.1f;
	backButton.text.SetColor(Colors::White);
	backButton.text.SetFont(&app->font);
	backButton.text.SetScale(16);
	backButton.text.SetString("Back");
	Add(backButton.text);
}

void ExitConfirmationMenuLayer::Update()
{
}

void ExitConfirmationMenuLayer::NewViewport(Vec2i size)
{
	backgroundPanel.transform.size = size + Vec2i(10, 10);
}
