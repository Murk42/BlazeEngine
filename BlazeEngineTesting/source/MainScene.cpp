#pragma once
#include "BlazeEngine/BlazeEngine.h"
using namespace Blaze;

class MainScene : StartupScene<MainScene>
{ 
public: 
	Window win;

	void Startup() override 
	{
		Input::SetEventFunction([&](InputEvent::ViewportChanged event)
			{
				Console::WriteLine("Viewport changed to " + String::Convert(event.size));
			});
		Input::SetEventFunction([&](InputEvent::WindowResized event)
			{
				Renderer::SetViewport({ 0, 0 }, event.size);
			});

		win.SetWindowed(true, false);
		win.ShowWindow(true);

		Renderer::SetTarget(win);		
	} 

	void Update() override
	{

	}

	void Cleanup() override
	{

	}
};