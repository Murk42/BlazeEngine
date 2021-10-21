#include "ExitConfirmationMenuLayer.h"
#include "App.h"

void ExitConfirmationMenuLayer::Setup()
{
	SetActive(false);	
	
	Add(backgroundPanel);
	backgroundPanel.transform.parentAlign = UI::Align::TopLeft;
	backgroundPanel.transform.align = UI::Align::TopLeft;
	backgroundPanel.transform.pos = { -5, -5 };
	backgroundPanel.transform.depth = -0.1f;
	backgroundPanel.UpdateTransform();
	backgroundPanel.radius = 0;	
	backgroundPanel.color = ColorRGBA(0, 0, 0, 64);

	Add(menuPanel);
	menuPanel.transform.parentAlign = UI::Align::Center;
	menuPanel.transform.align = UI::Align::Center;
	menuPanel.transform.size = Vec2i(300, 100);	
	menuPanel.transform.depth = -0.05f;
	menuPanel.UpdateTransform();
	menuPanel.color = ColorRGBA(86, 86, 86, 255);
	menuPanel.outlineColor = panelOutlineColor;
	menuPanel.thickness = 5;
	menuPanel.radius = 10;

	Add(confirmationText);	
	confirmationText.SetParent(menuPanel);
	confirmationText.transform.parentAlign = UI::Align::Top;
	confirmationText.transform.align = UI::Align::Top;
	confirmationText.transform.pos = { 0, -15 };
	confirmationText.transform.depth = 0.1f;
	confirmationText.UpdateTransform();
	confirmationText.SetColor(Colors::White);
	confirmationText.SetFont(&app->font);
	confirmationText.SetScale(18);
	confirmationText.SetString("Are you sure you want to exit?");
	
	Add(exitButton.button);
	exitButton.button.StateChangedEvent = [&]()
	{
		exitButton.panel.color = GetButtonColor(exitButton.button);
	};
	exitButton.button.PressedEvent = [&]()
	{
		Application::Stop();
	};
	exitButton.button.parent = &exitButton.panel;	
	Add(exitButton.panel);
	exitButton.panel.SetParent(menuPanel);
	exitButton.panel.transform.parentAlign = UI::Align::Bottom;
	exitButton.panel.transform.align = UI::Align::BottomRight;
	exitButton.panel.transform.pos = { -20, 20 };	
	exitButton.panel.transform.size = { 50, 25 };
	exitButton.panel.UpdateTransform();
	exitButton.panel.color = buttonUpColor;
	exitButton.panel.outlineColor = panelOutlineColor;
	Add(exitButton.text);
	exitButton.text.SetParent(exitButton.panel);
	exitButton.text.transform.parentAlign = UI::Align::Center;
	exitButton.text.transform.align = UI::Align::Center;
	exitButton.text.transform.depth = 0.1f;
	exitButton.text.UpdateTransform();
	exitButton.text.SetColor(Colors::White);
	exitButton.text.SetFont(&app->font);
	exitButton.text.SetScale(16);
	exitButton.text.SetString("Exit");

	Add(backButton.button);
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
	Add(backButton.panel);
	backButton.panel.SetParent(menuPanel);
	backButton.panel.transform.parentAlign = UI::Align::Bottom;
	backButton.panel.transform.align = UI::Align::BottomLeft;
	backButton.panel.transform.pos = { 20, 20 };
	backButton.panel.transform.size = { 50, 25 };
	backButton.panel.UpdateTransform();
	backButton.panel.color = buttonUpColor;
	backButton.panel.outlineColor = panelOutlineColor;
	Add(backButton.text);
	backButton.text.SetParent(backButton.panel);
	backButton.text.transform.parentAlign = UI::Align::Center;
	backButton.text.transform.align = UI::Align::Center;
	backButton.text.transform.depth = 0.1f;
	backButton.text.UpdateTransform();
	backButton.text.SetColor(Colors::White);
	backButton.text.SetFont(&app->font);
	backButton.text.SetScale(16);
	backButton.text.SetString("Back");
}

void ExitConfirmationMenuLayer::Update()
{
}

void ExitConfirmationMenuLayer::NewViewport(Vec2i size)
{
	backgroundPanel.transform.size = size;
	backgroundPanel.UpdateTransform();
}
