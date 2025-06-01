#pragma once
#include "BlazeEngineCore/DataStructures/String.h"
#include "BlazeEngineCore/Debug/Logger.h"

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
	inline StringViewIterator::StringViewIterator(const StringIterator& i)
		: ptr(i.Ptr())
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
			BLAZE_ENGINE_CORE_FATAL("Dereferencing a null iterator");
#endif
		return *ptr;
	}
	constexpr StringViewIterator StringViewIterator::operator++()
	{
#ifdef BLAZE_NULL_ITERATOR_CHECK
		if (ptr == nullptr)
			BLAZE_ENGINE_CORE_FATAL("Incrementing a null iterator");
#endif

		auto old = *this;
		++ptr;
		return old;
	}
	constexpr StringViewIterator& StringViewIterator::operator++(int)
	{
#ifdef BLAZE_NULL_ITERATOR_CHECK
		if (ptr == nullptr)
			BLAZE_ENGINE_CORE_FATAL("Incrementing a null iterator");
#endif

		++ptr;
		return *this;
	}
	constexpr StringViewIterator StringViewIterator::operator--()
	{
#ifdef BLAZE_NULL_ITERATOR_CHECK
		if (ptr == nullptr)
			BLAZE_ENGINE_CORE_FATAL("Decrementing a null iterator");
#endif

		auto old = *this;
		--ptr;
		return old;
	}
	constexpr StringViewIterator& StringViewIterator::operator--(int)
	{
#ifdef BLAZE_NULL_ITERATOR_CHECK
		if (ptr == nullptr)
			BLAZE_ENGINE_CORE_FATAL("Decrementing a null iterator");
#endif

		--ptr;
		return *this;
	}	
	constexpr StringViewIterator StringViewIterator::operator+(const intMem& value) const
	{
		return StringViewIterator(ptr + value);
	}	
	constexpr StringViewIterator StringViewIterator::operator-(const intMem& value) const
	{
		return StringViewIterator(ptr - value);
	}	
	constexpr StringViewIterator& StringViewIterator::operator+=(const intMem& value)
	{
		ptr += value;
		return *this;
	}	
	constexpr StringViewIterator& StringViewIterator::operator-=(const intMem& value)
	{
		ptr -= value;
		return *this;
	}
	constexpr uintMem StringViewIterator::operator-(const StringViewIterator& other)
	{
		return ptr - other.ptr;
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
			BLAZE_ENGINE_CORE_FATAL("Comparing a null iterator");
#endif

		return ptr < i.ptr;
	}
	constexpr bool StringViewIterator::operator>(const StringViewIterator& i) const
	{
#ifdef BLAZE_NULL_ITERATOR_CHECK
		if ((ptr == nullptr) != (i.ptr == nullptr))
			BLAZE_ENGINE_CORE_FATAL("Comparing a null iterator");
#endif

		return ptr > i.ptr;
	}
	constexpr bool StringViewIterator::operator<=(const StringViewIterator& i) const
	{
#ifdef BLAZE_NULL_ITERATOR_CHECK
		if ((ptr == nullptr) != (i.ptr == nullptr))
			BLAZE_ENGINE_CORE_FATAL("Comparing a null iterator");
#endif

		return ptr <= i.ptr;
	}
	constexpr bool StringViewIterator::operator>=(const StringViewIterator& i) const
	{
#ifdef BLAZE_NULL_ITERATOR_CHECK
		if ((ptr == nullptr) != (i.ptr == nullptr))
			BLAZE_ENGINE_CORE_FATAL("Comparing a null iterator");
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
		: ptr(nullptr), count(0)
	{
		if (count == 0)
			return;

		if (ptr[count - 1] == '\0')
			if (count == 1)
				return;
			else
			{
				this->ptr = ptr;
				this->count = count - 1;
			}
		else
		{
			this->ptr = ptr;
			this->count = count;
		}
	}
	constexpr StringView::StringView(const char* begin, const char* end)
		: ptr(nullptr), count(0)
	{
		if (begin != end && begin != nullptr && end != nullptr)
		{
			count = end - begin;			
			if (end[-1] == '\0')
				--count;

			if (count == 0)
				return;

			ptr = begin;
		}
	}
	inline constexpr StringView::StringView(StringViewIterator begin, StringViewIterator end)
		: StringView(begin.Ptr(), end.Ptr())
	{
		
	}
	template<size_t C>
	constexpr StringView::StringView(const char(&arr)[C])
		: ptr(nullptr), count(0)
	{
		if constexpr (C != 0)
		{
			if (arr[C - 1] == '\0')
				if (C == 1)
					return;
				else				
					count = C - 1;				
			else			
				count = C;			

			ptr = arr;
		}
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
			BLAZE_ENGINE_CORE_FATAL("String is empty");
#endif
		return ptr[0];
	}
	constexpr const char& StringView::Last() const
	{
#ifdef BLAZE_INVALID_ITERATOR_CHECK
		if (count == 0)
			BLAZE_ENGINE_CORE_FATAL("String is empty");
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
		return operator=(StringView(other));
	}	
	template<size_t C>
	constexpr StringView& StringView::operator=(const char(&arr)[C])
	{
		return operator=(StringView(arr));				
	}
}