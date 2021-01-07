#pragma once
#define BLAZE_ONLY_CORE
#include "BlazeEngine/BlazeEngine.h"
using namespace Blaze;

static void WindowResizeFunc(int x, int y, Window* win)
{
	auto app = App::Instance();
	app->camera.Projection.Perspective.aspectRatio = (float)x / y;	
	app->uiCamera.SetOrthographic(1, (float)x / y, -1, 1, false);
	Renderer::SetViewport(vec2i(0, 0), vec2i(x, y));
}

static void WindowCloseFunc(Window* win)
{
	Application::Stop();
}