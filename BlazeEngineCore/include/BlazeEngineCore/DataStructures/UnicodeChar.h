#pragma once
#include "BlazeEngineCore/BlazeEngineCoreDefines.h"

namespace Blaze
{	
	class BLAZE_CORE_API UnicodeChar
	{
	public:
		constexpr UnicodeChar();
		constexpr UnicodeChar(const UnicodeChar&);
		constexpr UnicodeChar(uint32 value);		
		constexpr UnicodeChar(const void* ptr, uintMem size);

		constexpr uint32 Value() const { return value; }
		//Returns the count of bytes that the character would take if it was
		//in a utf8 string
		constexpr uintMem UTF8Size() const;
		//Converts the unicode character into a utf-8 string if it can fit and 
		//returns the amount of bytes it takes up, if it can't fit it doesn't 
		//change the buffer and returns 0
		constexpr uintMem ToUTF8(void* buffer, uintMem size) const;

		constexpr uint32 Hash() const;

		constexpr UnicodeChar& operator++();
		constexpr UnicodeChar operator++(int);
		constexpr UnicodeChar& operator--();
		constexpr UnicodeChar operator--(int);

		constexpr bool operator==(const UnicodeChar& other) const;
		constexpr bool operator!=(const UnicodeChar& other) const;

		constexpr UnicodeChar& operator=(const UnicodeChar& other);
	private:
		uint32 value;

		static constexpr uintMem UTF8ToUnicode(const void* ptr, uintMem size, uint32& character);
	};
}