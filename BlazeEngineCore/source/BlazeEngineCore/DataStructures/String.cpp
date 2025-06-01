#include "pch.h"
#include "BlazeEngineCore/DataStructures/String.h"
#include "BlazeEngineCore/DataStructures/StringUTF8.h"
#include "BlazeEngineCore/Debug/Logger.h"

namespace Blaze
{
	StringIterator::StringIterator()
		: ptr(nullptr)
	{
	}
	StringIterator::StringIterator(const StringIterator& i)
		: ptr(i.ptr)
	{
	}
	bool StringIterator::IsNull() const
	{
		return ptr == nullptr;
	}
	char* StringIterator::Ptr() const
	{
		return ptr;
	}

	char& StringIterator::operator*()
	{
#ifdef BLAZE_NULL_ITERATOR_CHECK
		if (ptr == nullptr)
			BLAZE_ENGINE_CORE_FATAL("Dereferencing a null iterator");
#endif
		return *ptr;
	}
	const char& StringIterator::operator*() const
	{
#ifdef BLAZE_NULL_ITERATOR_CHECK
		if (ptr == nullptr)
			BLAZE_ENGINE_CORE_FATAL("Dereferencing a null iterator");
#endif
		return *ptr;
	}
	StringIterator StringIterator::operator++()
	{
#ifdef BLAZE_NULL_ITERATOR_CHECK
		if (ptr == nullptr)
			BLAZE_ENGINE_CORE_FATAL("Incrementing a null iterator");
#endif

		auto old = *this;
		++ptr;
		return old;
	}
	StringIterator& StringIterator::operator++(int)
	{
#ifdef BLAZE_NULL_ITERATOR_CHECK
		if (ptr == nullptr)
			BLAZE_ENGINE_CORE_FATAL("Incrementing a null iterator");
#endif

		++ptr;
		return *this;
	}
	StringIterator StringIterator::operator--()
	{
#ifdef BLAZE_NULL_ITERATOR_CHECK
		if (ptr == nullptr)
			BLAZE_ENGINE_CORE_FATAL("Decrementing a null iterator");
#endif

		auto old = *this;
		--ptr;
		return old;
	}
	StringIterator& StringIterator::operator--(int)
	{
#ifdef BLAZE_NULL_ITERATOR_CHECK
		if (ptr == nullptr)
			BLAZE_ENGINE_CORE_FATAL("Decrementing a null iterator");
#endif

		--ptr;
		return *this;
	}	
	StringIterator StringIterator::operator+(const intMem& value) const
	{
		return StringIterator(ptr + value);
	}	
	StringIterator StringIterator::operator-(const intMem& value) const
	{
		return StringIterator(ptr - value);
	}	
	StringIterator& StringIterator::operator+=(const intMem& value)
	{
		ptr += value;
		return *this;
	}	
	StringIterator& StringIterator::operator-=(const intMem& value)
	{
		ptr -= value;
		return *this;
	}
	uintMem StringIterator::operator-(const StringIterator& other)
	{
		return ptr - other.ptr;
	}
	uintMem StringIterator::operator-(const StringViewIterator& other)
	{
		return ptr - other.Ptr();
	}
	bool StringIterator::operator==(const StringIterator& i) const
	{
		return ptr == i.ptr;
	}
	bool StringIterator::operator!=(const StringIterator& i) const
	{
		return ptr != i.ptr;
	}
	bool StringIterator::operator<(const StringIterator& i) const
	{
#ifdef BLAZE_NULL_ITERATOR_CHECK
		if ((ptr == nullptr) != (i.ptr == nullptr))
			BLAZE_ENGINE_CORE_FATAL("Comparing a null iterator");
#endif

		return ptr < i.ptr;
	}
	bool StringIterator::operator>(const StringIterator& i) const
	{
#ifdef BLAZE_NULL_ITERATOR_CHECK
		if ((ptr == nullptr) != (i.ptr == nullptr))
			BLAZE_ENGINE_CORE_FATAL("Comparing a null iterator");
#endif

		return ptr > i.ptr;
	}
	bool StringIterator::operator<=(const StringIterator& i) const
	{
#ifdef BLAZE_NULL_ITERATOR_CHECK
		if ((ptr == nullptr) != (i.ptr == nullptr))
			BLAZE_ENGINE_CORE_FATAL("Comparing a null iterator");
#endif

		return ptr <= i.ptr;
	}
	bool StringIterator::operator>=(const StringIterator& i) const
	{
#ifdef BLAZE_NULL_ITERATOR_CHECK
		if ((ptr == nullptr) != (i.ptr == nullptr))
			BLAZE_ENGINE_CORE_FATAL("Comparing a null iterator");
#endif

		return ptr >= i.ptr;
	}
	StringIterator& StringIterator::operator=(const StringIterator& i)
	{
		ptr = i.ptr;		
		return *this;
	}
	StringIterator::StringIterator(char* ptr)
		: ptr(ptr)
	{
	}

	String::String()
		: ptr(nullptr), count(0)
	{		
	}
	String::String(const String& s)
		: count(s.count)
	{
		if (s.ptr != nullptr)
		{
			ptr = new char[count + 1];
			memcpy(ptr, s.ptr, count + 1);
		}
		else
			ptr = nullptr;
	}
	String::String(String&& s) noexcept
	{
		ptr = s.ptr;
		count = s.count;
		s.ptr = nullptr;
		s.count = 0;
	}
	String::String(const StringView& sv)
		: count(sv.Count())
	{
		if (count == 0)
			ptr = nullptr;
		else
		{
			ptr = new char[count + 1];
			memcpy(ptr, sv.Ptr(), count);
			ptr[count] = '\0';
		}
	}	
	String::String(uintMem count, char fill)		
		: ptr(nullptr), count(0)
	{
		if (count != 0)
		{
			ptr = new char[count + 1];
			memset(ptr, fill, count);
			ptr[count] = '\0';
			this->count = count;
		}
	}
	String::String(StringIterator begin, StringIterator end)
		: String(begin.ptr, end.ptr - begin.ptr)
	{
	}
	String::String(StringViewIterator begin, StringViewIterator end)
		: String(begin.ptr, end.ptr - begin.ptr)
	{
	}
	String::String(const char* ptr, uintMem count)
		: ptr(nullptr), count(0)
	{				
		if (count != 0 && ptr != nullptr)
		{
			this->ptr = new char[count + 1];
			memcpy(this->ptr, ptr, count);
			this->ptr[count] = '\0';
			this->count = count;
		}
	}	
	String::~String()
	{
		delete[] ptr;
	}	
	void String::Clear()
	{
		delete[] ptr;
		ptr = nullptr;
		count = 0;
	}
	String String::SubString(uintMem start, uintMem count) const
	{
		return String(ptr + start, count);
	}
	String& String::Resize(uintMem newCount, char fill)
	{
		char* old = ptr;
		ptr = new char[newCount + 1];		

		if (newCount > count)
		{
			memcpy(ptr, old, count);
			std::fill_n(ptr + count, newCount - count, fill);
		}
		else
			memcpy(ptr, old, newCount);

		count = newCount;
		delete[] old;
		ptr[count] = '\0';
		return *this;		
	}	
	uint32 String::Hash() const
	{
		return static_cast<uint32>(std::hash<String>()(*this));
	}
	char& String::First()
	{ 
#ifdef BLAZE_INVALID_ITERATOR_CHECK
		if (count == 0)
			BLAZE_ENGINE_CORE_FATAL("String is empty");
#endif
		return ptr[0];
	}
	const char& String::First() const
	{
#ifdef BLAZE_INVALID_ITERATOR_CHECK
		if (count == 0)
			BLAZE_ENGINE_CORE_FATAL("String is empty");
#endif
		return ptr[0];
	}
	char& String::Last()
	{
#ifdef BLAZE_INVALID_ITERATOR_CHECK
		if (count == 0)
			BLAZE_ENGINE_CORE_FATAL("String is empty");
#endif
		return ptr[count - 1];
	}
	const char& String::Last() const
	{
#ifdef BLAZE_INVALID_ITERATOR_CHECK
		if (count == 0)
			BLAZE_ENGINE_CORE_FATAL("String is empty");
#endif
		return ptr[count - 1];
	}
	StringIterator String::FirstIterator()
	{
		return StringIterator(ptr);
	}
	StringViewIterator String::FirstIterator() const
	{
		return StringViewIterator(ptr);
	}
	StringIterator String::LastIterator()
	{
		return StringIterator(ptr + count - 1);
	}
	StringViewIterator String::LastIterator() const
	{
		return StringViewIterator(ptr + count - 1);
	}
	StringIterator String::AheadIterator()
	{
		return StringIterator(ptr - 1);
	}
	StringViewIterator String::AheadIterator() const
	{
		return StringViewIterator(ptr - 1);
	}
	StringIterator String::BehindIterator()
	{
		return StringIterator(ptr + count);
	}
	StringViewIterator String::BehindIterator() const
	{
		return StringViewIterator(ptr + count);
	}
	bool String::operator==(const StringView& s) const
	{
		if (ptr == nullptr && s.Ptr() == nullptr)
			return true;
		if (ptr == nullptr || s.Ptr() == nullptr || count != s.Count())
			return false;
		return memcmp(ptr, s.Ptr(), count) == 0;
	}
	bool String::operator!=(const StringView& s) const
	{
		return !(*this == s);
	}
	String& String::operator+=(const String& other)
	{
		return *this = *this + StringView(other);
	}
	String& String::operator+=(const StringView& other)
	{
		return *this = *this + other;		
	}	
	String& String::operator+=(const char& other)
	{
		return *this = *this + StringView(other);
	}		
	String& String::operator= (const String& s)
	{
		if (s.Count() == 0)
		{
			delete[] ptr;
			ptr = nullptr;
			count = 0;
			return *this;
		}
		else
		{
			delete[] ptr;
			count = s.Count();
			ptr = new char[count + 1];
			memcpy(ptr, s.Ptr(), count + 1);
			return *this;
		}
	}
	String& String::operator=(String&& s) noexcept
	{
		delete[] ptr;
		ptr = s.ptr;
		count = s.count;
		s.ptr = nullptr;
		s.count = 0;
		return *this;
	}
	String& String::operator=(const StringView& s)
	{
		delete[] ptr;
		if (s.Empty())
		{
			ptr = nullptr;
			count = 0;			
		}
		else
		{
			count = s.Count();
			ptr = new char[count + 1];
			memcpy(ptr, s.Ptr(), count + 1);
		}
		return *this;
	}	

	StringIterator begin(String& string)
	{
		return string.FirstIterator();
	}
	StringViewIterator begin(const String& string)
	{
		return string.FirstIterator();
	}
	StringIterator end(String& string)
	{
		return string.BehindIterator();
	}
	StringViewIterator end(const String& string)
	{
		return string.BehindIterator();
	}

	String operator+(const StringView& left, const StringView& right)
	{
		if (left.Ptr() == nullptr)
			return right;
		if (right.Ptr() == nullptr)
			return left;
		uintMem count = left.Count() + right.Count();
		char* ptr = new char[count + 1];
		memcpy(ptr, left.Ptr(), left.Count());
		memcpy(ptr + left.Count(), right.Ptr(), right.Count() + 1);
		String out;
		out.ptr = ptr;
		out.count = count;
		return out;
	}
	String operator+(const StringView& left, const String& right)
	{
		return operator+(left, StringView(right));
	}	
	String operator+(const StringView& left, const char& right)
	{
		return operator+(left, StringView(&right, 1));
	}
	String operator+(const String& left, const StringView& right)
	{
		return operator+(StringView(left), right);
	}	
	String operator+(const char& left, const StringView& right)
	{
		return operator+(StringView(&left, 1), right);		
	}
	String operator+(const String& left, const String& right)
	{
		return operator+(StringView(left), StringView(right));
	}	
	String operator+(const String& left, const char& right)
	{
		return operator+(StringView(left), right);
	}	
	String operator+(const char& left, const String& right)
	{
		return operator+(left, StringView(right));
	}

}