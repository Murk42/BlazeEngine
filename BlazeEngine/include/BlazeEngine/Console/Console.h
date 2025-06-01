#pragma once
#include "BlazeEngineCore/Format/Format.h"
#include "BlazeEngine/BlazeEngineDefines.h"

namespace Blaze
{
	namespace Console
	{
		template<Formatable ... Args>
		void Write(StringViewUTF8 text, const Args& ... args);

		template<>
		BLAZE_API void Write<>(StringViewUTF8 text);

		BLAZE_API StringUTF8 Read();

		template<Formatable ...Args>
		void Write(StringViewUTF8 text, const Args & ...args)
		{
			Write(FormatUTF8(text, args...));
		}		
	}

	namespace Debug::Logger
	{
		BLAZE_API void AddOutputToConsole();
		BLAZE_API void RemoveOutputFromConsole();
	}
}