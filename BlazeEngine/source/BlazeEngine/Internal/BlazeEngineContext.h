#pragma once
#include "BlazeEngineCore/BlazeEngineCoreDefines.h"
#include "BlazeEngineCore/DataStructures/Hash.h"
#include "BlazeEngineCore/DataStructures/Map.h"
#include "BlazeEngineCore/DataStructures/Set.h"
#include "BlazeEngineCore/Math/Vector.h"
#include "BlazeEngineCore/Memory/MallocAllocator.h"
#include "BlazeEngineCore/Types/TemplateGroup.h"
#include "BlazeEngineCore/Types/TypeTraits.h"
#include "BlazeEngineCore/Utilities/Timing.h"
#include "BlazeEngine/Window/Window.h"
#include "BlazeEngine/Input/Input.h"
#include "BlazeEngine/Console/ConsoleOutputStream.h"
#include "ThreadEventStack.h"
#include <atomic>
#include <concepts>
#include <mutex>
#include <thread>
#include <bitset>
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_mouse.h>
#include <SDL3/SDL_scancode.h>

namespace Blaze
{		
	class BlazeEngineContext
	{	
	public:
		static constexpr bool createClientThread = true;
		static constexpr uint MAX_PRESSED_KEYS_AT_SAME_TIME = 32;
		static constexpr uint MAX_RELEASED_KEYS_AT_SAME_TIME = 32;
		static constexpr uint EVENT_STACK_SIZE = 2048 * 2;
						
		const std::thread::id mainThreadID;

		//Should not change throughout the lifetime of the process after initialization
		Blaze::TimingResult blazeInitTimings;
		uint32 mainThreadTaskEventIdentifier;
		uint32 clientThreadExitEventIdentifier;
		
		//Guards everything bellow, since everything above shouldn't change
		std::mutex mutex;
		
		std::atomic_flag clientThreadExited = ATOMIC_FLAG_INIT;

		EventStack<EVENT_STACK_SIZE> eventStack;
				
		//TODO replace this mutex
		Map<Display::DisplayID, Display::DisplayData> displaysData;

		uint primaryDesktopHeight;
		Vec2f currentDesktopMousePos;
		Vec2f currentUpdateMouseMovementSum;
		Vec2f lastUpdateDesktopMousePos;
		Vec2f lastUpdateMouseMovement;

		std::mutex windowsMutex;
		Set<Window*> windows;
				
		//A value is set if the key was pressed at least once between the Input::Update calls, otherwise its not set
		std::bitset<(uintMem)Keyboard::Key::COUNT> pressedKeysDuringUpdate;
		//A value is set if the key was pressed at least once between the Input::Update calls, otherwise its not set
		std::bitset<(uintMem)Keyboard::Key::COUNT> releasedKeysDuringUpdate;		
		//A value is set if the key is currently down
		std::bitset<(uintMem)Keyboard::Key::COUNT> downKeys;

		SDL_Cursor* cursors[(uint)Input::CursorType::CursorCount];		

		EventDispatcher<Window::WindowResizedEvent>               windowResizedEventDispatcher;
		EventDispatcher<Window::WindowMovedEvent>                 windowMovedEventDispatcher;
		EventDispatcher<Window::WindowMinimizedEvent>             windowMinimizedEventDispatcher;
		EventDispatcher<Window::WindowMaximizedEvent>             windowMaximizedEventDispatcher;
		EventDispatcher<Window::WindowFocusGainedEvent>           windowFocusGainedEventDispatcher;
		EventDispatcher<Window::WindowFocusLostEvent>             windowFocusLostEventDispatcher;
		EventDispatcher<Window::WindowCloseEvent>                 windowCloseEventDispatcher;
		EventDispatcher<Window::WindowMouseEnterEvent>            windowMouseEnterEventDispatcher;
		EventDispatcher<Window::WindowMouseLeaveEvent>            windowMouseLeaveEventDispatcher;
		EventDispatcher<Window::WindowDisplayChangedEvent>        windowDisplayChangedEventDispatcher;
		EventDispatcher<Window::WindowDestroyedEvent>             windowDestroyedEventDispatcher;
		EventDispatcher<Keyboard::KeyDownEvent>                   keyDownEventDispatcher;
		EventDispatcher<Keyboard::KeyUpEvent>                     keyUpEventDispatcher;
		EventDispatcher<Keyboard::TextInputEvent>                 textInputEventDispatcher;
		EventDispatcher<Mouse::MouseButtonDownEvent>              mouseButtonDownEventDispatcher;
		EventDispatcher<Mouse::MouseButtonUpEvent>                mouseButtonUpEventDispatcher;
		EventDispatcher<Mouse::MouseMotionEvent>                  mouseMotionEventDispatcher;
		EventDispatcher<Mouse::MouseScrollEvent>                  mouseScrollEventDispatcher;
		EventDispatcher<Input::InputPreUpdateEvent>               inputPreUpdateEventDispatcher;
		EventDispatcher<Input::InputPostUpdateEvent>              inputPostUpdateEventDispatcher;		
		EventDispatcher<Display::DisplayOrientationChangedEvent>  displayOrientationChangedEventDispatcher;
		EventDispatcher<Display::DisplayAddedEvent>               displayAddedEventDispatcher;
		EventDispatcher<Display::DisplayRemovedEvent>             displayRemovedEventDispatcher;
		EventDispatcher<Display::DisplayMovedEvent>               displayMovedEventDispatcher;
		EventDispatcher<Display::DisplayDesktopModeChangedEvent>  displayDesktopModeChangedEventDispatcher;
		EventDispatcher<Display::DisplayCurrentModeChangedEvent>  displayCurrentModeChangedEventDispatcher;
		EventDispatcher<Display::DisplayContentScaleChangedEvent> displayContentScaleChangedEventDispatcher;

		
#ifdef BLAZE_PLATFORM_WINDOWS
		//Value passed in wWinMain
		int nCmdShow;
#endif

		//Used to queue up threads that wait for the main thread to execute tasks
		std::mutex mainThreadTaskMutex;
		void(*mainThreadTaskfunction)(void*);
		void* mainThreadTaskUserData;		
		std::atomic_flag mainThreadTaskFlag = ATOMIC_FLAG_INIT;

		BlazeEngineContext();
		~BlazeEngineContext();

		//Can be run on any thread
		void ExecuteTaskOnMainThread(void(*function)(void*), void* userData);
		template<auto MemberFunctionPointer> requires std::same_as<MemberFunctionPointerTypeArgumentTypes<decltype(MemberFunctionPointer)>, TemplateGroup<>>
		void ExecuteTaskOnMainThread(MemberFunctionPointerTypeClassType<decltype(MemberFunctionPointer)>& object);
		//Must be run on the main thread
		void ExecuteMainThreadTask();	

		void UpdateInputState();
		void ProcessAllEvents();
		//This function might be called from the main thread, the client thread, or any other thread (internal SDL thread if called by a event watcher).
		void HandleSDLEvent(const SDL_Event& event);		
	};		
	
	extern ConsoleOutputStream consoleOutputStream;
	extern BlazeEngineContext blazeEngineContext;

	template<auto MemberFunctionPointer> requires std::same_as<MemberFunctionPointerTypeArgumentTypes<decltype(MemberFunctionPointer)>, TemplateGroup<>>
	inline void BlazeEngineContext::ExecuteTaskOnMainThread(MemberFunctionPointerTypeClassType<decltype(MemberFunctionPointer)>& object)
	{
		using Class = MemberFunctionPointerTypeClassType<decltype(MemberFunctionPointer)>;
		ExecuteTaskOnMainThread([](void* userData) { (((Class*)userData)->*MemberFunctionPointer)(); }, &object);
	}
}
