#include "pch.h"
#include "MainScreen.h"

MainScreen::MainScreen(ResourceManager& resourceManager, UI::FontManager& fontManager)
	: resourceManager(resourceManager), fontManager(fontManager)
{
	explanationText.SetWrapWidth(explanationText.GetTransform().size.x);
	
	//numberInput.SetValueEnteredCallback([&](float value)
	//	{
	//		Console::Write("Float entered \"{}\"\n", value);
	//		
	//		textInput.SetPressableFlag(value != 0.0f);
	//	});
}

