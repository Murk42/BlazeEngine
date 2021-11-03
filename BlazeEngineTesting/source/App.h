#pragma once
#include "BlazeEngine/BlazeEngine.h"
using namespace Blaze;

#include "UI.h"

class App : public BlazeEngine<App> 
{
public:			
	Window window;
	UI::Button button;

	void Startup() override;

	void Frame() override;

	void Cleanup() override;
};

extern App* app;