#pragma once

namespace Blaze
{
	class String;
	class StringViewUTF8;
	class StringUTF8;

	class StringViewIterator
	{
	public:
		constexpr StringViewIterator();
		constexpr StringViewIterator(const StringViewIterator&);
		constexpr StringViewIterator(const StringIterator&);

		constexpr bool IsNull() const;

		constexpr const char* Ptr() const;
		constexpr const char& operator*() const;

		constexpr StringViewIterator operator++();
		constexpr StringViewIterator& operator++(int);
		constexpr StringViewIterator operator--();
		constexpr StringViewIterator& operator--(int);

		constexpr StringViewIterator operator+(const uintMem& value) const;
		constexpr StringViewIterator operator+(const intMem& value) const;
		constexpr StringViewIterator operator-(const uintMem& value) const;
		constexpr StringViewIterator operator-(const intMem& value) const;

		constexpr StringViewIterator& operator+=(const uintMem& value);
		constexpr StringViewIterator& operator+=(const intMem& value);
		constexpr StringViewIterator& operator-=(const uintMem& value);
		constexpr StringViewIterator& operator-=(const intMem& value);

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

	class StringView
	{
		const char* ptr;
		uintMem count;
	public:
		constexpr StringView();
		constexpr StringView(const StringView& other);		
		inline StringView(const String& other);		
		constexpr StringView(const char* ptr, uintMem count);
		constexpr StringView(const char* begin, const char* end);
		template<size_t C>
		constexpr StringView(const char(&arr)[C]);

		constexpr void Clear();

		constexpr bool Empty() const { return count == 0; }
		inline uint32 Hash() const;
		constexpr uintMem CountOf(char ch) const;
		constexpr const char* Ptr() const { return ptr; }
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
