#include "BlazeEngine/DataStructures/String.h"
#include "BlazeEngine/DataStructures/StringView.h"
#include <algorithm>

namespace Blaze
{
	String::String()
		: ptr(nullptr), size(0)
	{

	}
	String::String(const String& s)
		: ptr(new char[s.size + 1]), size(s.size)
	{
		memcpy(ptr, s.ptr, size + 1);
	}
	String::String(String&& s)
		: ptr(std::exchange(s.ptr, nullptr)), size(s.size)
	{

	}
	String::String(const StringView& c)
		: ptr(new char[c.Size() + 1]), size(c.Size())
	{
		memcpy(ptr, c.Ptr(), size);
		ptr[size] = '\0';
	}

	String::String(const char* _ptr)
		: ptr(nullptr), size(0)
	{
		while (_ptr[size] != '\0') ++size;
		ptr = new char[size + 1];
		memcpy(ptr, _ptr, size + 1);
	}
	String::String(char* ptr, size_t size, bool copy)
		: size(size)
	{
		if (copy)
		{
			this->ptr = new char[size + 1];
			memcpy(this->ptr, ptr, size);
			this->ptr[size] = '\0';
		}
		else		
			this->ptr = ptr;		
	}
	String::String(const char& c, const unsigned& size)
		: ptr(new char[size + 1]), size(size)
	{
		memset(ptr, c, size);
		ptr[size] = '\0';
	}

	String::~String()
	{
		delete[] ptr;
	}

	void String::Clear()
	{
		delete[] ptr;
		ptr = nullptr;
		size = 0;
	}

	void String::Resize(size_t newSize, char def)
	{
		if (size == newSize) return;

		if (newSize == 0)
		{ 
			delete[] ptr;
			ptr = nullptr;
			size = 0;
		}
		else
		{
			char* newPtr = new char[newSize + 1];
			memcpy(newPtr, ptr, std::min(newSize, size));
			if (newSize > size)
				memset(newPtr + size, def, newSize - size);
			newPtr[newSize] = '\0';
			delete[] ptr;
			ptr = newPtr;
			size = newSize;
		}
	}

	void String::Append(const String& s)
	{
		if (s.ptr == nullptr) return;

		if (ptr == nullptr)
		{			
			ptr = new char[s.size + 1];
			size = s.size;
			memcpy(ptr, s.ptr, size + 1);
		}
		else
		{			
			char* oldPtr = ptr;
			ptr = new char[size + s.size + 1];

			memcpy(ptr, oldPtr, size);
			memcpy(ptr + size, s.ptr, s.size + 1);

			delete[] oldPtr;
			size += s.size;
		}
	}
	void String::Append(const char* _ptr)
	{
		if (_ptr == nullptr) return;

		if (ptr == nullptr)
		{			
			size = 0;
			while (_ptr[size] != '\0') ++size;

			if (size == 0) return;

			ptr = new char[size + 1];
			memcpy(ptr, _ptr, size + 1);			
		}
		else
		{
			size_t _size = 0;
			while(_ptr[_size] != '\0') ++_size;

			if (_size == 0) return;

			char* oldPtr = ptr;
			ptr = new char[size + _size + 1];

			memcpy(ptr, oldPtr, size);
			memcpy(ptr + size, _ptr, _size + 1);
			
			delete[] oldPtr;
			size += _size;			
		}
	}
	void String::Append(char* _ptr, size_t _size)
	{
		if (_ptr == nullptr) return;
		if (_size == 0) return;

		if (ptr == nullptr)
		{			
			ptr = new char[_size + 1];

			memcpy(ptr, _ptr, _size);
			ptr[_size] = '\0';

			size = _size;			
		}
		else
		{			
			char* oldPtr = ptr;
			ptr = new char[size + _size + 1];

			memcpy(ptr, oldPtr, size);
			memcpy(ptr + size, _ptr, _size + 1);
			
			delete[] oldPtr;
			size += _size;			
		}
	}
	void String::Append(const char& c)
	{

		char* oldPtr = ptr;
		ptr = new char[size + 2];

		memcpy(ptr, oldPtr, size);
		ptr[size] = c;
		ptr[size + 1] = '\0';
		
		delete[] oldPtr;
		size = size + 1;		
	}

	void String::Replace(char oldVal, char newVal)
	{
		if (ptr == nullptr) return;
			
		std::replace(ptr, ptr + size, oldVal, newVal);
	}

	void String::Reverse()
	{
		if (ptr == nullptr) return;		

		std::reverse(ptr, ptr + size);
	}

	int String::Find(char val) const
	{
		if (ptr == nullptr) return -1;

		char* i = std::find(ptr, ptr + size, val);
		if (i - ptr == size) return -1;
		return i - ptr;
	}
	int String::Find(const std::vector<char>& val)
	{
		if (ptr == nullptr) return -1;

		char* i = std::find_first_of(ptr, ptr + size, val.begin(), val.end());
		if (i - ptr == size) return -1;
		return i - ptr;
	}

	int String::Count(char val) const
	{
		if (ptr == nullptr) return -1;

		return std::count(ptr, ptr + size, val);
	}

	void String::operator+= (const String& s) { Append(s); }
	void String::operator+= (const char* _ptr) { Append(_ptr); }
	void String::operator+= (const char& c) { Append(c); }

	String String::operator+ (const String& s)
	{		
		if (s.ptr == nullptr) return *this;
		if (ptr == nullptr) return s;

		char* newPtr = new char[size + s.size + 1];
		memcpy(newPtr, ptr, size);
		memcpy(newPtr + size, s.ptr, s.size + 1);

		String out;
		out.ptr = newPtr;
		out.size = size + s.size;		
		return out;
	}
	String String::operator+ (const char* _ptr)
	{
		if (ptr == nullptr) return String(_ptr);

		size_t _size = 0;
		while(_ptr[_size] != '\0') ++_size;

		if (_size == 0) return *this;

		char* newPtr = new char[size + _size + 1];

		memcpy(newPtr, ptr, size);
		memcpy(newPtr + size, _ptr, _size + 1);

		String out;
		out.ptr = newPtr;
		out.size = size + _size;		
		return out;
	}
	String String::operator+ (const char& c)
	{
		if (ptr == nullptr)
			return String((char*)&c, 1);

		char* newPtr = new char[size + 1];
		memcpy(newPtr, ptr, size);
		newPtr[size] = c;		

		return String(newPtr, size + 1, false);
	}

	void String::operator= (const String& s)
	{
		delete[] ptr;

		size = s.size;
		ptr = new char[size + 1];
		memcpy(ptr, s.ptr, size + 1);
	}
	void String::operator= (String&& s)
	{
		delete[] ptr;
		ptr = std::exchange(s.ptr, nullptr);
		size = s.size;
	}
	void String::operator= (const char* _ptr)
	{		
		delete[] ptr;
		
		size = 0;
		while(_ptr[size] != '\0') ++size;
		ptr = new char[size + 1];
		memcpy(ptr, _ptr, size + 1);
	}

	bool String::operator==(const String& s) const
	{
		if (size != s.size) return false;		
		return strcmp(ptr, s.ptr) == 0;
	}
	bool String::operator==(const char* _ptr) const
	{
		size_t _size = 0;
		for (; _ptr[_size] != '\0'; _size++);

		if (size != _size) return false;
		return strcmp(ptr, _ptr) == 0;
	}
	bool String::operator!=(const String& s) const
	{
		if (size != s.size) return true;
		return strcmp(ptr, s.ptr) != 0;
	}
	bool String::operator!=(const char* _ptr) const
	{
		size_t _size = 0;
		while (_ptr[_size] != '\0') ++_size;

			if (size != _size) return true;
		return strcmp(ptr, _ptr) != 0;
	}
	const char& String::operator[](int index) const
	{
		return ptr[index];
	}
	char& String::operator[](int index)
	{
		return ptr[index];
	}


	String operator+ (const char* _ptr, const String& s)
	{
		if (s.ptr == nullptr) return String(_ptr);

		size_t _size = 0;
		while(_ptr[_size] != '\0') ++_size;

		if (_size == 0) return s;

		char* newPtr = new char[s.size + _size + 1];

		memcpy(newPtr, _ptr, _size);
		memcpy(newPtr + _size, s.ptr, s.size + 1);

		String out;
		out.ptr = newPtr;
		out.size = _size + s.size;				
		return out;
	}
	String BLAZE_API operator+(const char& c, const String& s)
	{
		if (s.ptr == nullptr)
			return String((char*)&c, 1);

		char* newPtr = new char[s.size + 1];
		newPtr[0] = c;
		memcpy(newPtr + 1, s.ptr, s.size);

		return String(newPtr, s.size + 1, false);
	}

	bool BLAZE_API operator==(const char* _ptr, const String& s)
	{
		size_t _size = 0;
		while(_ptr[_size] != '\0') ++_size;

		if (s.Size() != _size) return false;
		return memcmp(s.Ptr(), _ptr, _size) == 0 ? true : false;
	}
	bool BLAZE_API operator!=(const char* _ptr, const String& s)
	{
		size_t _size = 0;
		while (_ptr[_size] != '\0') ++_size;

		if (s.Size() != _size) return true;
		return memcmp(s.Ptr(), _ptr, _size) == 0 ? false : true;
	}
}