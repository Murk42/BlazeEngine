#pragma once
#include <vector>
#include "BlazeEngine/Core/EngineCore.h"
#include "BlazeEngine/DataStructures/StringView.h"
#include <functional>

namespace Blaze
{
	//thread-safe
	namespace Console
	{								
		void BLAZE_API Write(StringView text);
		void BLAZE_API Write(char text);
		void BLAZE_API WriteLine(StringView text);

		bool BLAZE_API ReadWord(String& value);	
		void BLAZE_API ClearInput();
		void BLAZE_API WaitForInput();
	}
}