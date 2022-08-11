#pragma once
#include "BlazeEngine/Core/EngineCore.h"
#include "BlazeEngine/DataStructures/Common.h"

namespace Blaze
{
	class StringViewUTF8;
	class BLAZE_API UnicodeChar
	{
		uint32 value;
	public:
		UnicodeChar() = default;
		UnicodeChar(const UnicodeChar&);
		UnicodeChar(char ch);
		UnicodeChar(const char*);		

		inline uint32 Value() const { return value; }
		size_t UTF8Size();
		void ToUTF8(char* buffer);
	};
}