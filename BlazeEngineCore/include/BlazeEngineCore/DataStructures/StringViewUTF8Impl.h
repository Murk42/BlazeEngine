#pragma once
#include "BlazeEngineCore/DataStructures/StringViewUTF8.h"
#include "BlazeEngineCore/DataStructures/StringUTF8.h"
#include "BlazeEngineCore/DataStructures/StringView.h"
#include "BlazeEngineCore/DataStructures/String.h"

namespace Blaze
{
	constexpr StringViewUTF8Iterator::StringViewUTF8Iterator()
		: ptr(nullptr)
	{
	}
	constexpr StringViewUTF8Iterator::StringViewUTF8Iterator(const StringViewUTF8Iterator& i)
		: ptr(i.ptr)
	{
	}
	constexpr StringViewUTF8Iterator::StringViewUTF8Iterator(const void* buffer)
		: ptr(buffer)
	{	
	}
	constexpr bool StringViewUTF8Iterator::IsNull() const
	{
		return ptr == nullptr;
	}
	constexpr UnicodeChar StringViewUTF8Iterator::ToUnicode() const
	{
#ifdef BLAZE_NULL_ITERATOR_CHECK
		if (ptr == nullptr)
		{
			BLAZE_ENGINE_CORE_FATAL("Accessing a null iterator");
			return UnicodeChar();
		}
#endif
		return UnicodeChar(ptr, DetermineSize());
	}
	constexpr const void* StringViewUTF8Iterator::Ptr() const
	{
		return ptr;
	}
	constexpr uintMem StringViewUTF8Iterator::DetermineSize() const
	{
		if ((((char*)ptr)[0] & 0b10000000) == 0)
			return 1;		
		if ((((char*)ptr)[0] & 0b11100000) == 0b11000000)
			return 2;		
		if ((((char*)ptr)[0] & 0b11110000) == 0b11100000)
			return 3;		
		if ((((char*)ptr)[0] & 0b11111000) == 0b11110000)
			return 4;

		BLAZE_ENGINE_CORE_FATAL("Invalid UTF-8 character sequence");

		return 0;
	}	
	constexpr UnicodeChar StringViewUTF8Iterator::operator*() const
	{
#ifdef BLAZE_NULL_ITERATOR_CHECK
		if (ptr == nullptr)
		{
			BLAZE_ENGINE_CORE_FATAL("Accessing a null iterator");
			return UnicodeChar();
		}
#endif
		return ToUnicode();
	}
	constexpr StringViewUTF8Iterator& StringViewUTF8Iterator::operator++()
	{
#ifdef BLAZE_NULL_ITERATOR_CHECK
		if (ptr == nullptr)
		{
			BLAZE_ENGINE_CORE_FATAL("Incrementing a null iterator");			
			return *this;
		}
#endif		
		ptr = (byte*)ptr + DetermineSize();
		return *this;
	}
	constexpr StringViewUTF8Iterator StringViewUTF8Iterator::operator++(int)
	{
#ifdef BLAZE_NULL_ITERATOR_CHECK
		if (ptr == nullptr)
		{
			BLAZE_ENGINE_CORE_FATAL("Incrementing a null iterator");
			return StringViewUTF8Iterator();			
		}
#endif
		StringViewUTF8Iterator old = *this;
		ptr = (byte*)ptr + DetermineSize();
		return old;		
	}
	constexpr StringViewUTF8Iterator& StringViewUTF8Iterator::Decrement(const StringViewUTF8& string)
	{
		return Decrement(string.FirstIterator());
	}
	inline constexpr StringViewUTF8Iterator& StringViewUTF8Iterator::Decrement(const StringViewUTF8Iterator& firstCharacterIterator)
	{
#ifdef BLAZE_NULL_ITERATOR_CHECK
		if (ptr == nullptr)
		{
			BLAZE_ENGINE_CORE_FATAL("Decrementing a null iterator");
			return *this;
		}
#endif

		if (ptr == firstCharacterIterator.Ptr())
			ptr = (byte*)ptr - 1;
		else if ((*((byte*)ptr - 1) & 0b10000000) == 0)
			ptr = (byte*)ptr - 1;
		else if ((*((byte*)ptr - 2) & 0b10000000) == 0)
			ptr = (byte*)ptr - 2;
		else if ((*((byte*)ptr - 3) & 0b10000000) == 0)
			ptr = (byte*)ptr - 3;
		else if ((*((byte*)ptr - 4) & 0b10000000) == 0)
			ptr = (byte*)ptr - 4;
		else
			BLAZE_ENGINE_CORE_FATAL("Invalid UTF-8 character sequence");

		return *this;
	}
	constexpr bool StringViewUTF8Iterator::operator==(const StringViewUTF8Iterator& i) const
	{
		return ptr == i.ptr;
	}
	constexpr bool StringViewUTF8Iterator::operator!=(const StringViewUTF8Iterator& i) const
	{
		return ptr != i.ptr;
	}
	constexpr bool StringViewUTF8Iterator::operator<(const StringViewUTF8Iterator& i) const
	{
#ifdef BLAZE_NULL_ITERATOR_CHECK
		if ((ptr == nullptr) != (i.ptr == nullptr))
		{
			BLAZE_ENGINE_CORE_FATAL("Comparing a null iterator");
			return false;
		}
#endif

		return ptr < i.ptr;
	}
	constexpr bool StringViewUTF8Iterator::operator>(const StringViewUTF8Iterator& i) const
	{
#ifdef BLAZE_NULL_ITERATOR_CHECK
		if ((ptr == nullptr) != (i.ptr == nullptr))
		{
			BLAZE_ENGINE_CORE_FATAL("Comparing a null iterator");
			return false;
		}
#endif

		return ptr > i.ptr;
	}
	constexpr bool StringViewUTF8Iterator::operator<=(const StringViewUTF8Iterator& i) const
	{
#ifdef BLAZE_NULL_ITERATOR_CHECK
		if ((ptr == nullptr) != (i.ptr == nullptr))
		{
			BLAZE_ENGINE_CORE_FATAL("Comparing a null iterator");
			return false;
		}
#endif

		return ptr <= i.ptr;
	}
	constexpr bool StringViewUTF8Iterator::operator>=(const StringViewUTF8Iterator& i) const
	{
#ifdef BLAZE_NULL_ITERATOR_CHECK
		if ((ptr == nullptr) != (i.ptr == nullptr))
		{
			BLAZE_ENGINE_CORE_FATAL("Comparing a null iterator");
			return false;
		}
#endif

		return ptr >= i.ptr;
	}
	constexpr StringViewUTF8Iterator& StringViewUTF8Iterator::operator=(const StringViewUTF8Iterator& i)
	{
		ptr = i.ptr;
		return *this;
	}

	constexpr StringViewUTF8::StringViewUTF8()
		: buffer(nullptr), bufferSize(0), characterCount(0)
	{
	}
	constexpr StringViewUTF8::StringViewUTF8(const StringViewUTF8& other)
		: buffer(other.buffer), bufferSize(other.bufferSize), characterCount(other.characterCount)
	{
	}
	inline StringViewUTF8::StringViewUTF8(const StringUTF8& other)
		: buffer(other.Buffer()), bufferSize(other.BufferSize()), characterCount(other.CharacterCount())
	{
	}
	constexpr StringViewUTF8::StringViewUTF8(const StringView& other)
		: buffer(nullptr), bufferSize(0), characterCount(0)
	{
		if (!other.Empty())
		{
			buffer = other.Ptr();
			bufferSize = other.Count();

			CalculateCharacterCount();
		}
	}
	inline StringViewUTF8::StringViewUTF8(const String& other)
		: StringViewUTF8(StringView(other))
	{
	}	
	constexpr StringViewUTF8::StringViewUTF8(const void* ptr, uintMem size)
		: StringViewUTF8(StringView((const char*)ptr, size))
	{		
	}
	constexpr StringViewUTF8::StringViewUTF8(StringViewUTF8Iterator begin, StringViewUTF8Iterator end)
		: buffer(nullptr), bufferSize(0), characterCount(0)
	{
		if (begin != end && begin.Ptr() != nullptr && end.Ptr() != nullptr)
		{
			bufferSize = (byte*)end.Ptr() - (byte*)begin.Ptr();
			if (*end.Decrement(begin) == '\0')
				--bufferSize;

			if (bufferSize == 0)
				return;

			buffer = begin.Ptr();


			CalculateCharacterCount();
		}
	}
	template<uintMem C>
	constexpr StringViewUTF8::StringViewUTF8(const char(&arr)[C])
		: buffer(nullptr), bufferSize(0), characterCount(0)
	{
		if constexpr (C != 0)
		{
			if (arr[C - 1] == '\0')
				if (C == 1)
					return;
				else
					bufferSize = C - 1;				
			else			
				bufferSize = C;			

			buffer = arr;

			CalculateCharacterCount();
		}		
	}
	constexpr void StringViewUTF8::Clear()
	{
		buffer = nullptr;
		bufferSize = 0;
		characterCount = 0;
	}
	constexpr StringViewUTF8 StringViewUTF8::SubString(uintMem start, intMem count) const
	{
		if (count == 0)
			return StringViewUTF8();

		const uintMem characterCount = CharacterCount();
		if (start > characterCount || start + count > characterCount)
		{
			BLAZE_ENGINE_CORE_FATAL("Accesing a string outside its bounds");
			return StringViewUTF8();
		}

		if (count < 0)
			count = CharacterCount() + count;

		auto begin = FirstIterator();
		for (uintMem i = 0; i != start; ++i, ++begin);
		auto end = begin;  
		for (uintMem i = 0; i != count; ++i, ++end);

		return StringViewUTF8(begin, end);
	}
	constexpr StringViewUTF8 StringViewUTF8::SubString(StringViewUTF8Iterator begin, intMem count) const
	{
		auto end = begin;
		for (uintMem i = 0; i != count; ++i, ++end);

		return StringViewUTF8(begin, end);
	}
	constexpr bool StringViewUTF8::Empty() const { return bufferSize == 0; }
	inline uint32 StringViewUTF8::Hash() const
	{
		return static_cast<uint32>(std::hash<StringViewUTF8>()(*this));
	}
	constexpr const void* StringViewUTF8::Buffer() const { return buffer; }
	constexpr uintMem StringViewUTF8::BufferSize() const { return bufferSize; }
	constexpr uintMem StringViewUTF8::CharacterCount() const { return characterCount; }
	constexpr UnicodeChar StringViewUTF8::First() const
	{
#ifdef BLAZE_INVALID_ITERATOR_CHECK
		if (bufferSize == 0)
		{
			BLAZE_ENGINE_CORE_FATAL("Accessing a empty string");
			return UnicodeChar();
		}
#endif
		return UnicodeChar(buffer, bufferSize);
	}
	constexpr UnicodeChar StringViewUTF8::Last() const
	{
#ifdef BLAZE_INVALID_ITERATOR_CHECK
		if (bufferSize == 0)
		{
			BLAZE_ENGINE_CORE_FATAL("Accessing a empty string");
			return UnicodeChar();
		}
#endif

		return StringViewUTF8Iterator((byte*)buffer + bufferSize).Decrement(*this).ToUnicode();
	}
	constexpr StringViewUTF8Iterator StringViewUTF8::FirstIterator() const
	{		
		if (buffer == nullptr)
			return StringViewUTF8Iterator();
		return StringViewUTF8Iterator(buffer);
	}
	constexpr StringViewUTF8Iterator StringViewUTF8::LastIterator() const
	{
		if (buffer == nullptr)
			return StringViewUTF8Iterator();
		return StringViewUTF8Iterator((byte*)buffer + bufferSize).Decrement(*this);
	}
	constexpr StringViewUTF8Iterator StringViewUTF8::AheadIterator() const
	{
		if (buffer == nullptr)
			return StringViewUTF8Iterator();
		return StringViewUTF8Iterator((byte*)buffer).Decrement(*this);
	}
	constexpr StringViewUTF8Iterator StringViewUTF8::BehindIterator() const
	{		
		if (buffer == nullptr)
			return StringViewUTF8Iterator();
		return StringViewUTF8Iterator((byte*)buffer + bufferSize);
	}
	constexpr bool StringViewUTF8::operator==(const StringViewUTF8& s) const
	{
		if (buffer == nullptr && s.Buffer() == nullptr)
			return true;
		if (buffer == nullptr || s.Buffer() == nullptr || bufferSize != s.BufferSize())
			return false;		

		return memcmp(buffer, s.buffer, bufferSize) == 0;
	}
	constexpr bool StringViewUTF8::operator!=(const StringViewUTF8& s) const
	{
		return !(*this == s);
	}
	constexpr StringViewUTF8& StringViewUTF8::operator=(const StringViewUTF8& other)
	{
		buffer = other.buffer;
		bufferSize = other.bufferSize;
		characterCount = other.characterCount;
		return *this;
	}
	inline StringViewUTF8& StringViewUTF8::operator=(const StringUTF8& other)
	{
		buffer = other.Buffer();
		bufferSize = other.BufferSize();
		characterCount = other.CharacterCount();
		return *this;
	}
	constexpr StringViewUTF8& StringViewUTF8::operator=(const StringView& other)
	{
		buffer = other.Ptr();
		bufferSize = characterCount;
		CalculateCharacterCount();
		return *this;
	}
	inline StringViewUTF8& StringViewUTF8::operator=(const String& other)
	{
		buffer = other.Ptr();
		bufferSize = characterCount;
		CalculateCharacterCount();
		return *this;
	}
	constexpr void StringViewUTF8::CalculateCharacterCount()
	{
		for (auto it = FirstIterator(); it != BehindIterator(); ++it, ++characterCount);
	}
	template<size_t C>
	constexpr StringViewUTF8& StringViewUTF8::operator=(const char(&arr)[C])
	{
		if constexpr (C != 0)
		{ 
			buffer = arr;
			bufferSize = C;
			CalculateCharacterCount();			
		}
		return *this;
	}

	constexpr StringViewUTF8Iterator begin(const StringViewUTF8& string)
	{
		return string.FirstIterator();
	}
	constexpr StringViewUTF8Iterator end(const StringViewUTF8& string)
	{
		return string.BehindIterator();
	}
}