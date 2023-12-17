#pragma once
#include "BlazeEngine/Input/Key.h"
#include <SDL2/SDL_scancode.h>
#include "BlazeEngine/Console/Console.h"

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

	struct GlobalData
	{			
		Blaze::Map<Blaze::Key, SDL_Scancode> scancodemap;
		Blaze::Map<SDL_Scancode, Blaze::Key> keymap;
		
		Blaze::TimingResult blazeInitTimings;		

		ConsoleOutputStream consoleOutputStream;
	};	
	
	extern GlobalData* globalData;
}