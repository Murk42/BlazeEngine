#include "BlazeEngine/BlazeEngine.h"
using namespace Blaze;

class App : public BlazeEngine<App>
{
public:	
	Window window;

	void Startup() override
	{		
		Input::SetEventFunction(WindowClosed);		

		window.SetWindowed(false, false);		
		window.ShowWindow(true);

		Renderer::SetTarget(window);				
	}

	void Frame() override
	{				
	}

	void Cleanup() override
	{

	}	
			
	InputEventFunction::WindowClosed WindowClosed = [&](InputEvent::WindowClosed event) 
	{
		Application::Stop();
	};
};