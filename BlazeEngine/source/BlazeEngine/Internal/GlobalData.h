#pragma once
#include "BlazeEngine/Input/Input.h"
#include <SDL2/SDL_scancode.h>
#include <SDL2/SDL_events.h>
#include "BlazeEngine/Console/Console.h"
#include "ThreadEventStack.h"
#include "ThreadTaskQueue.h"

namespace Blaze
{	
	class ConsoleOutputStream : public WriteStream
	{
		WriteStream* linkStream;
	public:
		uintMem Write(const void* ptr, uintMem byteCount) override
		{
			Console::Write(StringView((const char*)ptr, byteCount));

			return byteCount;
		}
		bool MovePosition(intMem offset) override
		{
			return false;
		}
		bool SetPosition(uintMem offset) override
		{
			return false;
		}
		bool SetPositionFromEnd(intMem offset) override
		{
			return false;
		}
		uintMem GetPosition() const override
		{
			return 0;
		}
		uintMem GetSize() const override
		{
			return 0;
		}
	};

	class GlobalData
	{	
	public:
		std::atomic_flag clientThreadExited = ATOMIC_FLAG_INIT;

		Blaze::TimingResult blazeInitTimings;		

		Blaze::Map<Blaze::Key, SDL_Scancode> scancodemap;
		Blaze::Map<SDL_Scancode, Blaze::Key> keymap;		
		EventStack<2048> inputEventStack;		
		Input::InputEventSystem inputEventSystem;

		Input::KeyState keyStates[(uint)Key::KeyCount];
		double keyComboTime = 0.2;
		std::mutex keyStatesMutex;

		SDL_Cursor* cursors[(uint)Input::CursorType::CursorCount];

		ConsoleOutputStream consoleOutputStream;
		
		std::atomic_flag windowSwappingSkipFlag;		

		uint32 mainThreadTaskEventIdentifier;
		uint32 clientThreadExitEventIdentifier;

		void ExecuteOnMainThread(std::function<void()>&& function);
		void CheckForMainThreadTask();
	private:
		std::mutex mainThreadTaskMutex;
		std::function<void()> mainThreadTask;
		std::atomic_flag mainThreadTaskFlag = ATOMIC_FLAG_INIT;
	};	
	
	
	extern GlobalData* globalData;
}
