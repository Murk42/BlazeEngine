#pragma once
#include "BlazeEngineCore/BlazeEngineCoreDefines.h"

namespace Blaze
{	
	class BLAZE_CORE_API UnicodeChar
	{
	public:
		/*
			Constructs a unicode character with a integer value 0
		*/
		constexpr UnicodeChar();
		/*
			Copies a unicode character
		*/
		constexpr UnicodeChar(const UnicodeChar&);
		/*
			Constructs a unicode character from its integer value

			\param value      the integer value of the unicode character
		*/
		constexpr UnicodeChar(uint32 value);	
		/*
			Constructs a unicode character from an UTF-8 character sequence

			\param buffer      the buffer where the UTF-8 sequence is be stored
			\param bufferSize  the size of the buffer
		*/
		constexpr UnicodeChar(const void* buffer, uintMem bufferSize);

		/*
			\returns The unicode integer value of the character
		*/
		constexpr uint32 Value() const { return value; }
		/*			
			\returns The count of bytes that the character would take if it was in an utf8 format
		*/
		constexpr uintMem UTF8Size() const;
		/*
			Converts the unicode character into a UTF-8 string. If the buffer is not big enough 0 is returned. 
			The amount of bytes written is returned otherwise.

			\param buffer      the buffer where the UTF-8 sequence will be stored
			\param bufferSize  the available size of the buffer
			\returns The amount of bytes written or 0 if the buffer is not big enough
		*/
		constexpr uintMem ToUTF8(void* buffer, uintMem bufferSize) const;
		/*
			Reads a UTF-8 character sequence and transforms it into an unicode character

			\param buffer     the buffer where the UTF-8 sequence is be stored
			\param bufferSize the size of the buffer
			\returns The amount of bytes read or 0 if the buffer is not big enough
		*/
		constexpr uintMem FromUTF8(const void* buffer, uintMem bufferSize);

		/*
			Returns a hash of the unicode character (basically returns it's value)
		*/
		constexpr uint32 Hash() const;

		/*
			Pre-increments the unicode character
		*/
		constexpr UnicodeChar& operator++();
		/*
			Post-increments the unicode character
		*/
		constexpr UnicodeChar operator++(int);
		/*
			Pre-decrements the unicode character
		*/
		constexpr UnicodeChar& operator--();
		/*
			Post-icrements the unicode character
		*/
		constexpr UnicodeChar operator--(int);

		/*
			\returns True if the value is the same as other unicode characters value
		*/
		constexpr bool operator==(const UnicodeChar& other) const;
		/*
			\returns True if the value is not the same as other unicode characters value
		*/
		constexpr bool operator!=(const UnicodeChar& other) const;

		/*
			Copies the other unicode character to this one
			\returns Reference to this unicode character
		*/
		constexpr UnicodeChar& operator=(const UnicodeChar& other);
	private:
		uint32 value;		
	};
}