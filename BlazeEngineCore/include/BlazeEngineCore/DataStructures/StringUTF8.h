#pragma once
#include "BlazeEngineCore/BlazeEngineCoreDefines.h"
#include "BlazeEngineCore/DataStructures/UnicodeChar.h"

namespace Blaze
{
	class StringViewUTF8;
	class StringView;
	class String;
	class StringViewUTF8Iterator;

	class BLAZE_CORE_API StringUTF8
	{
	public:
		/*
			Construct a empty UTF-8 string
		*/
		StringUTF8();
		/*
			Copies an another UTF-8 string
		*/
		StringUTF8(const StringUTF8& s);
		/*
			Moves an another UTF-8 string
		*/
		StringUTF8(StringUTF8&& s) noexcept;
		/*
			Copies the UTF-8 string from a UTF-8 string view
		*/
		StringUTF8(const StringViewUTF8&);
		/*
			Copies the UTF-8 string from a string. If the string view doesn't have a null-terminating character 
			it will be added
		*/
		StringUTF8(const String&);
		/*
			Copies the UTF-8 string from a string view
		*/
		StringUTF8(const StringView&);		
		/*
			Constructs a UTF-8 string from a buffer which holds a UTF-8 string. A normal string is also a valid 
			UTF-8 string. The string doesn't need to be null-terminated, but can be. If the string view doesn't 
			have a null-terminating character it will be added

			\param ptr     pointer to a buffer holding UTF-8 characters. If <ptr> is nullptr an empty string 
			 will be created.
			\param size    size of the buffer without the null-termination character. If <count> is 0 an empty 
			string will be created.
		*/
		StringUTF8(const void* buffer, uintMem size);
		/*
			Constructs a UTF-8 string from two iterators, one denoting the beginning and the other the end. The 
			string doesn't need to be null-terminated, but can be

			\param beign   the begin iterator of the string that will be copied
			\param beign   the end iterator of the string that will be copied
		*/
		StringUTF8(StringViewUTF8Iterator begin, StringViewUTF8Iterator end);
		/*
			Constructs a UTF-8 string view from a character array. The string doesn't need to be null-terminated,
			but can be

			\param arr   a character array
		*/
		template<uintMem C>
		StringUTF8(const char(&arr)[C]);
		~StringUTF8();

		/*
			Clears the string view
		*/
		void Clear();
		/*
			Resizes the string so that it has 'newCharacterCount' of caharacters. If the newn umber of
			characters is greater than the current one the new characters will be 'fillcharacter'.

			\param newCharacterCount     the new count of characters
			\param fillCharacter         the character with which to fill excess characters
			\returns Reference to string string
		*/
		StringUTF8& Resize(uintMem newCharacterCount, UnicodeChar fillCharacter = ' ');
		/*
			Creates a new UTF-8 string view from the characters of this string.

			\param begin - the index of the character from which to start the new string. Note this doesn't
			count bytes, it counts UTF-8 characters. If the index is negative then the first character of will 
			the new string will be at index (CharacterCount() - |index|)
			\param count - the number of character to use for the new string. Note this doesn't count bytes, 
			it counts UTF-8 characters. If the count is negative the final result string will have 
			(CharacterCount() - |count|) characters. If count is 0 a empty string view is returned
			\returns A new UTF8 string view
		*/
		StringViewUTF8 SubString(intMem begin, intMem count) const;
		/*
			Creates a new UTF-8 string view from the characters of this string.

			\param begin - iterator pointing to the character from which to start the new string.
			\param count - the number of character to use for the new string. Note this doesn't count bytes,
			it counts UTF-8 characters. If the count is negative the final result string will have
			(CharacterCount() - |count|) characters. If count is 0 a empty string view is returned
			\returns A new UTF8 string view
		*/
		StringViewUTF8 SubString(StringViewUTF8Iterator begin, intMem count) const;
		/*
			Inserts a string at the given index
		
			\param index   index of the character before which the string will be inserted. If the index is
			equal to the number of characters then the string will be appended to this string. If the index 
			is negative then the string will be inserted at index (CharacterCount() - |index|)
			\param string  the string to be inserted
		*/
		void Insert(intMem index, StringViewUTF8 string);
		/*
			Inserts a string at the given iterator

			\param iterator   iterator pointing to the character before which the string will be inserted.
			\param string  the string to be inserted
		*/
		void Insert(StringViewUTF8Iterator iterator, StringViewUTF8 string);
		/*
			Erases a sub string from this string starting at index 'begin' and including 'count' characters
			\param begin   index of the first character to be erased. Note this doesn't count bytes, it counts 
			UTF-8 characters. If the index is negative then the string will be erased starting at index 
			(CharacterCount() - |begin|)
			\param count   the count of characters to be erased. Note this doesn't count bytes, it counts UTF-8 
			characters. If the count is negative (CharacterCount() - |count|) of characters will be erased. If 
			count is 0 nothing will be changed
		*/
		void EraseSubString(intMem begin, uintMem count);
		/*
			Erases a sub string from this string starting at character pointed by 'begin' and including 
			'count' characters
			\param begin      iterator pointing to the first character to be erased			
			\param count      the count of characters to be erased. Note this doesn't count bytes, it counts UTF-8
			characters. If the count is negative (CharacterCount() - |count|) of characters will be erased. If
			count is 0 nothing will be changed
		*/
		void EraseSubString(StringViewUTF8Iterator begin, uintMem count);
		/*
			Erases a sub string from this string starting at character pointed by 'iterator' until
			'count' characters
			\param begin      iterator pointing to the first character to be erased
			\param end        iterator pointing to the character after the last one to be erased
		*/
		void EraseSubString(StringViewUTF8Iterator begin, StringViewUTF8Iterator end);

		/*
			\returns True if this string is empty, false otherwise
		*/
		inline bool Empty() const { return bufferSize == 0; }
		/*
			\returns The hash of this string
		*/
		uint32 Hash() const;
		/*
			Returns the pointer to the characters in this string. The buffer contains a null-terminating 
			character
		*/
		inline const void* Buffer() const { return buffer; }
		/*
			Returns the amount of bytes taken up by the characters excluding the null-terminating character.
		*/
		inline uintMem BufferSize() const { return bufferSize; }
		/*
			Returns the count of UTF-8 characters in the string
		*/
		inline uintMem CharacterCount() const { return characterCount; }

		/*
			Returns the first character in the string. The string mustn't be empty
		*/
		UnicodeChar First() const;
		/*
			Returns the last character in the string. The string mustn't be empty
		*/
		UnicodeChar Last() const;
		/*
			Returns a iterator pointing to the first character in the string
		*/
		StringViewUTF8Iterator FirstIterator() const;
		/*
			Returns a iterator pointing to the last character in the string
		*/
		StringViewUTF8Iterator LastIterator() const;
		/*
			Returns a iterator pointing ahead of the beginning of the string
		*/
		StringViewUTF8Iterator AheadIterator() const;
		/*
			Returns a iterator pointing behind the end of the string
		*/
		StringViewUTF8Iterator BehindIterator() const;

		/*
			Returs true if the whole strings are equal, false otherwise
		*/
		bool operator==(const StringViewUTF8& s) const;
		/*
			Returs false if the whole strings are equal, true otherwise
		*/
		bool operator!=(const StringViewUTF8& s) const;

		/*
			Appends a UTF-8 string view to this string
		*/
		StringUTF8& operator+=(const StringViewUTF8&);
		/*
			Appends a string to this string
		*/
		StringUTF8& operator+=(const String&);
		/*
			Appends a string view to this string
		*/
		StringUTF8& operator+=(const StringView&);
		/*
			Appends a character to this string
		*/
		StringUTF8& operator+=(const char&);
		/*
			Appends an unicode character to this string
		*/
		StringUTF8& operator+=(const UnicodeChar&);
		/*
			Appends a character array to this string
		*/
		template<size_t C>
		StringUTF8& operator+=(const char(&arr)[C]);

		/*
			Copies an another UTF-8 string to this one

			\returns Reference to this string
		*/
		StringUTF8& operator=(const StringUTF8& other);
		/*
			Moves an another UTF-8 string into this one

			\returns Reference to this string
		*/
		StringUTF8& operator=(StringUTF8&& other) noexcept;
		/*
			Copies a UTF-8 string view to this string

			\returns Reference to this string
		*/
		StringUTF8& operator=(const StringViewUTF8& other);
		/*
			Copies a string to this string

			\returns Reference to this string
		*/
		StringUTF8& operator=(const String& other);
		/*
			Copies a string view to this string

			\returns Reference to this string
		*/
		StringUTF8& operator=(const StringView& other);
		/*
			Copies an character array to this string

			\returns Reference to this string
		*/
		template<size_t C>
		StringUTF8& operator=(const char(&arr)[C]);

		friend BLAZE_CORE_API StringUTF8 operator+(const StringViewUTF8& left, const StringViewUTF8& right);
	private:
		void* buffer;		
		uintMem bufferSize;
		uintMem characterCount;
	};

	BLAZE_CORE_API StringViewUTF8Iterator begin(const StringUTF8& string);
	BLAZE_CORE_API StringViewUTF8Iterator end(const StringUTF8& string);

	BLAZE_CORE_API StringUTF8 operator+(const StringViewUTF8& left, const StringViewUTF8& right);
	BLAZE_CORE_API StringUTF8 operator+(const StringViewUTF8& left, const StringUTF8& right);
	BLAZE_CORE_API StringUTF8 operator+(const StringViewUTF8& left, const StringView& right);
	BLAZE_CORE_API StringUTF8 operator+(const StringViewUTF8& left, const String& right);
	BLAZE_CORE_API StringUTF8 operator+(const StringViewUTF8& left, const char& right);
	BLAZE_CORE_API StringUTF8 operator+(const StringViewUTF8& left, const UnicodeChar& right);
	template<uintMem C> StringUTF8 operator+(const StringViewUTF8& left, const char(&right)[C])
	{
		return left + StringViewUTF8(right);
	}
	BLAZE_CORE_API StringUTF8 operator+(const StringUTF8& left, const StringViewUTF8& right);
	BLAZE_CORE_API StringUTF8 operator+(const StringView& left, const StringViewUTF8& right);
	BLAZE_CORE_API StringUTF8 operator+(const String& left, const StringViewUTF8& right);
	BLAZE_CORE_API StringUTF8 operator+(const char& left, const StringViewUTF8& right);
	BLAZE_CORE_API StringUTF8 operator+(const UnicodeChar& left, const StringViewUTF8& right);
	template<uintMem C> StringUTF8 operator+(const char(&left)[C], const StringViewUTF8& right)
	{
		return StringViewUTF8(left) + right;
	}
	
	BLAZE_CORE_API StringUTF8 operator+(const StringUTF8& left, const StringUTF8& right);
	BLAZE_CORE_API StringUTF8 operator+(const StringUTF8& left, const StringView& right);
	BLAZE_CORE_API StringUTF8 operator+(const StringUTF8& left, const String& right);
	BLAZE_CORE_API StringUTF8 operator+(const StringUTF8& left, const char& right);
	BLAZE_CORE_API StringUTF8 operator+(const StringUTF8& left, const UnicodeChar& right);
	template<uintMem C> StringUTF8 operator+(const StringUTF8& left, const char(&right)[C])
	{
		return StringViewUTF8(left) + StringViewUTF8(right);
	}
	BLAZE_CORE_API StringUTF8 operator+(const StringView& left, const StringUTF8& right);
	BLAZE_CORE_API StringUTF8 operator+(const String& left, const StringUTF8& right);
	BLAZE_CORE_API StringUTF8 operator+(const char& left, const StringUTF8& right);
	BLAZE_CORE_API StringUTF8 operator+(const UnicodeChar& left, const StringUTF8& right);
	template<uintMem C> StringUTF8 operator+(const char(&left)[C], const StringUTF8& right)
	{
		return StringViewUTF8(left) + StringViewUTF8(right);
	}
	template<uintMem C>
	inline StringUTF8::StringUTF8(const char(&arr)[C])
		: StringUTF8(StringViewUTF8(arr))
	{
	}
	template<size_t C>
	inline StringUTF8& StringUTF8::operator+=(const char(&arr)[C])
	{
		return *this = StringViewUTF8(*this) + StringViewUTF8(arr);
	}
	template<size_t C>
	inline StringUTF8& StringUTF8::operator=(const char(&arr)[C])
	{
		return operator=(StringViewUTF8(arr));
	}
}

#include <string_view>

namespace std
{
	template <>
	struct hash<Blaze::StringUTF8>
	{
		inline std::size_t operator()(const Blaze::StringUTF8& k) const;
	};
	inline std::size_t hash<Blaze::StringUTF8>::operator()(const Blaze::StringUTF8& k) const
	{
		return hash<u8string_view>()(std::u8string_view((const char8_t*)k.Buffer(), k.BufferSize() - 1));
	}	
}