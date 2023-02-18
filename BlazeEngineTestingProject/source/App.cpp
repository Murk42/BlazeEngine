#include "App.h"

App* app = nullptr;

	 

void Start()
{
	Window window;
	window.SetWindowed(false, false);

	Graphics::Core::SetTarget(window);
	Graphics::Core::SetViewport(Vec2i(), window.GetSize());

	window.ShowWindow(true);

	while (!Input::GetKeyState(Key::Escape))
	{
		Input::Update();

		Graphics::Core::ClearTarget();

		Graphics::Core::SwapWindowBuffers();
	}
}