#pragma once
#include "BlazeEngine/Core/BlazeEngineCoreDefines.h"

namespace Blaze
{
	template<typename Char>
	class GenericStringView;

	namespace Details
	{
		template<typename Char>
		struct UnicodeCharConvertResultMaxLength
		{
			static constexpr uintMem value = 1;
		};

		template<>
		struct UnicodeCharConvertResultMaxLength<char>
		{
			static constexpr uintMem value = 1;
		};
		template<>
		struct UnicodeCharConvertResultMaxLength<char8_t>
		{
			static constexpr uintMem value = 4;
		};
		template<>
		struct UnicodeCharConvertResultMaxLength<char16_t>
		{
			static constexpr uintMem value = 2;
		};
		template<>
		struct UnicodeCharConvertResultMaxLength<char32_t>
		{
			static constexpr uintMem value = 1;
		};
	}

	template<typename Char>
	struct UnicodeCharConvertResult
	{
		Char chars[Details::UnicodeCharConvertResultMaxLength<Char>::value];
		uintMem count = 0;

		GenericStringView<Char> AsString() const { return { chars, count }; }
	};

	class BLAZE_API UnicodeChar
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
			Constructs a unicode character from its ASCII value

			\param value      the character
		*/
		constexpr UnicodeChar(char value);
		/*
			Constructs a unicode character from its UTF32 value

			\param value      the integer value of the unicode character
		*/
		constexpr UnicodeChar(char32_t value);
		/*
			Constructs a unicode character from a character sequence. For example multiple UTF-8 code-points
			can make a single unicode character, same for UTF-16. If the Char type is char or char32_t then only
			one character is copied.

			\param buffer      the buffer where the character sequence is stored
			\param bufferSize  the size of the buffer
		*/
		template<typename Char>
		constexpr UnicodeChar(const Char* buffer, uintMem bufferSize);

		/*
			\returns The unicode integer value of the character
		*/
		constexpr char32_t Value() const { return value; }
		/*
			\returns True if the character can be represented with an ASCII character
		*/
		constexpr bool IsASCII() const;

		/*
			\returns The count of code units that the character would take if it was in an other format
		*/
		template<typename Char>
		constexpr uintMem ToCharsSize();
		/*
			Converts the unicode character into a different format. If the buffer is not big enough 0 is returned.
			The amount of bytes written is returned otherwise.

			\returns The result of the conversion that stores the characters and the converted length
		*/
		template<typename Char>
		constexpr UnicodeCharConvertResult<Char> ToChars();
		/*
			Constructs a unicode character from a character sequence. For example multiple UTF-8 code-points
			can make a single unicode character, same for UTF-16. If the Char type is char or char32_t then only
			one character is copied.

			\param buffer     the buffer where the character sequence is be stored
			\param bufferSize the size of the buffer
			\returns The amount of bytes read or 0 if the buffer is not big enough
		*/
		template<typename Char>
		constexpr uintMem FromChars(const Char* buffer, uintMem bufferSize);

		/*
			Returns a hash of the unicode character (basically returns it's value)
		*/
		constexpr uint64 Hash() const;

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
		char32_t value;
	};
}

#include "BlazeEngine/Core/String/UnicodeCharImpl.h"