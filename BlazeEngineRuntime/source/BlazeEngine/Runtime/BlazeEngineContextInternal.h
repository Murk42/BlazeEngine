#pragma once
#include "BlazeEngine/Core/Math/Vector.h"
#include "BlazeEngine/Core/Common/EventQueue.h"
#include "BlazeEngine/Runtime/IO/Window.h"
#include "BlazeEngine/Runtime/IO/Input.h"
#include "BlazeEngine/Runtime/BlazeEngineContext.h"
#include <bitset>
#include <mutex>
#include "BlazeEngine/External/SDL/SDL.h"

namespace Blaze
{
	
	class BlazeEngineContextInternal : public BlazeEngineContext
	{
	public:
		using GlobalGenericEvent = Variant<Display::DisplayEvent>;

		EventDispatcher<Display::DisplayEvent> displayEventDispatcher;
		EventDispatcher<Input::InputPreUpdateEvent> inputPreUpdateEventDispatcher;
		EventDispatcher<Input::InputPostUpdateEvent> inputPostUpdateEventDispatcher;

		//This context must be created after all of the libraries are loaded
		BlazeEngineContextInternal(BlazeEngineContext::InitializationParameters parameters);
		//This context must be destroy before all of the libraries are unloaded
		~BlazeEngineContextInternal() override;

		void PollEvents() override;
		bool WaitForEvents(float seconds) override;
		
		void NudgeMainThread() override;

		bool GetKeyState(Input::Key key) const;
		bool GetMouseButtonState(Input::MouseButton button) const;
		Array<Input::MouseData> GetMice();

		void RegisterWindow(Window& window);
		void UnregisterWindow(Window& window);
		void MoveWindow(Window& window);
		Window* GetWindowFromHandle(SDL_Window* handle);
		Window* GetWindowFromWindowID(SDL_WindowID id);

		static BlazeEngineContextInternal* GetEngineContext();
	private:
		struct WindowData
		{
			Window* object;
		};

		SDL_EventFilter previousEventFilter;
		void* previousEventFilterUserData;
		EventQueue<GlobalGenericEvent> eventQueue;

		std::mutex windowsMutex;
		Map<SDL_Window*, WindowData> windowsData;

		//std::mutex cursorsMutex;
		//void* cursors[(uint)Input::CursorType::CursorCount]{ };

		/*
		------------------------------------ State-based/frame-based input handling ------------------------------------
		*/
		mutable std::mutex inputStateMutex;

		Vec2f mouseCurrentPos;
		
		std::atomic_flag mouseButtonsCurrentState[(uintMem)Input::MouseButton::COUNT];
		std::atomic_flag keysCurrentState[(uintMem)Input::Key::COUNT];		

		Array<Input::MouseData> mice;

		uint32 nudgeEventNumber;

		void UpdateMouseData(const Input::MouseData& data);
		void EraseMouseData(Input::MouseID id);

		void ProcessEventQueue();
		void ProcessSDLEvent(SDL_Event* event, bool calledFromEventWatch);

		static bool SDLEventFilter(void* data, SDL_Event* event);
	};
}