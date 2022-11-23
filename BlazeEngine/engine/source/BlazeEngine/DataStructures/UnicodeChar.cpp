#include "BlazeEngine/DataStructures/UnicodeChar.h"
#include "BlazeEngine/DataStructures/StringViewUTF8.h"

namespace Blaze
{
	size_t UTF8Size(const char* ptr)
	{
		while ((*ptr & 0b11000000) == 0b10000000)
			--ptr;

		if ((*ptr & 0b10000000) == 0)
			return 1;
		else if ((*ptr & 0b11100000) == 0b11000000)
			return 2;
		else if ((*ptr & 0b11110000) == 0b11100000)
			return 3;
		else if ((*ptr & 0b11111000) == 0b11110000)
			return 4;
	}

	size_t UTF8ToUnicode(const char* ptr, uint32& character)
	{
		character = 0;
		if ((ptr[0] & 0b10000000) == 0)
		{
			character = uint32(ptr[0]);
			return 1;
		}
		else if ((ptr[0] & 0b11100000) == 0b11000000)
		{
			character = uint32(ptr[0] & 0b00011111) << 6;
			character |= uint32(ptr[1] & 0b00111111);
			return 2;
		}
		else if ((ptr[0] & 0b11110000) == 0b11100000)
		{
			character = uint32(ptr[0] & 0b00001111) << 12;
			character |= uint32(ptr[1] & 0b00111111) << 6;
			character |= uint32(ptr[2] & 0b00111111);
			return 3;
		}
		else if ((ptr[0] & 0b11111000) == 0b11110000)
		{
			character = uint32(ptr[0] & 0b00000111) << 18;
			character |= uint32(ptr[1] & 0b00111111) << 12;
			character |= uint32(ptr[2] & 0b00111111) << 6;
			character |= uint32(ptr[3] & 0b00111111);
			return 4;
		}
		return 0;
	}

	UnicodeChar::UnicodeChar(const UnicodeChar& ch)
		: value(ch.value)
	{
	}

	UnicodeChar::UnicodeChar(char ch)
		: value(ch)
	{
	}
	UnicodeChar::UnicodeChar(const char* ptr)
	{
		if (ptr == nullptr)
			value = 0;
		else
			UTF8ToUnicode(ptr, value);
	}	

	size_t UnicodeChar::UTF8Size()
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

	void UnicodeChar::ToUTF8(char* buffer)
	{
		if ((value >> 6) == 0)
		{
			buffer[0] = (char)value;
		}
		else if ((value >> 12) == 0)
		{
			buffer[1] = (char)(value >> 0) & 0b00111111 | 0b11000000;
			buffer[0] = (char)(value >> 6) & 0b00011111 | 0b11000000;
		}
		else if ((value >> 18) == 0)
		{
			buffer[2] = (char)(value >> 0) & 0b00111111 | 0b11000000;
			buffer[1] = (char)(value >> 6) & 0b00111111 | 0b11000000;
			buffer[0] = (char)(value >> 12) & 0b00001111 | 0b11100000;
		}
		else if ((value >> 21) == 0)
		{
			buffer[3] = (char)(value >> 0) & 0b00111111 | 0b11000000;
			buffer[2] = (char)(value >> 6) & 0b00111111 | 0b11000000;
			buffer[1] = (char)(value >> 12) & 0b00111111 | 0b11000000;
			buffer[0] = (char)(value >> 18) & 0b00000111 | 0b11110000;
		}
	}

}