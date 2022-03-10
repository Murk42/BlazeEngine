#pragma once
#include "BlazeEngine/Core/EngineCore.h"
#include "BlazeEngine/DataStructures/StringView.h"
#include "BlazeEngine/DataStructures/StringViewUTF8.h"

namespace Blaze
{	
	namespace Console
	{								
		void BLAZE_API Write(char text);
		void BLAZE_API Write(StringView text);
		void BLAZE_API WriteLine(StringView text);		

		String BLAZE_API Read();
	}
}