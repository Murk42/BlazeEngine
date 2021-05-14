#include "BlazeEngine/DataStructures/StringView.h"
#include "BlazeEngine/DataStructures/String.h"
#include <algorithm>

namespace Blaze
{
	StringView::StringView()
		: ptr(nullptr), size(0)
	{
	}
	constexpr StringView::StringView(const StringView& s)
		: ptr(s.ptr), size(s.size)
	{
	}
	StringView::StringView(const String& s)
		: ptr(s.Ptr()), size(s.Size())
	{
	}
	constexpr StringView::StringView(const char* ptr)
		: ptr((char*)ptr), size(0)
	{
		for (; ptr[size] != '\0'; size++);
	}
	constexpr StringView::StringView(const char* ptr, size_t size)
		: ptr(ptr), size(size)
	{
		if (size == 0)
			ptr = nullptr;
	}	

	size_t StringView::Find(char val) const
	{
		if (ptr == nullptr) return -1;

		const char* i = std::find(ptr, ptr + size, val);
		if (i - ptr == size) return -1;
		return i - ptr;
	}
	size_t StringView::Find(const std::vector<char>& val)
	{
		if (ptr == nullptr) return -1;

		const char* i = std::find_first_of(ptr, ptr + size, val.begin(), val.end());
		if (i - ptr == size) return -1;
		return i - ptr;
	}
	size_t StringView::Count(char val) const
	{
		if (ptr == nullptr) return 0;

		return std::count(ptr, ptr + size, val);
	}
	constexpr StringView& StringView::operator=(const StringView& s)
	{
		ptr = s.ptr;
		size = s.size;
		return *this;
	}	
	StringView& StringView::operator=(const String& s)
	{
		ptr = s.Ptr();
		size = s.Size();
		return *this;
	}

	constexpr StringView& StringView::operator=(const char* _ptr)
	{
		ptr = _ptr;
		for (; ptr[size] != '\0'; size++);
		return *this;
	}

	constexpr bool StringView::operator==(const StringView& s) const
	{
		if (size != s.size) return false;
		return memcmp(ptr, s.ptr, size) == 0 ? true : false;
	}	
	constexpr bool StringView::operator!=(const StringView& s) const
	{
		if (size != s.size) return true;
		return memcmp(ptr, s.ptr, size) == 0 ? false : true;
	}	
}