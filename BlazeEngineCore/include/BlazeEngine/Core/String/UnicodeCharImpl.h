#include "UnicodeChar.h"
#pragma once

namespace Blaze
{
	constexpr UnicodeChar::UnicodeChar()
		: value(0)
	{
	}
	constexpr UnicodeChar::UnicodeChar(const UnicodeChar& ch)
		: value(ch.value)
	{
	}
	constexpr UnicodeChar::UnicodeChar(char value)
		: value(static_cast<char32_t>(value))
	{
	}
	constexpr UnicodeChar::UnicodeChar(char32_t value)
		: value(value)
	{
	}
	template<typename Char>
	constexpr UnicodeChar::UnicodeChar(const Char* buffer, uintMem bufferSize)
	{
		FromChars(buffer, bufferSize);
	}
	constexpr bool UnicodeChar::IsASCII() const
	{
		return value <= 0x7F;
	}
	template<typename Char>
	inline constexpr uintMem UnicodeChar::ToCharsSize()
	{
		if constexpr (SameAs<Char, char>)
		{
			return 1;
		}
		else if constexpr (SameAs<Char, char8_t>)
		{
			if (value <= 0x7F)
				return 1;
			else if (value <= 0x7FF)
				return 2;
			else if (value <= 0xFFFF)
				return 3;
			else if (value <= 0x10FFFF)
				return 4;

			return -1;
		}
		else if constexpr (SameAs<Char, char16_t>)
		{
			if (value <= 0xFFFF)
			{
				// exclude UTF-16 surrogate range
				if (value >= 0xD800 && value <= 0xDFFF)
					return -1; // invalid code point
				return 1;
			}
			else if (value <= 0x10FFFF)
			{
				return 2; // surrogate pair
			}

			return -1; // invalid code point
		}
		else if constexpr (SameAs<Char, char32_t>)
		{
			return 1;
		}
	}
	template<typename Char>
	constexpr UnicodeCharConvertResult<Char> UnicodeChar::ToChars()
	{
		if constexpr (SameAs<Char, char>)
		{
			UnicodeCharConvertResult<char> result;
			result.chars[0] = static_cast<char>(value);
			result.count = 1;
			return result;
		}
		else if constexpr (SameAs<Char, char8_t>)
		{
			UnicodeCharConvertResult<char8_t> result;
			if (value <= 0x7F)
			{
				result.chars[0] = static_cast<char8_t>(value);
				result.count = 1;
			}
			else if (value <= 0x7FF)
			{
				result.chars[0] = static_cast<char8_t>((value >> 6) | 0xC0);
				result.chars[1] = static_cast<char8_t>((value & 0x3F) | 0x80);
				result.count = 2;
			}
			else if (value <= 0xFFFF)
			{
				char32_t _value = value;

				// surrogate code points are invalid; emit replacement instead
				if (value >= 0xD800 && value <= 0xDFFF)
					_value = 0xFFFD;

				result.chars[0] = static_cast<char8_t>((_value >> 12) | 0xE0);
				result.chars[1] = static_cast<char8_t>(((_value >> 6) & 0x3F) | 0x80);
				result.chars[2] = static_cast<char8_t>((_value & 0x3F) | 0x80);
				result.count = 3;
			}
			else
			{
				char32_t _value = value;

				if (value > 0x10FFFF)
					_value = 0xFFFD;

				result.chars[0] = static_cast<char8_t>((_value >> 18) | 0xF0);
				result.chars[1] = static_cast<char8_t>(((_value >> 12) & 0x3F) | 0x80);
				result.chars[2] = static_cast<char8_t>(((_value >> 6) & 0x3F) | 0x80);
				result.chars[3] = static_cast<char8_t>((_value & 0x3F) | 0x80);
				result.count = 4;
			}

			return result;
		}
		else if constexpr (SameAs<Char, char16_t>)
		{
			UnicodeCharConvertResult<char16_t> result;

			if (value <= 0xFFFF)
			{
				char32_t _value = value;

				// don't write raw surrogates
				if (value >= 0xD800 && value <= 0xDFFF)
					_value = 0xFFFD;

				result.chars[0] = static_cast<char16_t>(_value);
				result.count = 1;
			}
			else
			{
				char32_t _value = value;

				if (value > 0x10FFFF)
					_value = 0xFFFD;

				_value -= 0x10000;

				result.chars[0] = static_cast<char16_t>(0xD800 + (_value >> 10));
				result.chars[1] = static_cast<char16_t>(0xDC00 + (_value & 0x3FF));
				result.count = 2;
			}

			return result;
		}
		else if constexpr (SameAs<Char, char32_t>)
		{
			UnicodeCharConvertResult<char32_t> result;

			result.chars[0] = value;
			result.count = 1;

			return result;
		}
		else
			static_assert("Conversion not supported");
	}
	template<typename Char>
	constexpr uintMem UnicodeChar::FromChars(const Char* buffer, uintMem bufferSize)
	{
		if constexpr (SameAs<Char, char>)
		{
			if (buffer == nullptr || bufferSize == 0)
				return 0;

			value = static_cast<char32_t>(*buffer);
		}
		else if constexpr (SameAs<Char, char8_t>)
		{
			// If no bytes left, return replacement
			if (buffer == nullptr || bufferSize == 0)
				return 0;

			// Read first byte as an unsigned value
			uint32 b0 = static_cast<uint32>(buffer[0]);

			// 1-byte (ASCII)
			if (b0 < 0x80)
			{
				value = static_cast<char32_t>(b0);
				return 1;
			}

			// 2-byte
			if ((b0 >> 5) == 0x6)
			{
				if (bufferSize < 2)
					return 0; // missing continuation

				uint32 b1 = static_cast<uint32>(buffer[1]);

				if ((b1 & 0xC0) != 0x80) { return 0; } // invalid continuation (consume it to avoid infinite loop)

				uint32 cp = ((b0 & 0x1F) << 6) | (b1 & 0x3F);
				// reject overlong (must be >= 0x80)
				if (cp < 0x80)
					return 0;

				value = static_cast<char32_t>(cp);
				return 2;
			}

			// 3-byte
			if ((b0 >> 4) == 0xE)
			{
				if (bufferSize < 3)
					return 0; // need two continuation bytes

				uint32 b1 = static_cast<uint32>(buffer[1]);
				uint32 b2 = static_cast<uint32>(buffer[2]);

				if ((b1 & 0xC0) != 0x80 || (b2 & 0xC0) != 0x80)
				{
					// consume one invalid continuation to progress
					return 0;
				}

				uint32 cp = ((b0 & 0x0F) << 12) | ((b1 & 0x3F) << 6) | (b2 & 0x3F);

				// reject overlong and surrogates (surrogates are invalid in UTF-8)
				if (cp < 0x800 || (cp >= 0xD800 && cp <= 0xDFFF))
					return 0;

				value = static_cast<char32_t>(cp);
				return 3;
			}

			// 4-byte
			if ((b0 >> 3) == 0x1E)
			{
				if (bufferSize < 4)
					return 0; // need three continuation bytes

				uint32 b1 = static_cast<uint32>(buffer[1]);
				uint32 b2 = static_cast<uint32>(buffer[2]);
				uint32 b3 = static_cast<uint32>(buffer[3]);

				if ((b1 & 0xC0) != 0x80 || (b2 & 0xC0) != 0x80 || (b3 & 0xC0) != 0x80)
				{
					return 0;
				}

				uint32 cp = ((b0 & 0x07) << 18) | ((b1 & 0x3F) << 12) | ((b2 & 0x3F) << 6) | (b3 & 0x3F);

				// valid range: U+10000 .. U+10FFFF
				if (cp < 0x10000 || cp > 0x10FFFF)
					return 0;

				value = static_cast<char32_t>(cp);
				return 4;
			}

			// otherwise invalid leading byte
			return 0;
		}
		else if constexpr (SameAs<Char, char16_t>)
		{
			if (buffer == nullptr || bufferSize == 0)
				return 0;

			char16_t first = buffer[0];

			// Check if it's a surrogate pair
			if (first >= 0xD800 && first <= 0xDBFF) // high surrogate
			{
				if (bufferSize < 2)
				{
					value = U'\0'; // incomplete
					return 0;
				}

				char16_t second = buffer[1];
				if (second >= 0xDC00 && second <= 0xDFFF) // valid low surrogate
				{
					char32_t high = first - 0xD800;
					char32_t low = second - 0xDC00;
					value = (high << 10) + low + 0x10000;
					return 2; // consumed 2 code units
				}
				else
				{
					value = U'\0'; // invalid low surrogate
					return 0;
				}
			}
			else if (first >= 0xDC00 && first <= 0xDFFF) // stray low surrogate
			{
				value = U'\0';
				return 0;
			}
			else
			{
				// Single BMP character
				value = static_cast<char32_t>(first);
				return 1; // consumed 1 code unit
			}
		}
		else
		{
			if (buffer == nullptr || bufferSize == 0)
				return 0;

			value = *buffer;
		}

		return 1;
	}
	constexpr uint64 UnicodeChar::Hash() const
	{
		uint64 hash = 1469598103934665603ULL;
		for (int i = 0; i < 4; ++i)
		{
			uint8 byte = (value >> (i * 8)) & 0xFF;
			hash ^= byte;
			hash *= 1099511628211ULL;
		}

		return hash;
	}
	constexpr UnicodeChar& UnicodeChar::operator++()
	{
		++value;
		return *this;
	}
	constexpr UnicodeChar UnicodeChar::operator++(int)
	{
		UnicodeChar copy = *this;
		++value;
		return *this;
	}
	constexpr UnicodeChar& UnicodeChar::operator--()
	{
		--value;
		return *this;
	}
	constexpr UnicodeChar UnicodeChar::operator--(int)
	{
		UnicodeChar copy = *this;
		--value;
		return *this;
	}
	constexpr bool UnicodeChar::operator==(const UnicodeChar& other) const
	{
		return value == other.value;
	}
	constexpr bool UnicodeChar::operator!=(const UnicodeChar& other) const
	{
		return value != other.value;
	}
	constexpr UnicodeChar& UnicodeChar::operator=(const UnicodeChar& other)
	{
		value = other.value;
		return *this;
	}
}