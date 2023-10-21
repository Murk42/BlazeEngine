#pragma once

namespace Blaze
{
	class String;
	class StringViewUTF8;
	class BLAZE_API StringView
	{
		const char* ptr;
		uintMem size;
	public:
		constexpr StringView();
		constexpr StringView(const StringView&);
		
		constexpr StringView(const char*);
		constexpr StringView(const char*, uintMem count);
		constexpr StringView(const char* begin, const char* end);
		inline StringView(const String&);		

		constexpr const char* Ptr() const { return ptr; }
		constexpr uintMem Size() const { return size; }

		constexpr uintMem Count(char ch) const;
		inline uint32 Hash() const;

		constexpr StringView SubString(uintMem start, uintMem count);

		constexpr StringView& operator= (const StringView&);						

		constexpr bool operator==(const StringView& s) const;				
		constexpr bool operator!=(const StringView& s) const;		
		
		inline const char& operator[](uintMem index) const { return ptr[index]; }
	};

	constexpr StringView::StringView()
		: ptr(nullptr), size(0)
	{
	}
	constexpr StringView::StringView(const StringView& sv)
		: ptr(sv.ptr), size(sv.size)
	{
	}
	constexpr StringView::StringView(const char* ptr)
		: ptr(nullptr), size(0)
	{
		if (ptr == nullptr)
			return;
				
		while (ptr[size] != '\0')		
			++size;		

		if (size == 0)
			this->ptr = nullptr; //When ptr is "" its not nullptr
		else
			this->ptr = ptr;
	}
	constexpr StringView::StringView(const char* ptr, uintMem size)
		: size(size)
	{
		if (size == 0)
			this->ptr = nullptr;
		else
			this->ptr = ptr;
	}
	constexpr StringView::StringView(const char* begin, const char* end)
		: size(end - begin)
	{
		if (size == 0)
			this->ptr = nullptr; //When ptr is "" its not nullptr
		else
			this->ptr = begin;
	}
	inline StringView::StringView(const String& string)
		: ptr(string.Ptr()), size(string.Size())
	{
	}
	constexpr uintMem StringView::Count(char ch) const
	{
		size_t count = 0;
		const char* ptr = this->ptr;
		const char* endPtr = this->ptr + this->size;
		while (ptr != endPtr)
			count += (*(ptr++) == ch);
		return count;
	}
	inline uint32 StringView::Hash() const
	{		
		return (uint32)std::hash<std::string_view>()(std::string_view(ptr, size));		
	}
	constexpr StringView StringView::SubString(uintMem start, uintMem count)
	{
		return StringView(ptr + start, count);
	}
	constexpr StringView& StringView::operator=(const StringView& sv)
	{
		ptr = sv.ptr;
		size = sv.size;
		return *this;
	}
	constexpr bool StringView::operator==(const StringView& s) const
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
	constexpr bool StringView::operator!=(const StringView& s) const
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
