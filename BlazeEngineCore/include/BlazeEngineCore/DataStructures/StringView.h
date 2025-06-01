#pragma once
#include "BlazeEngineCore/BlazeEngineCoreDefines.h"

namespace Blaze
{
	class String;
	class StringViewUTF8;
	class StringUTF8;
	class StringIterator;

	class BLAZE_CORE_API StringViewIterator
	{
	public:
		constexpr StringViewIterator();
		constexpr StringViewIterator(const StringViewIterator&);
		StringViewIterator(const StringIterator&);

		constexpr bool IsNull() const;

		constexpr const char* Ptr() const;
		constexpr const char& operator*() const;

		constexpr StringViewIterator operator++();
		constexpr StringViewIterator& operator++(int);
		constexpr StringViewIterator operator--();
		constexpr StringViewIterator& operator--(int);
		
		constexpr StringViewIterator operator+(const intMem& value) const;		
		constexpr StringViewIterator operator-(const intMem& value) const;
		
		constexpr StringViewIterator& operator+=(const intMem& value);		
		constexpr StringViewIterator& operator-=(const intMem& value);

		constexpr uintMem operator-(const StringViewIterator& other);

		constexpr bool operator==(const StringViewIterator&) const;
		constexpr bool operator!=(const StringViewIterator&) const;
		constexpr bool operator<(const StringViewIterator&) const;
		constexpr bool operator>(const StringViewIterator&) const;
		constexpr bool operator<=(const StringViewIterator&) const;
		constexpr bool operator>=(const StringViewIterator&) const;

		constexpr StringViewIterator& operator=(const StringViewIterator&);
	private:
		const char* ptr;

		constexpr StringViewIterator(const char* ptr);

		friend class StringView;
		friend class String;
	};

	class BLAZE_CORE_API StringView
	{
		const char* ptr;
		uintMem count;
	public:
		constexpr StringView();
		constexpr StringView(const StringView& other);		
		inline StringView(const String& other);		
		constexpr StringView(const char* ptr, uintMem count);
		constexpr StringView(const char* begin, const char* end);
		constexpr StringView(StringViewIterator begin, StringViewIterator end);
		template<size_t C>
		constexpr StringView(const char(&arr)[C]);

		constexpr void Clear();

		constexpr bool Empty() const { return count == 0; }
		inline uint32 Hash() const;
		constexpr uintMem CountOf(char ch) const;
		//The returned pointer might not be null-terminated
		constexpr const char* Ptr() const { return ptr; }
		//Does not include the null-terminating character
		constexpr uintMem Count() const { return count; }

		inline String SubString(uintMem start, uintMem count) const;
		
		constexpr const char& First() const;
		constexpr const char& Last() const;
		constexpr StringViewIterator FirstIterator() const;		
		constexpr StringViewIterator LastIterator() const;		
		constexpr StringViewIterator AheadIterator() const;		
		constexpr StringViewIterator BehindIterator() const;

		constexpr const char& operator[](uintMem index) const { return ptr[index]; }

		constexpr bool operator==(const StringView& s) const;				
		constexpr bool operator!=(const StringView& s) const;				

		constexpr StringView& operator= (const StringView& other);
		inline StringView& operator= (const String& other);		
		template<size_t C>
		constexpr StringView& operator=(const char(&arr)[C]);
	};		
}
 
#include <string_view>

namespace std
{
	template <>
	struct hash<Blaze::StringView>
	{
		inline size_t operator()(const Blaze::StringView& k) const;
	};
	inline size_t hash<Blaze::StringView>::operator()(const Blaze::StringView& k) const
	{
		return hash<string_view>()(string_view(k.Ptr(), k.Count()));
	}
}