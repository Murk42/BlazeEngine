#pragma once
#include "BlazeEngineCore/DataStructures/UnicodeChar.h"

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
	constexpr UnicodeChar::UnicodeChar(uint32 value)
		: value(value)
	{
	}
	constexpr UnicodeChar::UnicodeChar(const void* buffer, uintMem size)
	{
		if (buffer == nullptr || size == 0)
			value = 0;
		else
			UTF8ToUnicode(buffer, size, value);
	}
	constexpr uintMem UnicodeChar::UTF8Size() const
	{
		uint32 t = value;
		if ((value >> 7) == 0)
			return 1;
		else if ((value >> 12) == 0)
			return 2;
		else if ((value >> 18) == 0)
			return 3;
		else if ((value >> 21) == 0)
			return 4;
		return -1;
	}
	constexpr uintMem UnicodeChar::ToUTF8(void* buffer, uintMem size) const
	{
		if ((value >> 6) == 0)
		{
			if (size < 1)
				return 0;
			((char*)buffer)[0] = (char)value;
			return 1;
		}
		else if ((value >> 12) == 0)
		{
			if (size < 2)
				return 0;

			((char*)buffer)[1] = (char)(value >> 0) & 0b00111111 | 0b11000000;
			((char*)buffer)[0] = (char)(value >> 6) & 0b00011111 | 0b11000000;
			return 2;
		}
		else if ((value >> 18) == 0)
		{
			if (size < 3)
				return 0;

			((char*)buffer)[2] = (char)(value >> 0) & 0b00111111 | 0b11000000;
			((char*)buffer)[1] = (char)(value >> 6) & 0b00111111 | 0b11000000;
			((char*)buffer)[0] = (char)(value >> 12) & 0b00001111 | 0b11100000;
			return 3;
		}
		else if ((value >> 21) == 0)
		{
			if (size < 4)
				return 0;

			((char*)buffer)[3] = (char)(value >> 0) & 0b00111111 | 0b11000000;
			((char*)buffer)[2] = (char)(value >> 6) & 0b00111111 | 0b11000000;
			((char*)buffer)[1] = (char)(value >> 12) & 0b00111111 | 0b11000000;
			((char*)buffer)[0] = (char)(value >> 18) & 0b00000111 | 0b11110000;

			return 4;
		}

		return 0;
	}
	constexpr uint32 UnicodeChar::Hash() const
	{
		return value;
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
	constexpr uintMem UnicodeChar::UTF8ToUnicode(const void* ptr, uintMem size, uint32& character)
	{		
		if (size > 0 && (((char*)ptr)[0] & 0b10000000) == 0)
		{
			character = uint32(((char*)ptr)[0]);
			return 1;
		}
		else if (size > 1 && (((char*)ptr)[0] & 0b11100000) == 0b11000000)
		{
			character = uint32(((char*)ptr)[0] & 0b00011111) << 6;
			character |= uint32(((char*)ptr)[1] & 0b00111111);
			return 2;
		}
		else if (size > 2 && (((char*)ptr)[0] & 0b11110000) == 0b11100000)
		{
			character = uint32(((char*)ptr)[0] & 0b00001111) << 12;
			character |= uint32(((char*)ptr)[1] & 0b00111111) << 6;
			character |= uint32(((char*)ptr)[2] & 0b00111111);
			return 3;
		}
		else if (size > 3 && (((char*)ptr)[0] & 0b11111000) == 0b11110000)
		{
			character = uint32(((char*)ptr)[0] & 0b00000111) << 18;
			character |= uint32(((char*)ptr)[1] & 0b00111111) << 12;
			character |= uint32(((char*)ptr)[2] & 0b00111111) << 6;
			character |= uint32(((char*)ptr)[3] & 0b00111111);
			return 4;
		}

		return 0;
	}
}