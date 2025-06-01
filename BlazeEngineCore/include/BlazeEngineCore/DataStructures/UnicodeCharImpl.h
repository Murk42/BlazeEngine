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
	constexpr UnicodeChar::UnicodeChar(const void* buffer, uintMem bufferSize)
	{
		if (buffer == nullptr || bufferSize == 0)
			value = 0;
		else
			FromUTF8(buffer, bufferSize);
	}
	constexpr uintMem UnicodeChar::UTF8Size() const
	{		
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
	constexpr uintMem UnicodeChar::ToUTF8(void* buffer, uintMem bufferSize) const
	{
		if (bufferSize == 0)
			return 0;

		if ((value >> 6) == 0)
		{
			((char*)buffer)[0] = (char)value;
			return 1;
		}
		else if (bufferSize == 1)
			return 0;

		if ((value >> 12) == 0)
		{			
			((char*)buffer)[1] = (char)(value >> 0) & 0b00111111 | 0b11000000;
			((char*)buffer)[0] = (char)(value >> 6) & 0b00011111 | 0b11000000;
			return 2;
		}
		else if (bufferSize == 2)
			return 0;

		if ((value >> 18) == 0)
		{			
			((char*)buffer)[2] = (char)(value >> 0) & 0b00111111 | 0b11000000;
			((char*)buffer)[1] = (char)(value >> 6) & 0b00111111 | 0b11000000;
			((char*)buffer)[0] = (char)(value >> 12) & 0b00001111 | 0b11100000;
			return 3;
		}
		else if (bufferSize == 3)
			return 0;

		if ((value >> 21) == 0)
		{
			if (bufferSize < 4)
				return 0;

			((char*)buffer)[3] = (char)(value >> 0) & 0b00111111 | 0b11000000;
			((char*)buffer)[2] = (char)(value >> 6) & 0b00111111 | 0b11000000;
			((char*)buffer)[1] = (char)(value >> 12) & 0b00111111 | 0b11000000;
			((char*)buffer)[0] = (char)(value >> 18) & 0b00000111 | 0b11110000;

			return 4;
		}		

		return 0;
	}
	constexpr uintMem UnicodeChar::FromUTF8(const void* buffer, uintMem bufferSize)
	{
		if (bufferSize > 0 && (((char*)buffer)[0] & 0b10000000) == 0)
		{
			value = uint32(((char*)buffer)[0]);
			return 1;
		}
		else if (bufferSize > 1 && (((char*)buffer)[0] & 0b11100000) == 0b11000000)
		{
			value = uint32(((char*)buffer)[0] & 0b00011111) << 6;
			value |= uint32(((char*)buffer)[1] & 0b00111111);
			return 2;
		}
		else if (bufferSize > 2 && (((char*)buffer)[0] & 0b11110000) == 0b11100000)
		{
			value = uint32(((char*)buffer)[0] & 0b00001111) << 12;
			value |= uint32(((char*)buffer)[1] & 0b00111111) << 6;
			value |= uint32(((char*)buffer)[2] & 0b00111111);
			return 3;
		}
		else if (bufferSize > 3 && (((char*)buffer)[0] & 0b11111000) == 0b11110000)
		{
			value = uint32(((char*)buffer)[0] & 0b00000111) << 18;
			value |= uint32(((char*)buffer)[1] & 0b00111111) << 12;
			value |= uint32(((char*)buffer)[2] & 0b00111111) << 6;
			value |= uint32(((char*)buffer)[3] & 0b00111111);
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
}