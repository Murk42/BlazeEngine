#include "pch.h"
#include "BlazeEngine/UI/Common/System.h"

namespace Blaze::UI
{
	inline void SetScreenSize(Screen& screen, Vec2f size)
	{
		NodeTransform transform{
			.pos = { 0.0f, 0.0f},
			.parentPivot = { 0.0f, 0.0f },
			.pivot = { 0.0f, 0.0f },
			.size = size,
			.scale = 1.0f,
			.rotation = 0.0f
		};

		screen.SetTransform(transform);
	}

	System::System()
		: window(nullptr), resizeScreenWithWindow(false)
	{
	}
	System::System(Graphics::RendererRegistry newRegistry, Window& newWindow, bool newResizeScreenWithWindow)
		: System()
	{
		SetWindow(&newWindow, newResizeScreenWithWindow);
		SetRendererRegistry(newRegistry);
	}
	System::~System()
	{
		SetWindow(nullptr, false);
		destructionEventDispatcher.Call({ *this });
	}
	void System::SetScreen(Handle<Screen>&& newScreen)
	{
		screen = std::move(newScreen);
		inputSubSystem.SetScreen(screen);
		graphicsSubSystem.SetScreen(screen);

		if (resizeScreenWithWindow && !screen.Empty() && window != nullptr)
			SetScreenSize(*screen, Vec2f(window->GetSize()));
	}
	void System::SetWindow(Window* newWindow, bool newResizeScreenWithWindow)
	{
		if (window != nullptr && resizeScreenWithWindow)
			window->resizedEventDispatcher.RemoveHandler<&System::WindowResized>(*this);

		resizeScreenWithWindow = newResizeScreenWithWindow;
		window = newWindow;
		inputSubSystem.SetWindow(window);

		if (window != nullptr && resizeScreenWithWindow)
		{
			if (!screen.Empty())
				SetScreenSize(*screen, Vec2f(window->GetSize()));

			window->resizedEventDispatcher.AddHandler<&System::WindowResized>(*this);
		}

		windowChangedEventDispatcher.Call({ *this });
	}
	void System::SetRendererRegistry(Graphics::RendererRegistry newRegistry)
	{
		graphicsSubSystem.SetRendererRegistry(newRegistry);
	}
	void System::Render()
	{
		if (window != nullptr)
			graphicsSubSystem.Render(window->GetSize());
	}
	bool System::ProcessInputEvent(const Input::GenericInputEvent& event, bool isProcessed)
	{
		return inputSubSystem.ProcessInputEvent(event, isProcessed);
	}
	void System::WindowResized(const Window::ResizedEvent& event)
	{
		if (!screen.Empty())
			SetScreenSize(*screen, Vec2f(event.size));
	}
}