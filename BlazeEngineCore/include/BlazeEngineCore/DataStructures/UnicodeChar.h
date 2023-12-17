#pragma once

namespace Blaze
{
	class StringViewUTF8;
	class BLAZE_CORE_API UnicodeChar
	{
		uint32 value;
	public:
		UnicodeChar();
		UnicodeChar(const UnicodeChar&);
		UnicodeChar(uint32 value);		
		UnicodeChar(const void* ptr, uintMem size);

		inline uint32 Value() const { return value; }
		size_t UTF8Size();
		bool ToUTF8(void* buffer, uintMem size);

		uint32 Hash() const;

		UnicodeChar& operator++();
		UnicodeChar operator++(int);
		UnicodeChar& operator--();
		UnicodeChar operator--(int);

		bool operator==(const UnicodeChar& other) const;
		bool operator!=(const UnicodeChar& other) const;

		UnicodeChar& operator=(const UnicodeChar& other);
	};
}