#pragma once
#include "BlazeEngine/Core/Common/Handle.h"
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
		System(Graphics::RendererRegistry newRegistry, Window& window, bool resizeScreenWithWindow = true);
		~System();

		bool ProcessInputEvent(const Input::GenericInputEvent& event, bool isProcessed);
		void Render();

		template<IsDerivedFrom<Screen> S, typename ... Args> requires IsConstructibleFrom<S, Args...>
		S& SetScreen(Args&& ... args);
		void SetScreen(Handle<Screen>&& newScreen);
		Screen* GetScreen();

		void SetWindow(Window* newWindow, bool resizeScreenWithWindow = true);
		Window* GetWindow() const;

		void SetRendererRegistry(Graphics::RendererRegistry newRegistry);

		const InputSubSystem& GetInputSubSystem() const;
		const GraphicsSubSystem& GetGraphicsSubSystem() const;
	private:
		GraphicsSubSystem graphicsSubSystem;
		InputSubSystem inputSubSystem;
		Handle<Screen> screen;
		Window* window;

		bool resizeScreenWithWindow;

		void WindowResized(const Window::ResizedEvent& event);
	};

	template<IsDerivedFrom<Screen> S, typename ... Args> requires IsConstructibleFrom<S, Args...>
	inline S& System::SetScreen(Args&& ... args)
	{
		SetScreen(Handle<Screen>::CreateDerived<S>(std::forward<Args>(args)...));
		return *static_cast<S*>(screen.Ptr());
	}
	inline Screen* System::GetScreen() { return screen; }
	inline Window* System::GetWindow() const { return window; }
	inline const InputSubSystem& System::GetInputSubSystem() const { return inputSubSystem; }
	inline const GraphicsSubSystem& System::GetGraphicsSubSystem() const { return graphicsSubSystem; }
}