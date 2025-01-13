#pragma once
#include "BlazeEngineCore/BlazeEngineCoreDefines.h"
#include "BlazeEngineCore/DataStructures/UnicodeChar.h"

namespace Blaze
{
	class StringUTF8;
	class StringView;	
	class String;

	class StringViewUTF8Iterator
	{
	public:
		constexpr StringViewUTF8Iterator();
		constexpr StringViewUTF8Iterator(const StringViewUTF8Iterator&);

		constexpr bool IsNull() const;
		constexpr UnicodeChar ToUnicode() const;
		constexpr const void* Ptr() const;

		constexpr UnicodeChar operator*() const;

		constexpr StringViewUTF8Iterator operator++();
		constexpr StringViewUTF8Iterator& operator++(int);
		constexpr StringViewUTF8Iterator operator--();
		constexpr StringViewUTF8Iterator& operator--(int);

		constexpr bool operator==(const StringViewUTF8Iterator&) const;
		constexpr bool operator!=(const StringViewUTF8Iterator&) const;
		constexpr bool operator<(const StringViewUTF8Iterator&) const;
		constexpr bool operator>(const StringViewUTF8Iterator&) const;
		constexpr bool operator<=(const StringViewUTF8Iterator&) const;
		constexpr bool operator>=(const StringViewUTF8Iterator&) const;

		constexpr StringViewUTF8Iterator& operator=(const StringViewUTF8Iterator&);
	private:
		const void* ptr;
		UnicodeChar value;

		constexpr StringViewUTF8Iterator(const void* ptr, uintMem size);

		friend class StringUTF8;
		friend class StringViewUTF8;
	};

	class StringViewUTF8
	{
	public:
		constexpr StringViewUTF8();
		constexpr StringViewUTF8(const StringViewUTF8& other);
		inline StringViewUTF8(const StringUTF8& other);
		constexpr StringViewUTF8(const StringView& other);		
		inline StringViewUTF8(const String& other);				
		//The buffer size doesn't include the null-terminating character,
		//but the passed buffer must be null-terminated
		inline StringViewUTF8(const void* ptr, uintMem size);
		//The count doesn't include the null-terminating character, but
		//the passed string must be null-terminated
		inline StringViewUTF8(const char* ptr, uintMem count);
		inline StringViewUTF8(StringViewUTF8Iterator begin, StringViewUTF8Iterator end);
		template<uintMem C>
		constexpr StringViewUTF8(const char(&arr)[C]);

		constexpr void Clear();
		StringUTF8 SubString(uintMem start, uintMem count) const;

		constexpr bool Empty() const;
		inline uint32 Hash() const;
		constexpr const void* Buffer() const;
		constexpr uintMem BufferSize() const;
		constexpr uintMem CharacterCount() const;

		constexpr UnicodeChar First() const;
		constexpr StringViewUTF8Iterator FirstIterator() const;				
		constexpr StringViewUTF8Iterator BehindIterator() const;		

		constexpr bool operator==(const StringViewUTF8& other) const;
		constexpr bool operator!=(const StringViewUTF8& other) const;

		constexpr StringViewUTF8& operator= (const StringViewUTF8& other);
		inline StringViewUTF8& operator= (const StringUTF8& other);
		constexpr StringViewUTF8& operator= (const StringView& other);
		inline StringViewUTF8& operator= (const String& other);
		template<size_t C>
		constexpr StringViewUTF8& operator=(const char(&arr)[C]);
	private:
		const void* buffer;		
		uintMem bufferSize;
		uintMem characterCount;
	};

	constexpr StringViewUTF8Iterator begin(const StringViewUTF8& string);
	constexpr StringViewUTF8Iterator end(const StringViewUTF8& string);
	template<uintMem C>
	constexpr StringViewUTF8::StringViewUTF8(const char(&arr)[C])
		: StringViewUTF8(arr, C - 1)
	{
	}
	template<size_t C>
	constexpr StringViewUTF8& StringViewUTF8::operator=(const char(&arr)[C])
	{
		buffer = arr;
		bufferSize = C;
		characterCount = C - 1;
		return *this;
	}
}
