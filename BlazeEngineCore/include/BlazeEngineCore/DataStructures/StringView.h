#pragma once

namespace Blaze
{
	class String;
	class StringViewUTF8;
	class BLAZE_CORE_API StringView
	{
		const char* ptr;
		uintMem count;
	public:
		constexpr StringView();
		constexpr StringView(const StringView&);
		
		constexpr StringView(const char*);
		constexpr StringView(const char*, uintMem count);
		constexpr StringView(const char* begin, const char* end);
		inline StringView(const String&);		

		constexpr const char* Ptr() const { return ptr; }
		constexpr uintMem Count() const { return count; }

		constexpr uintMem Count(char ch) const;
		inline uint32 Hash() const;

		constexpr StringView SubString(uintMem start, uintMem count);

		constexpr StringView& operator= (const StringView&);						

		constexpr bool operator==(const StringView& s) const;				
		constexpr bool operator!=(const StringView& s) const;		
		
		constexpr const char& operator[](uintMem index) const { return ptr[index]; }
	};

	constexpr StringView::StringView()
		: ptr(nullptr), count(0)
	{
	}
	constexpr StringView::StringView(const StringView& sv)
		: ptr(sv.ptr), count(sv.count)
	{
	}
	constexpr StringView::StringView(const char* ptr)
		: ptr(nullptr), count(0)
	{
		if (ptr == nullptr)
			return;
				
		while (ptr[count] != '\0')		
			++count;		

		if (count == 0)
			this->ptr = nullptr; //When ptr is "" its not nullptr
		else
			this->ptr = ptr;
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
	inline StringView::StringView(const String& string)
		: ptr(string.Ptr()), count(string.Count())
	{
	}
	constexpr uintMem StringView::Count(char ch) const
	{
		uintMem count = 0;
		const char* ptr = this->ptr;
		const char* endPtr = this->ptr + this->count;
		while (ptr != endPtr)
			count += (*(ptr++) == ch);
		return count;
	}	
	constexpr StringView StringView::SubString(uintMem start, uintMem count)
	{
		return StringView(ptr + start, count);
	}
	constexpr StringView& StringView::operator=(const StringView& sv)
	{
		ptr = sv.ptr;
		count = sv.count;
		return *this;
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
}
