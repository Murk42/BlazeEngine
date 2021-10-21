#include "BlazeEngine/DataStructures/StringView.h"
#include "BlazeEngine/DataStructures/String.h"
#include <cstring>

namespace Blaze
{
	StringView::StringView()
		: ptr(nullptr), size(0)
	{
	}
	StringView::StringView(const StringView& sv)
		: ptr(sv.ptr), size(sv.size)
	{
	}	
	StringView::StringView(const char* ptr)
		: ptr(ptr), size(ptr != nullptr ? strlen(ptr) : 0)
	{
	}
	StringView::StringView(const char* ptr, size_t size)
		: ptr(ptr), size(size)
	{
	}
	StringView::StringView(const char* begin, const char* end)
		: ptr(begin), size(end - begin)
	{
	}
	StringView::StringView(const String& string)
		: ptr(string.Ptr()), size(string.Size())
	{
	}
	size_t StringView::Count(char ch) const
	{
		size_t count = 0;
		const char* ptr = this->ptr; 
		const char* endPtr = this->ptr + this->size;
		while (ptr != endPtr)
			count += (*(ptr++) == ch);
		return count;
	}

	StringView& StringView::operator=(const StringView& sv)
	{
		ptr = sv.ptr;
		size = sv.size;
		return *this;
	}
	bool StringView::operator==(const StringView& s) const
	{
		if (size != s.size)
			return false;		

		const char* end1 = ptr + size;		
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
	bool StringView::operator!=(const StringView& s) const
	{
		if (size != s.size)
			return true;

		const char* end1 = ptr + size;
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
}