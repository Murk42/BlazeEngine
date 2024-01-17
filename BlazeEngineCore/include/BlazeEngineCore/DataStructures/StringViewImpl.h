#include "StringView.h"
#pragma once

namespace Blaze
{
	constexpr StringViewIterator::StringViewIterator()
		: ptr(nullptr)
	{
	}
	constexpr StringViewIterator::StringViewIterator(const StringViewIterator& i)
		: ptr(i.ptr)
	{
	}
	constexpr bool StringViewIterator::IsNull() const
	{
		return ptr == nullptr;
	}	
	constexpr const char* StringViewIterator::Ptr() const
	{
		return ptr;
	}
	constexpr const char& StringViewIterator::operator*() const
	{
#ifdef BLAZE_NULL_ITERATOR_CHECK
		if (ptr == nullptr)
			Debug::Logger::LogFatal("Blaze Engine", "Dereferencing a null iterator");
#endif
		return *ptr;
	}
	constexpr StringViewIterator StringViewIterator::operator++()
	{
#ifdef BLAZE_NULL_ITERATOR_CHECK
		if (ptr == nullptr)
			Debug::Logger::LogFatal("Blaze Engine", "Incrementing a null iterator");
#endif

		auto old = *this;
		++ptr;
		return old;
	}
	constexpr StringViewIterator& StringViewIterator::operator++(int)
	{
#ifdef BLAZE_NULL_ITERATOR_CHECK
		if (ptr == nullptr)
			Debug::Logger::LogFatal("Blaze Engine", "Incrementing a null iterator");
#endif

		++ptr;
		return *this;
	}
	constexpr StringViewIterator StringViewIterator::operator--()
	{
#ifdef BLAZE_NULL_ITERATOR_CHECK
		if (ptr == nullptr)
			Debug::Logger::LogFatal("Blaze Engine", "Decrementing a null iterator");
#endif

		auto old = *this;
		--ptr;
		return old;
	}
	constexpr StringViewIterator& StringViewIterator::operator--(int)
	{
#ifdef BLAZE_NULL_ITERATOR_CHECK
		if (ptr == nullptr)
			Debug::Logger::LogFatal("Blaze Engine", "Decrementing a null iterator");
#endif

		--ptr;
		return *this;
	}
	constexpr StringViewIterator StringViewIterator::operator+(const uintMem& value) const
	{
		return StringViewIterator(ptr + value);
	}
	constexpr StringViewIterator StringViewIterator::operator+(const intMem& value) const
	{
		return StringViewIterator(ptr + value);
	}
	constexpr StringViewIterator StringViewIterator::operator-(const uintMem& value) const
	{
		return StringViewIterator(ptr - value);
	}
	constexpr StringViewIterator StringViewIterator::operator-(const intMem& value) const
	{
		return StringViewIterator(ptr - value);
	}
	constexpr StringViewIterator& StringViewIterator::operator+=(const uintMem& value)
	{
		ptr += value;
		return *this;
	}
	constexpr StringViewIterator& StringViewIterator::operator+=(const intMem& value)
	{
		ptr += value;
		return *this;
	}
	constexpr StringViewIterator& StringViewIterator::operator-=(const uintMem& value)
	{
		ptr -= value;
		return *this;
	}
	constexpr StringViewIterator& StringViewIterator::operator-=(const intMem& value)
	{
		ptr -= value;
		return *this;
	}
	constexpr bool StringViewIterator::operator==(const StringViewIterator& i) const
	{
		return ptr == i.ptr;
	}
	constexpr bool StringViewIterator::operator!=(const StringViewIterator& i) const
	{
		return ptr != i.ptr;
	}
	constexpr bool StringViewIterator::operator<(const StringViewIterator& i) const
	{
#ifdef BLAZE_NULL_ITERATOR_CHECK
		if ((ptr == nullptr) != (i.ptr == nullptr))
			Debug::Logger::LogFatal("Blaze Engine", "Comparing a null iterator");
#endif

		return ptr < i.ptr;
	}
	constexpr bool StringViewIterator::operator>(const StringViewIterator& i) const
	{
#ifdef BLAZE_NULL_ITERATOR_CHECK
		if ((ptr == nullptr) != (i.ptr == nullptr))
			Debug::Logger::LogFatal("Blaze Engine", "Comparing a null iterator");
#endif

		return ptr > i.ptr;
	}
	constexpr bool StringViewIterator::operator<=(const StringViewIterator& i) const
	{
#ifdef BLAZE_NULL_ITERATOR_CHECK
		if ((ptr == nullptr) != (i.ptr == nullptr))
			Debug::Logger::LogFatal("Blaze Engine", "Comparing a null iterator");
#endif

		return ptr <= i.ptr;
	}
	constexpr bool StringViewIterator::operator>=(const StringViewIterator& i) const
	{
#ifdef BLAZE_NULL_ITERATOR_CHECK
		if ((ptr == nullptr) != (i.ptr == nullptr))
			Debug::Logger::LogFatal("Blaze Engine", "Comparing a null iterator");
#endif

		return ptr >= i.ptr;
	}
	constexpr StringViewIterator& StringViewIterator::operator=(const StringViewIterator& i)
	{
		ptr = i.ptr;
		return *this;
	}
	constexpr StringViewIterator::StringViewIterator(const char* ptr)
		: ptr(ptr)
	{
	}

	constexpr StringView::StringView()
		: ptr(nullptr), count(0)
	{
	}
	constexpr StringView::StringView(const StringView& sv)
		: ptr(sv.ptr), count(sv.count)
	{
	}
	inline StringView::StringView(const String& other)
		: ptr(other.Ptr()), count(other.Count())
	{
	}	
	constexpr StringView::StringView(const char* ptr, uintMem count)
		: count(count)
	{
		if (count == 0)
			this->ptr = nullptr;
		else
			this->ptr = ptr;
	}
	constexpr StringView::StringView(const char* begin, const char* end)
		: count(end - begin)
	{
		if (count == 0)
			this->ptr = nullptr; //When ptr is "" its not nullptr
		else
			this->ptr = begin;
	}
	template<size_t C>
	constexpr StringView::StringView(const char(&arr)[C])
		: StringView(arr, C - 1)
	{
	}	
	inline uint32 StringView::Hash() const
	{
		return static_cast<uint32>(std::hash<StringView>()(*this));
	}
	constexpr uintMem StringView::CountOf(char ch) const
	{
		uintMem count = 0;
		const char* ptr = this->ptr;
		const char* endPtr = this->ptr + this->count;
		while (ptr != endPtr)
			count += (*(ptr++) == ch);
		return count;
	}
	inline String StringView::SubString(uintMem start, uintMem count) const
	{
		return String(ptr + start, count);
	}
	constexpr const char& StringView::First() const
	{
#ifdef BLAZE_INVALID_ITERATOR_CHECK
		if (count == 0)
			Debug::Logger::LogFatal("Blaze Engine", "String is empty");
#endif
		return ptr[0];
	}
	constexpr const char& StringView::Last() const
	{
#ifdef BLAZE_INVALID_ITERATOR_CHECK
		if (count == 0)
			Debug::Logger::LogFatal("Blaze Engine", "String is empty");
#endif
		return ptr[count - 1];
	}
	constexpr StringViewIterator StringView::FirstIterator() const
	{
		return StringViewIterator(ptr);
	}
	constexpr StringViewIterator StringView::LastIterator() const
	{
		if (ptr == nullptr)
			return StringViewIterator();
		return StringViewIterator(ptr + count - 1);
	}
	constexpr StringViewIterator StringView::AheadIterator() const
	{
		if (ptr == nullptr)
			return StringViewIterator();
		return StringViewIterator(ptr - 1);
	}
	constexpr StringViewIterator StringView::BehindIterator() const
	{		
		return StringViewIterator(ptr + count);
	}
	constexpr bool StringView::operator==(const StringView& s) const
	{
		if (count != s.count)
			return false;

		const char* end1 = ptr + count;
		const char* ptr1 = ptr;
		const char* ptr2 = s.ptr;

		while (ptr1 != end1)
		{
			if (*ptr1 != *ptr2)
				return false;

			++ptr1;
			++ptr2;
		}
		return true;
	}
	constexpr bool StringView::operator!=(const StringView& s) const
	{
		if (count != s.count)
			return true;

		const char* end1 = ptr + count;
		const char* ptr1 = ptr;
		const char* ptr2 = s.ptr;

		while (ptr1 != end1)
		{
			if (*ptr1 != *ptr2)
				return true;

			++ptr1;
			++ptr2;
		}
		return false;
	}
	constexpr StringView& StringView::operator=(const StringView& other)
	{
		ptr = other.ptr;
		count = other.count;
		return *this;
	}
	inline StringView& StringView::operator=(const String& other)
	{
		ptr = other.Ptr();
		count = other.Count();
		return *this;
	}	
	template<size_t C>
	constexpr StringView& StringView::operator=(const char(&arr)[C])
	{
		ptr = arr;
		count = C - 1;

		return *this;
	}
}