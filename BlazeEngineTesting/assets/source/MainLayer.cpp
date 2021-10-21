#include "MainLayer.h"
#include "App.h" 

void MainLayer::Setup()
{	

	Add(fpsText.text);
	fpsText.text.transform.parentAlign = UI::Align::BottomRight;
	fpsText.text.transform.align = UI::Align::BottomRight;
	fpsText.text.transform.pos = Vec2i(-5, 5);
	fpsText.text.UpdateTransform();	 
	fpsText.text.SetFont(&app->font);
	fpsText.text.SetColor(Colors::Green);
	fpsText.text.SetScale(16);	

	Add(exitButton.button);
	exitButton.button.parent = &exitButton.panel;	
	exitButton.button.StateChangedEvent = [&]()
	{
		exitButton.panel.color = GetButtonColor(exitButton.button);
	};
	exitButton.button.PressedEvent = [&]()
	{
		app->exitConfirmationMenuLayer.SetActive(true);

		SetInteractable(false);
	};
		
	Add(exitButton.panel);
	exitButton.panel.transform.parentAlign = UI::Align::TopLeft;
	exitButton.panel.transform.align = UI::Align::TopLeft;
	exitButton.panel.transform.pos = { 5, -5 };
	exitButton.panel.transform.size = { 50, 25 };   
	exitButton.panel.UpdateTransform();
	exitButton.panel.color = buttonUpColor;
	exitButton.panel.outlineColor = panelOutlineColor;
	 
	Add(exitButton.text);
	exitButton.text.transform.parentAlign = UI::Align::Center;
	exitButton.text.transform.align = UI::Align::Center;
	exitButton.text.UpdateTransform();
	exitButton.text.SetParent(exitButton.panel);	
	exitButton.text.SetColor(Colors::White);
	exitButton.text.SetScale(16);
	exitButton.text.SetString("Exit");
	exitButton.text.SetFont(&app->font);	


	Add(textField.textField);
	textField.textField.parent = &textField.panel;	
	textField.textField.StateChangedEvent = [&]()
	{
		textField.panel.color = GetTextFieldColor(textField.textField);
	};
	textField.textField.StringChangedEvent = [&]()
	{
			textField.text.SetString(textField.textField.string);
			if (textField.text.transform.size.x + textField.text.transform.pos.x > textField.panel.transform.size.x - 5)
			{
				textField.text.transform.parentAlign = UI::Align::Right;
				textField.text.transform.align = UI::Align::Right;
				textField.text.transform.pos.x = -5;
			}
			else
			{
				textField.text.transform.parentAlign = UI::Align::Left;
				textField.text.transform.align = UI::Align::Left;
				textField.text.transform.pos.x = 5;
			}
	};
	textField.textField.SelectedEvent = [&]()
	{
		textField.text.SetColor(ColorRGBA(255, 255, 255, 255));
		if (textField.textField.string.Size() == 0)
			textField.text.SetString("");
	};
	textField.textField.DeselectedEvent = [&]()
	{
		textField.text.SetColor(ColorRGBA(200, 200, 200, 255));
		if (textField.textField.string.Size() == 0)
			textField.text.SetString("Enter text");
	};
	
	Add(textField.panel);
	textField.panel.SetParent(exitButton.panel); 
	textField.panel.transform.parentAlign = UI::Align::BottomLeft;
	textField.panel.transform.align = UI::Align::TopLeft;
	textField.panel.transform.pos = { 0, -5 };
	textField.panel.transform.size = { 200, 25 };
	textField.panel.color = buttonUpColor;
	textField.panel.outlineColor = panelOutlineColor;
	textField.panel.TransformUpdate = [&]()
	{
		textField.text.clipRect.pos = textField.panel.transform.absolutePos;
		textField.text.clipRect.size = textField.panel.transform.absoluteSize;		
	}; 
	textField.panel.UpdateTransform();
	
	textField.text.SetParent(textField.panel);
	textField.text.transform.parentAlign = UI::Align::Left;
	textField.text.transform.align = UI::Align::Left;
	textField.text.transform.pos = { 5, 0 };
	textField.text.UpdateTransform();
	textField.text.SetColor(ColorRGBA(200, 200, 200, 255));	
	textField.text.SetScale(16);
	textField.text.SetString("Enter text");
	textField.text.SetFont(&app->font);
	textField.text.clip = true;			
	Add(textField.text);
}

void MainLayer::Update()
{
	fpsText.text.SetString(String::Convert(Application::GetFPS()));
}
