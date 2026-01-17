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

		void ProcessEvents() override;
		void ResetInputState();
		void CopyInputState();

		Vec2f GetMouseLastFramePos() const;
		Vec2f GetMouseLastFrameMovementSum() const;
		Vec2f GetMouseLastFrameScrollSum() const;
		Input::KeyFrameState GetKeyFrameState(Input::Key key) const;
		Input::MouseButtonFrameState GetMouseButtonFrameState(Input::MouseButton button) const;
		bool GetKeyState(Input::Key key) const;
		bool GetMouseButtonState(Input::MouseButton button) const;

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
		Vec2f mouseLastFramePos;
		Vec2f mouseCurrentFrameMovementSum;
		Vec2f mouseLastFrameMovementSum;
		Vec2f mouseCurrentScrollSum;
		Vec2f mouseLastFrameScrollSum;

		uint8 mouseButtonsComboDuringCurrentFrame[(uintMem)Input::MouseButton::COUNT];
		uint8 mouseButtonsComboDuringLastFrame[(uintMem)Input::MouseButton::COUNT];
		std::atomic_flag mouseButtonsCurrentState[(uintMem)Input::MouseButton::COUNT];
		std::bitset<(uintMem)Input::MouseButton::COUNT> pressedMouseButtonsDuringCurrentFrame;
		std::bitset<(uintMem)Input::MouseButton::COUNT> pressedMouseButtonsDuringLastFrame;
		std::bitset<(uintMem)Input::MouseButton::COUNT> releasedMouseButtonsDuringCurrentUpdate;
		std::bitset<(uintMem)Input::MouseButton::COUNT> releasedMouseButtonsDuringLastUpdate;
		std::bitset<(uintMem)Input::MouseButton::COUNT> downMouseButtonsDuringCurrentFrame;
		std::bitset<(uintMem)Input::MouseButton::COUNT> downMouseButtonsDuringLastFrame;

		std::atomic_flag keysCurrentState[(uintMem)Input::Key::COUNT];
		std::bitset<(uintMem)Input::Key::COUNT> pressedKeysDuringCurrentFrame;
		std::bitset<(uintMem)Input::Key::COUNT> pressedKeysDuringLastFrame;
		std::bitset<(uintMem)Input::Key::COUNT> releasedKeysDuringCurrentFrame;
		std::bitset<(uintMem)Input::Key::COUNT> releasedKeysDuringLastFrame;
		std::bitset<(uintMem)Input::Key::COUNT> downKeysDuringCurrentFrame;
		std::bitset<(uintMem)Input::Key::COUNT> downKeysDuringLastFrame;

		void ProcessEventQueue();
		void ProcessSDLEvent(SDL_Event* event, bool calledFromEventWatch);

		static bool SDLEventFilter(void* data, SDL_Event* event);
	};
}