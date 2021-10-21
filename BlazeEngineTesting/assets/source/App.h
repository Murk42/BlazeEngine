#pragma once
#include "BlazeEngine/BlazeEngine.h"
using namespace Blaze;

#include "MainLayer.h"
#include "ExitConfirmationMenuLayer.h"

constexpr ColorRGBA buttonUpColor = { 86, 86, 86, 255 };
constexpr ColorRGBA buttonDownColor = { 70, 70, 70, 255 };
constexpr ColorRGBA buttonHoveredColor = { 106, 106, 106, 255 };
constexpr ColorRGBA panelOutlineColor = { 50, 50, 50, 255 };

ColorRGBA GetButtonColor(UI::ButtonState&);
ColorRGBA GetTextFieldColor(UI::TextFieldState&);

class App : public BlazeEngine<App>
{
public:	
	Window window;	

	Font font; 

	UI::LayerManager layerManager;
	MainLayer mainLayer;
	ExitConfirmationMenuLayer exitConfirmationMenuLayer;

	App(); 

	void SetViewport(Vec2i size);

	void SetupEvents();
	void SetupWindow();
	void SetupRenderer();

	void Startup() override;

	void Frame() override;

	void Cleanup() override;
};

extern App* app;