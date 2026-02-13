#pragma once
#include "BlazeEngineCore/Common/Format.h"

namespace Blaze
{
	namespace Console
	{
		template<Formattable<char8_t> ... Args>
		void Write(u8StringView text, const Args& ... args);

		template<>
		BLAZE_API void Write<>(u8StringView text);

		BLAZE_API u8String Read();

		template<Formattable<char8_t> ...Args>
		void Write(u8StringView text, const Args & ...args)
		{
			Write(Format(text, args...));
		}
	}

	namespace Debug::Logger
	{
		BLAZE_API void AddOutputToConsole();
		BLAZE_API void RemoveOutputFromConsole();
	}
}