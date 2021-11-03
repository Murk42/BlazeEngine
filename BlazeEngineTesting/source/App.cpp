#include "App.h"

App* app;

void App::Startup()
{
	app = this;	
	window.ShowWindow(true);
	
	Renderer::SetTarget(window);
	Renderer::SetViewport(Vec2i(), window.GetSize());			
}	   
 
void App::Frame() 
{	
	if (Input::GetKeyState(Key::Escape).down)
		Application::Stop();	
	
	Renderer::ClearTarget();	

	Renderer::UpdateTarget();
} 

void App::Cleanup()
{
}