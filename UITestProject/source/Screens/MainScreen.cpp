#include "pch.h"
#include "MainScreen.h"

MainScreen::MainScreen(ResourceManager& resourceManager, UI::FontManager& fontManager)
	: resourceManager(resourceManager), fontManager(fontManager)
{
	//slider.slider.SetValueChangedCallback([&](float value) {
	//	auto textStyle = slider.label.GetTextStyle();		
	//	textStyle.fontHeight = 20 * (1 + value * 2);
	//	slider.label.SetTextStyle(textStyle);
	//	});
	//
	//slider.label.SetWrapWidth(200);

	longText.SetWrapWidth(200);
	//textInput.SetMultiline(true);  
	//textInput.SetWrapWidth(150);
}

