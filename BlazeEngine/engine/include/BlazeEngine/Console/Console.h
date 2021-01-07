#pragma once
#include <vector>
#include "BlazeEngine/Core/EngineCore.h"
#include "BlazeEngine/DataStructures/Vector.h"

#define F_BLACK "$f0"
#define F_RED "$f4"
#define F_GREEN "$f2"
#define F_YELLOW "$f6"
#define F_BLUE "$f1"
#define F_MAGENTA "$f5"
#define F_CYAN "$f3"
#define F_WHITE "$f7"
#define F_DEFAULT F_WHITE

#define B_BLACK "$b0"
#define B_RED "$b4"
#define B_GREEN "$b2"
#define B_YELLOW "$b6"
#define B_BLUE "$b1"
#define B_MAGENTA "$b5"
#define B_CYAN "$b3"
#define B_WHITE "$b7"
#define B_DEFAULT B_BLACK

#define A_UNDERLINE "$au"
#define A_BOLD "$ab"
#define A_REVERSE "$ar"
#define A_DEFAULT "$ad"

namespace Blaze
{
	namespace Console
	{									
		enum class Key
		{
			Down = 0x102,
			Up = 0x103,
			Left = 0x104,
			Right = 0x105,
			Enter = '\n',
			Backspace = 8,
			Escape = 27,
		};

		void BLAZE_API SetSize(const Vec2i& size);				
		char BLAZE_API GetKeyPressed();		
	};

	bool BLAZE_API operator==(const char& c, const Console::Key& k);
	bool BLAZE_API operator==(const Console::Key& k, const char& c);
}