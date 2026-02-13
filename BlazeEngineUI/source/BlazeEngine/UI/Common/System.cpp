#include "pch.h"
#include "BlazeEngine/UI/Common/System.h"

namespace Blaze::UI
{
	static void SetNodeSize(Node& node, Vec2f size)
	{
		NodeTransform transform = node.GetTransform();
		
		transform.size = size;

		node.SetTransform(transform);
	}
	static void SetNodeScale(Node& node, float scale)
	{
		NodeTransform transform = node.GetTransform();

		transform.scale = scale;

		node.SetTransform(transform);
	}

	System::System(Graphics::GraphicsContextBase& graphicsContext)
		: window(nullptr), resizeScreenWithWindow(false), graphicsSubSystem(graphicsContext)
	{
	}
	System::System(Graphics::GraphicsContextBase& graphicsContext, Graphics::RendererRegistry newRegistry, Window& newWindow, bool newResizeScreenWithWindow)
		: System(graphicsContext)
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

		if (!screen.Empty() && window != nullptr)
		{
			if (resizeScreenWithWindow)
				SetNodeSize(*screen, Vec2f(window->GetSize()));

			SetNodeScale(*screen, window->GetContentScale());
		}

		screen->Update();

		inputSubSystem.SetScreen(screen);
		graphicsSubSystem.SetScreen(screen);
	}
	void System::SetWindow(Window* newWindow, bool newResizeScreenWithWindow)
	{
		if (window != nullptr)
		{
			if (resizeScreenWithWindow)
				window->resizedEventDispatcher.RemoveHandler<&System::WindowResized>(*this);

			window->contentScaleChangedEventDispatcher.RemoveHandler<&System::WindowContentScaleChanged>(*this);
		}

		resizeScreenWithWindow = newResizeScreenWithWindow;
		window = newWindow;

		if (window != nullptr)
		{
			if (resizeScreenWithWindow)
			{
				if (screen != nullptr)
					SetNodeSize(*screen, Vec2f(window->GetSize()));

				window->resizedEventDispatcher.AddHandler<&System::WindowResized>(*this);
			}

			window->contentScaleChangedEventDispatcher.AddHandler<&System::WindowContentScaleChanged>(*this);

			if (screen != nullptr)
				SetNodeScale(*screen, window->GetContentScale());
		}
	

		inputSubSystem.SetWindow(window);

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
	Input::EventProcessedState System::ProcessInputEvent(const Input::GenericInputEvent& event, bool processed)
	{
		if (auto result = inputSubSystem.ProcessInputEvent(event, processed))
			return result;

		return Input::EventProcessedState::NotProcessed;
	}
	void System::WindowResized(const Window::ResizedEvent& event)
	{
		if (!screen.Empty())
			SetNodeSize(*screen, Vec2f(event.size));
	}
	void System::WindowContentScaleChanged(const Window::ContentScaleChangedEvent& event)
	{
		if (screen != nullptr)
			SetNodeScale(*screen, event.newContentScale);
	}
}