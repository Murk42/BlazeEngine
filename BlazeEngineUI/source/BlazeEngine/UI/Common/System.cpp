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

	System::System()
		: window(nullptr), resizeScreenWithWindow(false), graphicsSubSystem(), screen(nullptr)
	{
	}
	System::System(Screen& screen)
		: System()
	{
		SetScreen(&screen);
	}
	System::~System()
	{
		SetWindow(nullptr, false);
		destructionEventDispatcher.Call({ *this });
	}
	void System::SetScreen(Screen* newScreen)
	{
		screen = newScreen;

		inputSubSystem.SetScreen(screen);
		graphicsSubSystem.SetScreen(screen);

		if (screen != nullptr && window != nullptr)
		{
			if (resizeScreenWithWindow)
				SetNodeSize(*screen, Vec2f(window->GetSize()));

			screen->SetScale(window->GetContentScale());
		}
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
				screen->SetScale(window->GetContentScale());
		}
	

		inputSubSystem.SetWindow(window);

		windowChangedEventDispatcher.Call({ *this });

	}
	void System::InitializeGraphics(Graphics::GraphicsContext& graphicsContext, Graphics::RendererRegistry newRegistry)
	{
		graphicsSubSystem.Initialize(graphicsContext, newRegistry);
	}
	void System::Render(const Graphics::RenderContext& renderContext)
	{
		graphicsSubSystem.Render(renderContext);
	}
	Input::EventProcessedState System::ProcessInputEvent(const Input::GenericInputEvent& event, bool processed)
	{
		if (auto result = inputSubSystem.ProcessInputEvent(event, processed))
			return result;

		return Input::EventProcessedState::NotProcessed;
	}
	void System::WindowResized(const Window::ResizedEvent& event)
	{
		if (screen != nullptr)
			SetNodeSize(*screen, Vec2f(event.size));
	}
	void System::WindowContentScaleChanged(const Window::ContentScaleChangedEvent& event)
	{
		if (screen != nullptr)
			screen->SetScale(event.newContentScale);
	}
}