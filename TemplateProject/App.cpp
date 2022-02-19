#include "BlazeEngine/BlazeEngine.h"
using namespace Blaze;

CLIENT_API void Setup()
{
	Window window;
	bool running = true;

	window.ShowWindow(true);

	Console::WriteLine("Press ESC to exit");

	while (running)
	{
		Input::Update();

		if (Input::GetKeyState(Key::Escape).pressed)
			running = false;
	
	}	
}