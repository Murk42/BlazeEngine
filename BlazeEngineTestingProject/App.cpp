#include "BlazeEngine/BlazeEngine.h"
using namespace Blaze;

CLIENT_API void Setup()
{
	Window window{ "Blaze engine application", Vec2i(640, 360) };
	window.SetWindowed(false, false);
	window.ShowWindow(true);

	Renderer::SetTarget(window);
	Renderer::SetViewport({}, window.GetSize());
	Renderer::EnableBlending(true);		
	Graphics::GetLine2DRenderer().SetImmediateMode();

	Graphics::TextRenderData textRenderData = Graphics::GetTextRenderData("Hello :)", 140);	


	while (!Input::GetKeyState(Key::Escape).pressed)
	{
		Input::Update();

		Renderer::ClearTarget();		
		float time = TimePoint::GetRunTime();
		float r = Math::Sin(time + (float)Math::PI / 3 * 0) / 2 + 0.5f;
		float g = Math::Sin(time + (float)Math::PI / 3 * 1) / 2 + 0.5f;
		float b = Math::Sin(time + (float)Math::PI / 3 * 2) / 2 + 0.5f;
		
		Graphics::DrawBoxOutline2D(Vec2f(20, 125), Vec2f(20 + textRenderData.GetSize().x, 125 + Graphics::GetDefaultFont().GetBaselineDistance() * 140), Colors::Green, 1);
		Graphics::Write(textRenderData, Vec2i(20, 125), ColorRGBAf(r, g, b, 1));				

		Renderer::SwapWindowBuffers();
	}
}