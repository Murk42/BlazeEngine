#include "pch.h"
#include "BlazeEngineCore/DataStructures/String.h"

namespace Blaze
{
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
	String::String(uintMem count)
		: count(count)
	{
		if (count == 0)
		{
			ptr = nullptr;
		}
		else
		{
			ptr = new char[count + 1];
			ptr[count] = '\0';
		}
	}	
	String::String(const char* ptr, uintMem count)
		: count(count)
	{		
		if (count == 0 || ptr == nullptr)
		{
			this->ptr = nullptr;
			this->count = 0;
		}
		else
		{
			this->ptr = new char[count + 1];
			memcpy(this->ptr, ptr, count);
			this->ptr[count] = '\0';
		}
	}
	String::String(const StringView& sv)
		: count(sv.Count())
	{
		if (count == 0)
			ptr = nullptr;
		else
		{
			ptr = new char[count + 1];
			memcpy(ptr, sv.Ptr(), count + 1);
		}
	}	
	String::String(const char* ptr)
		: String(ptr, (ptr == nullptr ? 0 : strlen(ptr)))
	{
	}
	String::~String()
	{
		delete[] ptr;
	}	
	String String::SubString(uintMem start, uintMem count) const
	{
		return String(ptr + start, count);
	}
	void String::Clear()
	{
		delete[] ptr;
		count = 0;
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
		return (uint32)std::hash<std::string_view>()(std::string_view(ptr, count));		
	}
	char* String::begin()
	{
		return ptr;
	}
	char* String::end()
	{
		return ptr + count;
	}
	String& String::operator=(const StringView& s)
	{
		delete[] ptr;
		if (s == nullptr)
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
	String& String::operator+=(const char* other)
	{
		return *this = *this + StringView(other);
	}
	String& String::operator+=(const char& other)
	{
		return *this = *this + StringView(other);
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
	String operator+(const StringView& left, const char* right)
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
	String operator+(const char* left, const StringView& right)
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

	String operator+(const String& left, const char* right)
	{
		return operator+(StringView(left), StringView(right));
	}
	String operator+(const String& left, const char& right)
	{
		return operator+(StringView(left), right);
	}

	String operator+(const char* left, const String& right)
	{
		return operator+(StringView(left), StringView(right));
	}
	String operator+(const char& left, const String& right)
	{
		return operator+(left, StringView(right));
	}

}