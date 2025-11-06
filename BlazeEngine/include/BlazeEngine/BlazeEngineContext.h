#pragma once
#include "BlazeEngineCore/Container/Set.h"
#include "BlazeEngineCore/Math/Vector.h"
#include "BlazeEngineCore/Time/Timing.h"
#include "BlazeEngine/Window/Window.h"
#include "BlazeEngine/Input/Input.h"
#include "BlazeEngine/Console/ConsoleOutputStream.h"
#include "BlazeEngine/Common/ThreadEventStack.h"
#include <bitset>
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_mouse.h>
#include <mutex>
#include <condition_variable>

namespace Blaze
{
	struct BlazeEngineContextInitializationParameters
	{
		Timing initializationTimings;
		uint32 mainThreadSDLEventIdentifier;
		//This mode is used on Windows to support rendering while the window is being resized. On other platforms should be `false`
		bool separateClientThreadMode;
#ifdef BLAZE_PLATFORM_WINDOWS
		int nCmdShow;
#endif
	};
	class BlazeEngineContext
	{
	public:
		EventDispatcher<Display::DisplayEvent> displayEventDispatcher;
		EventDispatcher<Input::InputPreUpdateEvent> inputPreUpdateEventDispatcher;
		EventDispatcher<Input::InputPostUpdateEvent> inputPostUpdateEventDispatcher;

		//This context must be created after all of the libraries are loaded
		BlazeEngineContext(BlazeEngineContextInitializationParameters parameters);
		//This context must be destroy before all of the libraries are unloaded
		~BlazeEngineContext();

		void RegisterWindow(Window& window);
		void UnregisterWindow(Window& window);
		void MoveWindow(Window& window);
		Window* GetWindowFromHandle(void* handle);

		void UpdateInputAndEvents();
		SDL_Cursor* GetSystemCursor(Input::CursorType type);
		Vec2f GetMouseLastFramePos() const;
		Vec2f GetMouseLastFrameMovementSum() const;
		Vec2f GetMouseLastFrameScrollSum() const;
		Input::KeyFrameState GetKeyFrameState(Input::Key key) const;
		Input::MouseButtonFrameState GetMouseButtonFrameState(Input::MouseButton button) const;


		//Returns true if the function was successfully executed on the main thread, false if timeout has
		//elapsed. If the context was NOT created in separate client thread mode the function will be
		//executed on the calling thread and the timeout is ignored.
		bool ExecuteOnMainThread(void(*function)(void*), void* data, uint64 timeoutMilliseconds);
	private:
		const TimingResult initializationTimings;
		const uint32 mainThreadSDLEventIdentifier;
		const bool separateClientThreadMode;
#ifdef BLAZE_PLATFORM_WINDOWS
		const int nCmdShow; //Value passed in wWinMain
#endif
		static constexpr uint EVENT_STACK_SIZE = 8192;
		EventStack<EVENT_STACK_SIZE> clientEventStack;

		//Guards everything bellow, since everything above shouldn't change
		std::mutex windowsMutex;
		Map<void*, Window*> windows;

		std::mutex cursorsMutex;
		SDL_Cursor* cursors[(uint)Input::CursorType::CursorCount];

		/*
		------------------------------------ Used for state-based/frame-based input handling ------------------------------------
		*/
		std::mutex inputCurrentFrameMutex;

		Vec2f mouseCurrentPos;
		Vec2f mouseLastFramePos;
		Vec2f mouseCurrentFrameMovementSum;
		Vec2f mouseLastFrameMovementSum;
		Vec2f mouseCurrentScrollSum;
		Vec2f mouseLastFrameScrollSum;

		uint8 mouseButtonsComboDuringCurrentFrame[(uintMem)Input::MouseButton::COUNT];
		uint8 mouseButtonsComboDuringLastFrame[(uintMem)Input::MouseButton::COUNT];
		std::bitset<(uintMem)Input::MouseButton::COUNT> pressedMouseButtonsDuringCurrentFrame;
		std::bitset<(uintMem)Input::MouseButton::COUNT> pressedMouseButtonsDuringLastFrame;
		std::bitset<(uintMem)Input::MouseButton::COUNT> releasedMouseButtonsDuringCurrentUpdate;
		std::bitset<(uintMem)Input::MouseButton::COUNT> releasedMouseButtonsDuringLastUpdate;
		std::bitset<(uintMem)Input::MouseButton::COUNT> downMouseButtonsDuringCurrentFrame;
		std::bitset<(uintMem)Input::MouseButton::COUNT> downMouseButtonsDuringLastFrame;

		std::bitset<(uintMem)Input::Key::COUNT> pressedKeysDuringCurrentFrame;
		std::bitset<(uintMem)Input::Key::COUNT> pressedKeysDuringLastFrame;
		std::bitset<(uintMem)Input::Key::COUNT> releasedKeysDuringCurrentFrame;
		std::bitset<(uintMem)Input::Key::COUNT> releasedKeysDuringLastFrame;
		std::bitset<(uintMem)Input::Key::COUNT> downKeysDuringCurrentFrame;
		std::bitset<(uintMem)Input::Key::COUNT> downKeysDuringLastFrame;

		void ResetInputState();
		void CopyInputState();
		/*
		----------------------------------------------------------------------------------------------------------------------------
		*/

		//Thread safe - can be called from any thread
		Window* GetWindowFromWindowID(SDL_WindowID id);

		//Thread safe - can be called from any thread
		template<typename T>
		void AddToClientEventStack(const T& event, EventDispatcher<T>* dispatcher);

		static bool SDLEventWatch(void* data, SDL_Event* event);
	};

	extern BlazeEngineContext* blazeEngineContext;

	template<typename T>
	inline void BlazeEngineContext::AddToClientEventStack(const T& event, EventDispatcher<T>* dispatcher)
	{
		const char* name = typeid(T).name();
		uintMem length = strlen(name);
		clientEventStack.Add<T>(StringView(name, length), [](const T& event, EventDispatcher<T>* dispatcher) {
			dispatcher->Call(event);
			}, event, dispatcher);
	}
}
