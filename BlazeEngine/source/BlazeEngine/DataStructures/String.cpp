#include "pch.h"
#include "BlazeEngine/DataStructures/String.h"

namespace Blaze
{
	String::String()
		: ptr(nullptr), size(0)
	{		
	}
	String::String(const String& s)
		: size(s.size)
	{
		if (s.ptr != nullptr)
		{
			ptr = new char[size + 1];
			memcpy(ptr, s.ptr, size + 1);
		}
		else
			ptr = nullptr;
	}
	String::String(String&& s) noexcept
	{
		ptr = s.ptr;
		size = s.size;
		s.ptr = nullptr;
		s.size = 0;
	}
	String::String(size_t size)
		: size(size)
	{
		if (size == 0)
		{
			ptr = nullptr;
		}
		else
		{
			ptr = new char[size + 1];
			ptr[size] = '\0';
		}
	}	
	String::String(const char* ptr, size_t size)
		: size(size)
	{		
		if (size == 0 || ptr == nullptr)
		{
			this->ptr = nullptr;
			this->size = 0;
		}
		else
		{
			this->ptr = new char[size + 1];
			memcpy(this->ptr, ptr, size);
			this->ptr[size] = '\0';
		}
	}
	String::String(const StringView& sv)
		: size(sv.Size())
	{
		if (size == 0)
			ptr = nullptr;
		else
		{
			ptr = new char[size + 1];
			memcpy(ptr, sv.Ptr(), size + 1);
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
	String String::SubString(size_t start, size_t size) const
	{
		return String(ptr + start, size);
	}
	void String::Clear()
	{
		delete[] ptr;
		size = 0;
	}
	String& String::Resize(size_t newSize, char fill)
	{
		char* old = ptr;
		ptr = new char[newSize + 1];		

		if (newSize > size)
		{
			memcpy(ptr, old, size);
			std::fill_n(ptr + size, newSize - size, fill);
		}
		else
			memcpy(ptr, old, newSize);

		size = newSize;
		delete[] old;
		ptr[size] = '\0';
		return *this;		
	}
	uint32 String::Hash() const
	{
		return std::hash<std::string_view>()(std::string_view(ptr, size));		
	}
	char* String::begin()
	{
		return ptr;
	}
	char* String::end()
	{
		return ptr + size;
	}
	String& String::operator=(const StringView& s)
	{
		delete[] ptr;
		if (s == nullptr)
		{
			ptr = nullptr;
			size = 0;			
		}
		else
		{
			size = s.Size();
			ptr = new char[size + 1];
			memcpy(ptr, s.Ptr(), size + 1);
		}
		return *this;
	}
	String& String::operator= (const String& s)
	{
		if (s.Size() == 0)
		{
			delete[] ptr;
			ptr = nullptr;
			size = 0;
			return *this;
		}
		else
		{
			delete[] ptr;
			size = s.Size();
			ptr = new char[size + 1];
			memcpy(ptr, s.Ptr(), size + 1);
			return *this;
		}
	}
	String& String::operator=(String&& s) noexcept
	{
		delete[] ptr;
		ptr = s.ptr;
		size = s.size;
		s.ptr = nullptr;
		s.size = 0;
		return *this;
	}
	bool String::operator==(const StringView& s) const
	{
		if (ptr == nullptr && s.Ptr() == nullptr)
			return true;
		if (ptr == nullptr || s.Ptr() == nullptr || size != s.Size())
			return false;
		return memcmp(ptr, s.Ptr(), size) == 0;
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
		size_t size = left.Size() + right.Size();
		char* ptr = new char[size + 1];
		memcpy(ptr, left.Ptr(), left.Size());
		memcpy(ptr + left.Size(), right.Ptr(), right.Size() + 1);
		String out;
		out.ptr = ptr;
		out.size = size;
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