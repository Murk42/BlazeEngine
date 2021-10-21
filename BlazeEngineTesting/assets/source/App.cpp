#include "App.h"


ColorRGBA GetButtonColor(UI::ButtonState& button)
{
	if (button.down)
		return buttonDownColor; 
	if (button.hovered)
		return buttonHoveredColor;
	return buttonUpColor;
}
ColorRGBA GetTextFieldColor(UI::TextFieldState& button)
{
	if (button.down)
		return buttonDownColor;
	if (button.hovered)
		return buttonHoveredColor;
	return buttonUpColor;
}

App* app = nullptr;

App::App()
{
	app = this;
}

void App::SetViewport(Vec2i size) 
{
	Renderer::SetViewport(Vec2i(), size); 	
}

void App::SetupEvents()
{
	Input::SetEventFunction([&](InputEvent::WindowClosed event)
		{
			Application::Stop();
		});
	Input::SetEventFunction([&](InputEvent::WindowSizeChanged event)
		{
			SetViewport(event.size);
		});			
	Input::SetEventFunction([&](InputEvent::KeyPressed event)
		{
			if (event.key == Key::F11)
				window.SetFullscreen(!window.IsFullscreen());
		});
}
void App::SetupWindow()
{
	window.SetWindowed(true, false);
	window.SetSize(Vec2i(16 * 80, 9 * 80));	
	window.SetMinimumSize(window.GetSize());
	window.ShowWindow(true);
}
void App::SetupRenderer()
{
	Renderer::SetTarget(window);
	Renderer::EnableFaceCulling(false);
	Renderer::EnableDepthBuffer(false); 
	Renderer::EnableDepthTest(false);
	Renderer::SetClearColor(ColorRGBA(86, 86, 86, 255));	
	SetViewport(window.GetSize());
}
void App::SetupAssets()
{
	font.Load("assets/Roboto.ttf", FontType::Antialiased, 30);
}

void App::Startup()
{	
	SetupEvents(); 
	SetupWindow();
	SetupRenderer();		
	SetupAssets();
	
}

void App::Frame()
{	
	Renderer::ClearTarget();
	
	UI::RenderUI();

	Renderer::UpdateTarget();
} 

void App::Cleanup()
{
}