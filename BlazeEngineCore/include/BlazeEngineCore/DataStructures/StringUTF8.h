#pragma once
#include "BlazeEngineCore/DataStructures/UnicodeChar.h"

namespace Blaze
{
	class StringViewUTF8;
	class StringView;
	class String;
	class StringViewUTF8Iterator;

	class BLAZE_CORE_API StringUTF8
	{
	public:
		StringUTF8();
		StringUTF8(const StringUTF8& s);
		StringUTF8(StringUTF8&& s) noexcept;
		StringUTF8(const StringViewUTF8&);
		StringUTF8(const String&);
		StringUTF8(const StringView&);
		//The buffer size doesn't inlcude the null-terminating character
		StringUTF8(const void* buffer, uintMem size);
		//The count doesn't include the null-terminating character
		StringUTF8(const char* ptr, uintMem count);
		template<uintMem C>
		StringUTF8(const char(&arr)[C]);
		StringUTF8(StringViewUTF8Iterator begin, StringViewUTF8Iterator end);
		~StringUTF8();

		void Clear();
		StringUTF8 SubString(uintMem start, uintMem count) const;
		StringUTF8& Resize(uintMem newCharacterCount, UnicodeChar fill);
		//
		// Inserts a string at the givent index into the target string
		//
		// \param 
		// <index>:
		// Index of the character before which the string will be inserted.
		// If the index is equal to the number of characters then the string
		// will be appended to the target string. If the index is negative then
		// the string will be inserted at index [CharacterCount() + index]
		//
		void InsertString(intMem index, StringViewUTF8 string);
		//
		// Erases a substring of the target string.
		// 
		// \param 
		// <index>:
		// Index of the first character of the substring to be erased.
		// If it is negative, [count] number of characters AFTER the
		// character with index [CharacterCount() + index] are erased.
		// 
		// If the substring is outside the bounds of the target string, only the
		// characters that are in bounds will be erased
		//
		void EraseSubString(intMem index, uintMem count);

		inline bool Empty() const { return bufferSize == 0; }
		uint32 Hash() const;
		inline const void* Buffer() const { return buffer; }
		inline uintMem BufferSize() const { return bufferSize; }
		inline uintMem CharacterCount() const { return characterCount; }

		UnicodeChar First() const;
		StringViewUTF8Iterator FirstIterator() const;
		StringViewUTF8Iterator BehindIterator() const;

		bool operator==(const StringViewUTF8& s) const;
		bool operator!=(const StringViewUTF8& s) const;

		StringUTF8& operator+=(const StringViewUTF8&);
		StringUTF8& operator+=(const String&);
		StringUTF8& operator+=(const StringView&);
		StringUTF8& operator+=(const char&);
		StringUTF8& operator+=(const UnicodeChar&);
		template<size_t C>
		StringUTF8& operator+=(const char(&arr)[C]);

		StringUTF8& operator=(const StringUTF8& other);
		StringUTF8& operator=(StringUTF8&& other) noexcept;
		StringUTF8& operator=(const StringViewUTF8& other);
		StringUTF8& operator=(const String& other);
		StringUTF8& operator=(const StringView& other);
		template<size_t C>
		StringUTF8& operator=(const char(&arr)[C]);

		friend BLAZE_CORE_API StringUTF8 operator+(const StringViewUTF8& left, const StringViewUTF8& right);
	private:
		void* buffer;
		uintMem bufferSize;
		uintMem characterCount;

		static void* Advance(void* it);
	};

	BLAZE_CORE_API StringViewUTF8Iterator begin(const StringUTF8& string);
	BLAZE_CORE_API StringViewUTF8Iterator end(const StringUTF8& string);

	BLAZE_CORE_API StringUTF8 operator+(const StringViewUTF8& left, const StringViewUTF8& right);
	BLAZE_CORE_API StringUTF8 operator+(const StringViewUTF8& left, const StringUTF8& right);
	BLAZE_CORE_API StringUTF8 operator+(const StringViewUTF8& left, const StringView& right);
	BLAZE_CORE_API StringUTF8 operator+(const StringViewUTF8& left, const String& right);
	BLAZE_CORE_API StringUTF8 operator+(const StringViewUTF8& left, const char& right);
	BLAZE_CORE_API StringUTF8 operator+(const StringViewUTF8& left, const UnicodeChar& right);
	template<uintMem C> StringUTF8 operator+(const StringViewUTF8& left, const char(&right)[C])
	{
		return left + StringViewUTF8(right, C - 1);
	}
	BLAZE_CORE_API StringUTF8 operator+(const StringUTF8& left, const StringViewUTF8& right);
	BLAZE_CORE_API StringUTF8 operator+(const StringView& left, const StringViewUTF8& right);
	BLAZE_CORE_API StringUTF8 operator+(const String& left, const StringViewUTF8& right);
	BLAZE_CORE_API StringUTF8 operator+(const char& left, const StringViewUTF8& right);
	BLAZE_CORE_API StringUTF8 operator+(const UnicodeChar& left, const StringViewUTF8& right);
	template<uintMem C> StringUTF8 operator+(const char(&left)[C], const StringViewUTF8& right)
	{
		return StringViewUTF8(left, C - 1) + right;
	}
	
	BLAZE_CORE_API StringUTF8 operator+(const StringUTF8& left, const StringUTF8& right);
	BLAZE_CORE_API StringUTF8 operator+(const StringUTF8& left, const StringView& right);
	BLAZE_CORE_API StringUTF8 operator+(const StringUTF8& left, const String& right);
	BLAZE_CORE_API StringUTF8 operator+(const StringUTF8& left, const char& right);
	BLAZE_CORE_API StringUTF8 operator+(const StringUTF8& left, const UnicodeChar& right);
	template<uintMem C> StringUTF8 operator+(const StringUTF8& left, const char(&right)[C])
	{
		return StringViewUTF8(left) + StringViewUTF8(right, C - 1);
	}
	BLAZE_CORE_API StringUTF8 operator+(const StringView& left, const StringUTF8& right);
	BLAZE_CORE_API StringUTF8 operator+(const String& left, const StringUTF8& right);
	BLAZE_CORE_API StringUTF8 operator+(const char& left, const StringUTF8& right);
	BLAZE_CORE_API StringUTF8 operator+(const UnicodeChar& left, const StringUTF8& right);
	template<uintMem C> StringUTF8 operator+(const char(&left)[C], const StringUTF8& right)
	{
		return StringViewUTF8(left, C - 1) + StringViewUTF8(right);
	}
	template<uintMem C>
	inline StringUTF8::StringUTF8(const char(&arr)[C])
		: StringUTF8(arr, C - 1)
	{
	}
	template<size_t C>
	inline StringUTF8& StringUTF8::operator+=(const char(&arr)[C])
	{
		return *this = StringViewUTF8(*this) + StringViewUTF8(arr, C - 1);
	}
	template<size_t C>
	inline StringUTF8& StringUTF8::operator=(const char(&arr)[C])
	{
		return operator=(StringViewUTF8(arr));
	}
}