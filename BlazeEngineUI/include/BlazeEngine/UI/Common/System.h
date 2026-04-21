#pragma once
#include "BlazeEngine/Runtime/IO/Window.h"
#include "BlazeEngine/UI/Graphics/GraphicsSubSystem.h"
#include "BlazeEngine/UI/Input/InputSubSystem.h"

namespace Blaze::UI
{
	class BLAZE_API System
	{
	public:
		struct WindowChangedEvent
		{
			System& system;
		};
		struct DestructionEvent
		{
			System& system;
		};
		EventDispatcher<WindowChangedEvent> windowChangedEventDispatcher;
		EventDispatcher<DestructionEvent> destructionEventDispatcher;

		System();
		System(Screen& screen);
		~System();

		Input::EventProcessedState ProcessInputEvent(const Input::GenericInputEvent& event, bool processed);
		void Render(const Graphics::RenderContext& renderContext);
		
		void SetScreen(Screen* screen);
		Screen* GetScreen();

		void SetWindow(Window* newWindow, bool resizeScreenWithWindow = true);
		Window* GetWindow() const;

		void InitializeGraphics(Graphics::GraphicsContext& graphicsContext, Graphics::RendererRegistry newRegistry);

		const InputSubSystem& GetInputSubSystem() const;
		const GraphicsSubSystem& GetGraphicsSubSystem() const;
	private:
		GraphicsSubSystem graphicsSubSystem;
		InputSubSystem inputSubSystem;
		Screen* screen;
		Window* window;

		bool resizeScreenWithWindow;

		void WindowResized(const Window::ResizedEvent& event);
		void WindowContentScaleChanged(const Window::ContentScaleChangedEvent& event);
	};
	
	inline Screen* System::GetScreen() { return screen; }
	inline Window* System::GetWindow() const { return window; }
	inline const InputSubSystem& System::GetInputSubSystem() const { return inputSubSystem; }
	inline const GraphicsSubSystem& System::GetGraphicsSubSystem() const { return graphicsSubSystem; }
}