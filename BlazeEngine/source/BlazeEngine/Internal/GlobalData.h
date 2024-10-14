#pragma once
#include "BlazeEngine/Input/Input.h"
#include <SDL2/SDL_scancode.h>
#include <SDL2/SDL_events.h>
#include "BlazeEngine/Console/ConsoleOutputStream.h"
#include "ThreadEventStack.h"
#include "ThreadTaskQueue.h"

namespace Blaze
{		
	class BlazeEngineContext
	{	
	public:
		static constexpr uint MAX_PRESSED_KEYS_AT_SAME_TIME = 32;
		static constexpr uint MAX_RELEASED_KEYS_AT_SAME_TIME = 32;
		static constexpr uint EVENT_STACK_SIZE = 2048;
		
		const Blaze::Map<Key, SDL_Scancode, Hash<Key>, MallocAllocator> scancodemap;
		const Blaze::Map<SDL_Scancode, Blaze::Key, Hash<SDL_Scancode>, MallocAllocator> keymap;		
		const std::thread::id mainThreadID;


		//Should not change throughout the lifetime of the process after initialization
		Blaze::TimingResult blazeInitTimings;
		uint32 mainThreadTaskEventIdentifier;
		uint32 clientThreadExitEventIdentifier;
		
		//Guards everything bellow, since everything above shouldn't change
		//std::mutex mutex;
		
		std::atomic_flag clientThreadExited = ATOMIC_FLAG_INIT;

		EventStack<EVENT_STACK_SIZE> eventStack;
				
		//TODO replace this mutex
		uint desktopHeight;
		Vec2i desktopMousePos;			
		Vec2i desktopMouseMovement;

		//std::mutex windowsMutex;
		Set<WindowBase*> windows;

		double keyComboTime = 0.2;

		SDL_Cursor* cursors[(uint)Input::CursorType::CursorCount];		

		Input::InputEventSystem inputEventSystem;		
		
#ifdef BLAZE_PLATFORM_WINDOWS
		//Value passed in wWinMain
		int nCmdShow;
#endif

		//Used to queue up threads that wait for the main thread to execute tasks
		//std::mutex mainThreadTaskMutex;
		std::function<void()> mainThreadTask;
		std::atomic_flag mainThreadTaskFlag = ATOMIC_FLAG_INIT;

		BlazeEngineContext();
		~BlazeEngineContext();

		//Must be run on any but the main thread
		void ExecuteTaskOnMainThread(std::function<void()>&& function);
		//Must be run on the main thread
		void ExecuteMainThreadTask();	

		void HandleWindowKeyEvent(const Input::Events::KeyPressed& event);
		void HandleWindowKeyEvent(const Input::Events::KeyReleased& event);
		void HandleWindowResizeEvent(const Input::Events::WindowResizedEvent& event);
	};		
	
	extern ConsoleOutputStream consoleOutputStream;
	extern BlazeEngineContext blazeEngineContext;
}
