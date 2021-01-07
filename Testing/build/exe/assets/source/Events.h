#pragma once

static void WindowCloseEvent(Window* win)
{
	BaseApplication::Stop();
}

static void WindowSizeChangedEvent(int w, int h, Window* win)
{
	Logger::AddLog(LogType::Message, __FUNCTION__, String(format_string, "Window size set to (%d, %d)", w, h));
}

static void SetupEvents()
{
	Input::SetEventFunction(InputEvent::WindowSizeChanged, WindowSizeChangedEvent);
	Input::SetEventFunction(InputEvent::WindowClosed, WindowCloseEvent);	
}