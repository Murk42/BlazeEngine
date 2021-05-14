#pragma once
#include <vector>
#include "BlazeEngine/Core/EngineCore.h"
#include "BlazeEngine/DataStructures/StringView.h"
#include <functional>

namespace Blaze
{
	namespace Console
	{								
		void BLAZE_API Write(const StringView& text);
		bool BLAZE_API ReadWord(String& value);				
	}
}