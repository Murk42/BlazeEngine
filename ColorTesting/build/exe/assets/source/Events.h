#pragma once
#include "BlazeEngine/BlazeEngine.h"
using namespace Blaze;

static void WindowResizeFunc(int x, int y, Window* win)
{	
	Renderer::SetViewport({ 0, 0 }, { x, y });
}

static void WindowCloseFunc(Window* win)
{
	BaseApplication::Stop();
}