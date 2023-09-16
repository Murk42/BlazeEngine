#include "BlazeEngine/DataStructures/UnicodeChar.h"
#include "BlazeEngine/DataStructures/StringViewUTF8.h"

namespace Blaze
{

	size_t UTF8ToUnicode(const void* ptr, uint32& character)
	{
		character = 0;
		if ((((char*)ptr)[0] & 0b10000000) == 0)
		{
			character = uint32(((char*)ptr)[0]);
			return 1;
		}
		else if ((((char*)ptr)[0] & 0b11100000) == 0b11000000)
		{
			character = uint32(((char*)ptr)[0] & 0b00011111) << 6;
			character |= uint32(((char*)ptr)[1] & 0b00111111);
			return 2;
		}
		else if ((((char*)ptr)[0] & 0b11110000) == 0b11100000)
		{
			character = uint32(((char*)ptr)[0] & 0b00001111) << 12;
			character |= uint32(((char*)ptr)[1] & 0b00111111) << 6;
			character |= uint32(((char*)ptr)[2] & 0b00111111);
			return 3;
		}
		else if ((((char*)ptr)[0] & 0b11111000) == 0b11110000)
		{
			character = uint32(((char*)ptr)[0] & 0b00000111) << 18;
			character |= uint32(((char*)ptr)[1] & 0b00111111) << 12;
			character |= uint32(((char*)ptr)[2] & 0b00111111) << 6;
			character |= uint32(((char*)ptr)[3] & 0b00111111);
			return 4;
		}
		return 0;
	}

	UnicodeChar::UnicodeChar()
		: value(0)
	{
	}

	UnicodeChar::UnicodeChar(const UnicodeChar& ch)
		: value(ch.value)
	{
	}

	UnicodeChar::UnicodeChar(uint value)
		: value(value)
	{
	}
	
	UnicodeChar::UnicodeChar(const void* ptr, uint size)
	{
		if (ptr == nullptr || size == 0)
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

	bool UnicodeChar::ToUTF8(void* buffer, uint size)
	{
		if ((value >> 6) == 0)
		{
			if (size < 1)
				return false;
			((char*)buffer)[0] = (char)value;
		}
		else if ((value >> 12) == 0)
		{
			if (size < 2)
				return false;

			((char*)buffer)[1] = (char)(value >> 0) & 0b00111111 | 0b11000000;
			((char*)buffer)[0] = (char)(value >> 6) & 0b00011111 | 0b11000000;
		}
		else if ((value >> 18) == 0)
		{
			if (size < 3)
				return false;

			((char*)buffer)[2] = (char)(value >> 0) & 0b00111111 | 0b11000000;
			((char*)buffer)[1] = (char)(value >> 6) & 0b00111111 | 0b11000000;
			((char*)buffer)[0] = (char)(value >> 12) & 0b00001111 | 0b11100000;
		}
		else if ((value >> 21) == 0)
		{
			if (size < 4)
				return false;

			((char*)buffer)[3] = (char)(value >> 0) & 0b00111111 | 0b11000000;
			((char*)buffer)[2] = (char)(value >> 6) & 0b00111111 | 0b11000000;
			((char*)buffer)[1] = (char)(value >> 12) & 0b00111111 | 0b11000000;
			((char*)buffer)[0] = (char)(value >> 18) & 0b00000111 | 0b11110000;
		}

		return true;
	}
	
	uint32 UnicodeChar::Hash() const
	{
		return value;
	}

	UnicodeChar& UnicodeChar::operator++()
	{
		++value;
		return *this;
	}
	UnicodeChar UnicodeChar::operator++(int)
	{
		UnicodeChar copy = *this;
		++value;
		return *this;
	}

	UnicodeChar& UnicodeChar::operator--()
	{
		--value;
		return *this;
	}

	UnicodeChar UnicodeChar::operator--(int)
	{
		UnicodeChar copy = *this;
		--value;
		return *this;
	}


	bool UnicodeChar::operator==(const UnicodeChar& other) const
	{
		return value == other.value;
	}
	bool UnicodeChar::operator!=(const UnicodeChar& other) const
	{
		return value != other.value;
	}

	UnicodeChar& UnicodeChar::operator=(const UnicodeChar& other)
	{
		value = other.value;
		return *this;
	}
}