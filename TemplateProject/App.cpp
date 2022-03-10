#include "BlazeEngine/BlazeEngine.h"
using namespace Blaze;

CLIENT_API void Setup()
{
	Window window{ "Blaze engine application", Vec2i(640, 360) };
	window.SetWindowed(false, false);
	window.ShowWindow(true);

	Renderer::SetTarget(window);
	Renderer::SetViewport({}, window.GetSize());

	while (!Input::GetKeyState(Key::Escape).pressed)
	{
		Input::Update();

		Renderer::ClearTarget();

		float time = TimePoint::GetRunTime().GetValue();
		float r = Math::Sin(time + Math::PI / 3 * 0) / 2 + 0.5f;
		float g = Math::Sin(time + Math::PI / 3 * 1) / 2 + 0.5f;
		float b = Math::Sin(time + Math::PI / 3 * 2) / 2 + 0.5f;

		Graphics::Write("Hello :)", Vec2i(20, 125), 140, ColorRGBAf(r, g, b, 1));

		Renderer::SwapWindowBuffers();
	}
}