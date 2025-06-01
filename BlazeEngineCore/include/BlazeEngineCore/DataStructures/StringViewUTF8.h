#pragma once
#include "BlazeEngineCore/BlazeEngineCoreDefines.h"
#include "BlazeEngineCore/DataStructures/UnicodeChar.h"

namespace Blaze
{	
	class StringUTF8;
	class StringView;	
	class String;

	class StringViewUTF8Iterator
	{
	public:
		/*
			Constructs a null string view UTF-8 iterator
		*/
		constexpr StringViewUTF8Iterator();
		/*
			Copies a string view UTF-8 iterator
		*/
		constexpr StringViewUTF8Iterator(const StringViewUTF8Iterator&);
		/*
			Constructs a string view UTF-8 iterator from a pointer to memory where a UTF-8 charecter sequence is
			stored

			\param buffer - the buffer where the UTF-8 sequence is be stored			
		*/
		constexpr StringViewUTF8Iterator(const void* buffer);

		/*
			\returns True if the iterator is null, false otherwise
		*/
		constexpr bool IsNull() const;
		/*
			Returns the unicode character corresponding to the UTF-8 character sequence pointer by the iterator.
			The iterator must be pointing to a valid UTF-8 character sequence
		*/
		constexpr UnicodeChar ToUnicode() const;
		/*
			Returns the pointer to the memory where the UTF-8 character sequence pointed by this iterator is 
			stored. Returns nullptr if the iterator is null
		*/
		constexpr const void* Ptr() const;
		/*
			Returns the amount of bytes taken up by the UTF-8 character sequence pointed by this iterator. The 
			iterator must be pointing to a valid UTF-8 character sequence
		*/
		constexpr uintMem DetermineSize() const;

		/*
			Returns the unicode character corresponding to the UTF-8 character sequence pointer by the iterator.
			The iterator must be pointing to a valid UTF-8 character sequence
		*/
		constexpr UnicodeChar operator*() const;

		/*
			Pre-increments the iterator. The iterator must be pointing to a valid UTF-8 character sequence

			\returns A copy of the non incremented iterator
		*/
		constexpr StringViewUTF8Iterator& operator++();
		/*
			Post-increments the iterator. The iterator must be pointing to a valid UTF-8 character sequence

			\returns Reference to this iterator
		*/
		constexpr StringViewUTF8Iterator operator++(int);

		/*
			Decrements the iterator. Checks if the iterator is pointing at the begining of the string so it can
			become a valid 'ahead iterator' after it is decremented

			\param string    the string whose character this iterator is pointing to
			\returns Reference to this iterator
		*/
		constexpr StringViewUTF8Iterator& Decrement(const StringViewUTF8& string);
		/*
			Decrements the iterator. Checks if the iterator is pointing at the begining of the string so it can
			become a valid 'ahead iterator' after it is decremented

			\param firstCharacterIterator    iterator pointing to the beginning of the string
			\returns Reference to this iterator
		*/
		constexpr StringViewUTF8Iterator& Decrement(const StringViewUTF8Iterator& firstCharacterIterator);

		/*			
			\returns True if the iterators are pointing to the same memory
		*/
		constexpr bool operator==(const StringViewUTF8Iterator& other) const;
		/*
			\returns True the iterators are not pointing to the same memory
		*/
		constexpr bool operator!=(const StringViewUTF8Iterator& other) const;
		/*
			\returns True if this iterators pointer is smaller than the other iterators pointer
		*/
		constexpr bool operator<(const StringViewUTF8Iterator& other) const;
		/*
			\returns True if this iterators pointer is larger than the other iterators pointer
		*/
		constexpr bool operator>(const StringViewUTF8Iterator& other) const;
		/*
			\returns True if this iterators pointer is smaller or equal as the other iterators pointer
		*/
		constexpr bool operator<=(const StringViewUTF8Iterator& other) const;
		/*
			\returns True if this iterators pointer is bigger or equal as the other iterators pointer
		*/
		constexpr bool operator>=(const StringViewUTF8Iterator& other) const;

		/*
			Copies the other iterator to this one

			\returns the reference to this iterator
		*/
		constexpr StringViewUTF8Iterator& operator=(const StringViewUTF8Iterator&);
	private:
		const void* ptr;
	};

	class StringViewUTF8
	{
	public:
		/*
			Constructs an empty UTF-8 string view
		*/
		constexpr StringViewUTF8();
		/*
			Copies an UTF-8 string view
		*/
		constexpr StringViewUTF8(const StringViewUTF8& other);
		/*
			Constructs UTF-8 string view from a UTF-8 string
		*/
		inline StringViewUTF8(const StringUTF8& other);
		/*
			Constructs UTF-8 string view from a string view
		*/
		constexpr StringViewUTF8(const StringView& other);		
		/*
			Constructs UTF-8 string view from a string
		*/
		inline StringViewUTF8(const String& other);				
		/*
			Constructs a UTF-8 string view from a buffer which holds a UTF-8 string. A normal string is also a 
			valid UTF-8 string. The string doesn't need to be null-terminated, but can be

			\param ptr     pointer to a buffer holding UTF-8 characters. If <ptr> is nullptr an empty string 
			view will be created.
			\param size    size of the buffer without the null-termination character. If <count> is 0 an empty 
			string view will be created.
		*/
		constexpr StringViewUTF8(const void* ptr, uintMem size);
		/*
			Constructs a UTF-8 string view from two iterators, one denoting the beginning and the other the end.
			The string doesn't need to be null-terminated, but can be. If either of the iterators is null an
			empty string is created

			\param beign   the begin iterator of the string will be copied
			\param beign   the end iterator of the string will be copied
		*/
		constexpr StringViewUTF8(StringViewUTF8Iterator begin, StringViewUTF8Iterator end);
		/*
			Constructs a UTF-8 string view from a character array. The string doesn't need to be null-terminated, 
			but can be

			\param arr   a character array
		*/
		template<uintMem C>
		constexpr StringViewUTF8(const char(&arr)[C]);

		/*
			Clears the string view
		*/
		constexpr void Clear();
		/*
			Creates a new UTF-8 string view from the characters of this one. 

			\param start - the index of the character from which to start the new string. Note this doesn't
			count bytes, it counts UTF-8 characters.
			\param count - the number of character to use for the new string. Note this doesn't count bytes, 
			it counts UTF-8 characters. If the count is negative the final result string will have 
			(CharacterCount() - |count|) characters. If count is 0 a empty string view is returned
			\returns A new UTF8 string view
		*/
		constexpr StringViewUTF8 SubString(uintMem start, intMem count) const;
		/*
			Creates a new UTF-8 string view from the characters of this string.

			\param begin - iterator pointing to the character from which to start the new string.
			\param count - the number of character to use for the new string. Note this doesn't count bytes,
			it counts UTF-8 characters. If the count is negative the final result string will have
			(CharacterCount() - |count|) characters. If count is 0 a empty string view is returned
			\returns A new UTF8 string view
		*/
		constexpr StringViewUTF8 SubString(StringViewUTF8Iterator begin, intMem count) const;

		/*
			\returns True if this string view is empty, false otherwise
		*/
		constexpr bool Empty() const;
		/*
			\returns The hash of this string view
		*/
		inline uint32 Hash() const;
		/*
			Returns the pointer to the characters in this string view. Note the string view might not have a
			null-terminating character. Returns nullptr if the string is empty
		*/
		constexpr const void* Buffer() const;
		/*
			Returns the amount of bytes taken up by the characters excluding the null-terminating character if 
			the string has one			
		*/
		constexpr uintMem BufferSize() const;
		/*
			Returns the count of UTF-8 characters in the string
		*/
		constexpr uintMem CharacterCount() const;

		/*
			Returns the first character in the string. The string mustn't be empty
		*/
		constexpr UnicodeChar First() const;
		/*
			Returns the last character in the string. The string mustn't be empty
		*/
		constexpr UnicodeChar Last() const;
		/*
			Returns a iterator pointing to the first character in the string
		*/
		constexpr StringViewUTF8Iterator FirstIterator() const;						
		/*
			Returns a iterator pointing to the last character in the string
		*/
		constexpr StringViewUTF8Iterator LastIterator() const;		
		/*
			Returns a iterator pointing ahead of the beginning of the string
		*/
		constexpr StringViewUTF8Iterator AheadIterator() const;
		/*
			Returns a iterator pointing behind the end of the string
		*/
		constexpr StringViewUTF8Iterator BehindIterator() const;

		/*
			Returs true if the whole strings are equal, false otherwise
		*/
		constexpr bool operator==(const StringViewUTF8& other) const;
		/*
			Returs false if the whole strings are equal, true otherwise
		*/
		constexpr bool operator!=(const StringViewUTF8& other) const;

		/*
			Copies an another UTF-8 string view to this one

			\returns Reference to this string
		*/
		constexpr StringViewUTF8& operator= (const StringViewUTF8& other);
		/*
			Copies an another UTF-8 string to this one

			\returns Reference to this string
		*/
		inline StringViewUTF8& operator= (const StringUTF8& other);
		/*
			Copies an another string view to this one

			\returns Reference to this string
		*/
		constexpr StringViewUTF8& operator= (const StringView& other);
		/*
			Copies an another string to this one

			\returns Reference to this string
		*/
		inline StringViewUTF8& operator= (const String& other);
		/*
			Copies an character array to this string

			\returns Reference to this string
		*/
		template<size_t C>
		constexpr StringViewUTF8& operator=(const char(&arr)[C]);
	private:
		const void* buffer;		
		//Stores the size of the buffer being referenced. It doesn't include the null-terminating character at
		//the end
		uintMem bufferSize;
		uintMem characterCount;		

		constexpr void CalculateCharacterCount();
	};

	constexpr StringViewUTF8Iterator begin(const StringViewUTF8& string);
	constexpr StringViewUTF8Iterator end(const StringViewUTF8& string);		
}

#include <string_view>

namespace std
{
	template <>
	struct hash<Blaze::StringViewUTF8>
	{
		inline size_t operator()(const Blaze::StringViewUTF8& k) const;
	};
	inline size_t hash< Blaze::StringViewUTF8>::operator()(const Blaze::StringViewUTF8& k) const
	{
		return hash<u8string_view>()(u8string_view((const char8_t*)k.Buffer(), k.BufferSize() - 1));
	}	
}