#include "pch.h"
#include "BlazeEngine/UI/Core/Screen.h"

namespace Blaze::UI
{
	Screen::Screen(Window* window, bool resizeWithWindow)
		: Node(), window(nullptr)
	{
		SetTransform({
			.pos = Vec2f(0.0f, 0.0f),
			.parentPivot = Vec2f(0.0f, 0.0f),
			.pivot = Vec2f(0.0f, 0.0f),
			.size = Vec2f(0.0f, 0.0f),
			.scale = 1.0f,
			.rotation = 0.0f,
			});

		SetWindow(window, resizeWithWindow);
	}
	Screen::~Screen()
	{
		dataMap.map.Clear();

		screenDestructionEventDispatcher.Call({ *this });

		if (window != nullptr)
			window->windowResizedEventDispatcher.RemoveHandler(*this);
	}
	void Screen::SetWindow(Window* window, bool resizeWithWindow)
	{
		if (window == this->window)
			return;

		Window* old = this->window;
		this->window = window;

		if (old != nullptr)
			old->windowResizedEventDispatcher.RemoveHandler(*this);

		if (window != nullptr)
		{
			if (resizeWithWindow)
			{
				auto transform = GetTransform();
				transform.size = (Vec2f)window->GetSize();
				SetTransform(transform);

				window->windowResizedEventDispatcher.AddHandler(*this);
			}
		}

		screenWindowChangedEventDispatcher.Call({ .oldWindow = old, .screen = *this });
	}
	void Screen::OnEvent(const Window::WindowResizedEvent& event)
	{
		auto transform = GetTransform();
		transform.size = (Vec2f)event.size;
		SetTransform(transform);
	}
}