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
		: ptr(i.ptr), value(i.value)
	{
	}
	constexpr StringViewUTF8Iterator::StringViewUTF8Iterator(const void* ptr, uintMem size)
		: ptr(ptr), value(ptr, size)
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
			BLAZE_ENGINE_CORE_FATAL("Decrementing a null iterator");
#endif
		return value;
	}
	constexpr const void* StringViewUTF8Iterator::Ptr() const
	{
		return ptr;
	}
	constexpr UnicodeChar StringViewUTF8Iterator::operator*() const
	{
#ifdef BLAZE_NULL_ITERATOR_CHECK
		if (ptr == nullptr)
			BLAZE_ENGINE_CORE_FATAL("Dereferencing a null iterator");
#endif
		return ToUnicode();
	}
	constexpr StringViewUTF8Iterator StringViewUTF8Iterator::operator++()
	{
#ifdef BLAZE_NULL_ITERATOR_CHECK
		if (ptr == nullptr)
			BLAZE_ENGINE_CORE_FATAL("Incrementing a null iterator");
#endif

		StringViewUTF8Iterator old = *this;
		if ((*(uint8*)ptr) >> 3 == 0b11110)
			ptr = (byte*)ptr + 4;
		else if ((*(uint8*)ptr) >> 4 == 0b1110)
			ptr = (byte*)ptr + 3;
		else if ((*(uint8*)ptr) >> 5 == 0b110)
			ptr = (byte*)ptr + 2;
		else
			ptr = (byte*)ptr + 1;
		value = UnicodeChar(ptr, 4);
		return old;
	}
	constexpr StringViewUTF8Iterator& StringViewUTF8Iterator::operator++(int)
	{
#ifdef BLAZE_NULL_ITERATOR_CHECK
		if (ptr == nullptr)
			BLAZE_ENGINE_CORE_FATAL("Incrementing a null iterator");
#endif

		if (((*(uint8*)ptr) >> 6) == 0)
			ptr = (byte*)ptr + 1;
		else if (((*(uint8*)((byte*)ptr + 1)) >> 4) == 0)
			ptr = (byte*)ptr + 2;
		else if (((*(uint8*)((byte*)ptr + 2)) >> 2) == 0)
			ptr = (byte*)ptr + 3;
		else if (((*(uint8*)((byte*)ptr + 3)) >> 5) == 0)
			ptr = (byte*)ptr + 4;
		value = UnicodeChar(ptr, 4);
		return *this;
	}
	constexpr StringViewUTF8Iterator StringViewUTF8Iterator::operator--()
	{
#ifdef BLAZE_NULL_ITERATOR_CHECK
		if (ptr == nullptr)
			BLAZE_ENGINE_CORE_FATAL("Decrementing a null iterator");
#endif

		StringViewUTF8Iterator old = *this;
		if ((*((byte*)ptr - 1) & 0b10000000) == 0)
			ptr = (byte*)ptr - 1;
		else if ((*((byte*)ptr - 2) & 0b10000000) == 0)
			ptr = (byte*)ptr - 2;
		else if ((*((byte*)ptr - 3) & 0b10000000) == 0)
			ptr = (byte*)ptr - 3;
		else if ((*((byte*)ptr - 4) & 0b10000000) == 0)
			ptr = (byte*)ptr - 4;
		value = UnicodeChar(ptr, 4);
		return old;
	}
	constexpr StringViewUTF8Iterator& StringViewUTF8Iterator::operator--(int)
	{
#ifdef BLAZE_NULL_ITERATOR_CHECK
		if (ptr == nullptr)
			BLAZE_ENGINE_CORE_FATAL("Decrementing a null iterator");
#endif

		if ((*((byte*)ptr - 1) & 0b10000000) == 0)
			ptr = (byte*)ptr - 1;
		else if ((*((byte*)ptr - 2) & 0b10000000) == 0)
			ptr = (byte*)ptr - 2;
		else if ((*((byte*)ptr - 3) & 0b10000000) == 0)
			ptr = (byte*)ptr - 3;
		else if ((*((byte*)ptr - 4) & 0b10000000) == 0)
			ptr = (byte*)ptr - 4;
		value = UnicodeChar(ptr, 4);
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
			BLAZE_ENGINE_CORE_FATAL("Comparing a null iterator");
#endif

		return ptr < i.ptr;
	}
	constexpr bool StringViewUTF8Iterator::operator>(const StringViewUTF8Iterator& i) const
	{
#ifdef BLAZE_NULL_ITERATOR_CHECK
		if ((ptr == nullptr) != (i.ptr == nullptr))
			BLAZE_ENGINE_CORE_FATAL("Comparing a null iterator");
#endif

		return ptr > i.ptr;
	}
	constexpr bool StringViewUTF8Iterator::operator<=(const StringViewUTF8Iterator& i) const
	{
#ifdef BLAZE_NULL_ITERATOR_CHECK
		if ((ptr == nullptr) != (i.ptr == nullptr))
			BLAZE_ENGINE_CORE_FATAL("Comparing a null iterator");
#endif

		return ptr <= i.ptr;
	}
	constexpr bool StringViewUTF8Iterator::operator>=(const StringViewUTF8Iterator& i) const
	{
#ifdef BLAZE_NULL_ITERATOR_CHECK
		if ((ptr == nullptr) != (i.ptr == nullptr))
			BLAZE_ENGINE_CORE_FATAL("Comparing a null iterator");
#endif

		return ptr >= i.ptr;
	}
	constexpr StringViewUTF8Iterator& StringViewUTF8Iterator::operator=(const StringViewUTF8Iterator& i)
	{
		ptr = i.ptr;
		value = i.value;
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
			bufferSize = other.Count() + 1;
			characterCount = other.Count();
		}
	}
	inline StringViewUTF8::StringViewUTF8(const String& other)
		: buffer(nullptr), bufferSize(0), characterCount(0)		
	{
		if (!other.Empty())
		{
			buffer = other.Ptr();
			bufferSize = other.Count() + 1;
			characterCount = other.Count();
		}
	}	
	inline StringViewUTF8::StringViewUTF8(const void* ptr, uintMem size)
		: buffer(nullptr), bufferSize(0), characterCount(0)
	{
		if (ptr != 0 && size != 0)		
		{
			if (((char*)ptr)[size] != '\0')
				BLAZE_ENGINE_CORE_ERROR("Passing a not null-terminate string to StringViewUTF8");

			buffer = ptr;
			bufferSize = size + 1;
			for (auto it = FirstIterator(); it != BehindIterator(); ++it, ++characterCount);
		}
	}
	inline StringViewUTF8::StringViewUTF8(const char* ptr, uintMem count)
		: buffer(nullptr), bufferSize(0), characterCount(0)
	{
		if (ptr != nullptr && count != 0)
		{
			if (((char*)ptr)[count] != '\0')
				BLAZE_ENGINE_CORE_ERROR("Passing a not null-terminate string to StringViewUTF8");

			buffer = ptr;
			bufferSize = count + 1;
			for (auto it = FirstIterator(); it != BehindIterator(); ++it, ++characterCount);
		}
	}
	inline StringViewUTF8::StringViewUTF8(StringViewUTF8Iterator begin, StringViewUTF8Iterator end)
		: buffer(nullptr), bufferSize(0), characterCount(0)
	{
		if (begin != end && begin.Ptr() != nullptr && end.Ptr() != nullptr)
		{
			buffer = begin.Ptr();
			bufferSize = (byte*)end.ptr - (byte*)begin.ptr;

			for (auto it = FirstIterator(); it != BehindIterator(); ++it, ++characterCount);
		}
	}
	constexpr void StringViewUTF8::Clear()
	{
		buffer = nullptr;
		bufferSize = 0;
		characterCount = 0;
	}
	inline StringUTF8 StringViewUTF8::SubString(uintMem start, uintMem count) const
	{
		auto b = FirstIterator();
		for (uintMem i = 0; i != start; ++i, ++b);
		auto e = b;
		for (uintMem i = 0; i != count; ++i, ++e);

		return StringUTF8(b.ptr, (char*)e.ptr - (char*)b.ptr);
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
			BLAZE_ENGINE_CORE_FATAL("String is empty");
#endif
		return UnicodeChar(buffer, bufferSize);
	}
	constexpr StringViewUTF8Iterator StringViewUTF8::FirstIterator() const
	{		
		if (buffer == nullptr)
			return StringViewUTF8Iterator(nullptr, 0);
		return StringViewUTF8Iterator(buffer, bufferSize - 1);
	}	
	constexpr StringViewUTF8Iterator StringViewUTF8::BehindIterator() const
	{		
		if (buffer == nullptr)
			return StringViewUTF8Iterator(nullptr, 0);
		return StringViewUTF8Iterator((byte*)buffer + bufferSize - 1, 0);
	}
	constexpr bool StringViewUTF8::operator==(const StringViewUTF8& s) const
	{
		if (bufferSize != s.bufferSize)
			return false;

		return memcmp(buffer, s.buffer, bufferSize) == 0;
	}
	constexpr bool StringViewUTF8::operator!=(const StringViewUTF8& s) const
	{
		if (bufferSize != s.bufferSize)
			return true;

		return memcmp(buffer, s.buffer, bufferSize) != 0;
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
		characterCount = other.Count();
		bufferSize = characterCount + 1;
		return *this;
	}
	inline StringViewUTF8& StringViewUTF8::operator=(const String& other)
	{
		buffer = other.Ptr();
		characterCount = other.Count();
		bufferSize = characterCount + 1;
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