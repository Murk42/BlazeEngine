#pragma once
#include "BlazeEngineCore/BlazeEngineCoreDefines.h"

namespace Blaze
{
	class StringView;
	class StringUTF8;
	class StringViewUTF8;
	class StringViewIterator;

	class BLAZE_CORE_API StringIterator
	{
	public:
		StringIterator();
		StringIterator(const StringIterator&);

		bool IsNull() const;
		char* Ptr() const;
		char& operator*();
		const char& operator*() const;

		StringIterator operator++();
		StringIterator& operator++(int);
		StringIterator operator--();
		StringIterator& operator--(int);
		
		StringIterator operator+(const intMem& value) const;		
		StringIterator operator-(const intMem& value) const;
		
		StringIterator& operator+=(const intMem& value);		
		StringIterator& operator-=(const intMem& value);

		uintMem operator-(const StringIterator& other);
		uintMem operator-(const StringViewIterator& other);

		bool operator==(const StringIterator&) const;
		bool operator!=(const StringIterator&) const;
		bool operator<(const StringIterator&) const;
		bool operator>(const StringIterator&) const;
		bool operator<=(const StringIterator&) const;
		bool operator>=(const StringIterator&) const;

		StringIterator& operator=(const StringIterator&);
	private:
		char* ptr;

		StringIterator(char* ptr);

		friend class String;
	};

	class BLAZE_CORE_API String
	{
		char* ptr;
		uintMem count;
	public:
		String();
		String(const String& other);
		String(String&& other) noexcept;
		String(const StringView&);		
		String(uintMem size, char fill = ' ');
		String(StringIterator begin, StringIterator end);
		String(StringViewIterator begin, StringViewIterator end);
		//Count doesn't include the null-terminating character
		String(const char* ptr, uintMem count);
		template<uintMem C>
		String(const char(&arr)[C]);
		~String();

		void Clear();
		String SubString(uintMem start, uintMem count) const;
		String& Resize(uintMem newCount, char fill = ' ');		

		inline bool Empty() const { return count == 0; }
		uint32 Hash() const;
		inline char* Ptr() { return ptr; }
		inline const char* Ptr() const { return ptr; }
		inline uintMem Count() const { return count; }

		char& First();
		const char& First() const;
		char& Last();
		const char& Last() const;
		StringIterator FirstIterator();
		StringViewIterator FirstIterator() const;
		StringIterator LastIterator();
		StringViewIterator LastIterator() const;
		StringIterator AheadIterator();
		StringViewIterator AheadIterator() const;
		StringIterator BehindIterator();
		StringViewIterator BehindIterator() const;

		char& operator[](uintMem index) { return ptr[index]; }
		const char& operator[](uintMem index) const { return ptr[index]; }

		bool operator==(const StringView& s) const;
		bool operator!=(const StringView& s) const;

		String& operator+=(const String&);
		String& operator+=(const StringView&);		
		String& operator+=(const char&);
		template<uintMem C>
		String& operator+=(const char (&arr)[C]);

		String& operator=(const String& other);
		String& operator=(String&& other) noexcept;
		String& operator=(const StringView& other);		
		template<size_t C>
		constexpr String& operator=(const char(&arr)[C]);

		friend BLAZE_CORE_API String operator+(const StringView& left, const StringView& right);
	};

	BLAZE_CORE_API StringIterator begin(String& string);
	BLAZE_CORE_API StringViewIterator begin(const String& string);
	BLAZE_CORE_API StringIterator end(String& string);
	BLAZE_CORE_API StringViewIterator end(const String& string);

	BLAZE_CORE_API String operator+(const StringView& left, const StringView& right);
	BLAZE_CORE_API String operator+(const StringView& left, const String& right);
	template<uintMem C>
	String operator+(const StringView& left, const char(&right)[C]);
	BLAZE_CORE_API String operator+(const StringView& left, const char& right);
	BLAZE_CORE_API String operator+(const String& left, const StringView& right);		
	template<uintMem C>
	String operator+(const char(&left)[C], const StringView& right);
	BLAZE_CORE_API String operator+(const char& left, const StringView& right);

	BLAZE_CORE_API String operator+(const String& left, const String& right);
	template<uintMem C>
	String operator+(const String& left, const char (&right)[C]);
	BLAZE_CORE_API String operator+(const String& left, const char& right);	
	BLAZE_CORE_API String operator+(const char& left, const String& right);
	template<uintMem C>
	String operator+(const char(&left)[C], const String& right);

	template<uintMem C>
	inline String::String(const char(&arr)[C])
		: String(arr, C - 1)
	{
	}
	template<uintMem C>
	inline String& String::operator+=(const char(&arr)[C])
	{
		*this = StringView(*this) + StringView(arr, C - 1);
		return *this;
	}
	template<size_t C>
	constexpr String& String::operator=(const char(&arr)[C])
	{
		return operator=(StringView(arr, C - 1));
	}
	template<uintMem C>
	String operator+(const StringView& left, const char(&right)[C])
	{
		return left + StringView(right, C - 1);
	}
	template<uintMem C>
	String operator+(const char(&left)[C], const StringView& right)
	{
		return StringView(left, C - 1) + right;
	}
	template<uintMem C>
	String operator+(const String& left, const char(&right)[C])
	{
		return StringView(left) + StringView(right, C - 1);
	}
	template<uintMem C>
	String operator+(const char(&left)[C], const String& right)
	{
		return StringView(left, C - 1) + StringView(right);
	}
}