#pragma once
#include "BlazeEngine/DataStructures/UnicodeChar.h"

namespace Blaze
{	
	class StringViewUTF8;		
	class StringView;	
	class String;
	
	class BLAZE_API StringUTF8
	{
		void* buffer;	
		uintMem bufferSize;
		uintMem characterCount;		
	public:
		class BLAZE_API Iterator
		{
			const void* ptr;
			UnicodeChar value;

			Iterator(const void* value, uintMem size);
		public:
			Iterator();
			Iterator(const Iterator&);

			UnicodeChar ToUnicode() const;
			const void* Ptr() const;

			Iterator operator++();
			Iterator operator--();
			Iterator& operator++(int);
			Iterator& operator--(int);

			bool operator<(const Iterator&) const;
			bool operator>(const Iterator&) const;
			bool operator<=(const Iterator&) const;
			bool operator>=(const Iterator&) const;
			bool operator==(const Iterator&) const;
			bool operator!=(const Iterator&) const;

			Iterator& operator=(const Iterator&);

			friend class StringUTF8;
		};

		StringUTF8();		
		StringUTF8(const StringUTF8& s);
		StringUTF8(StringUTF8&& s) noexcept;
				
		StringUTF8(const void* buffer, uintMem bufferSize);
		StringUTF8(const char* ptr);
		StringUTF8(const char* ptr, uintMem size);
		StringUTF8(const StringViewUTF8&);
		explicit StringUTF8(const StringView&);		
		explicit StringUTF8(const String&);

		~StringUTF8();			
		
		void Clear();
		
		const void* Buffer() const { return buffer; }
		uintMem BufferSize() const { return bufferSize; }
		uintMem CharacterCount() const { return characterCount; }
		
		Iterator begin() const;		
		Iterator end() const;

		StringUTF8 SubString(uintMem start, uintMem size) const;
		StringUTF8& Resize(uintMem newCharacterCount, UnicodeChar fill);

		StringUTF8& operator= (const StringViewUTF8& s);
		StringUTF8& operator= (const StringUTF8& s);
		StringUTF8& operator= (StringUTF8&& s) noexcept;
		StringUTF8& operator= (const StringView& s) noexcept;
		StringUTF8& operator= (const String& s) noexcept;

		bool operator==(const StringViewUTF8& s) const;
		bool operator!=(const StringViewUTF8& s) const;		

		StringUTF8& operator+= (const StringViewUTF8&);
		StringUTF8& operator+= (const char&);
		
		friend BLAZE_API StringUTF8 operator+(const StringViewUTF8& left, const StringViewUTF8& right);				
	};	
	
	BLAZE_API StringUTF8 operator+(const StringViewUTF8& left, const StringViewUTF8& right);
	BLAZE_API StringUTF8 operator+(const StringViewUTF8& left, const StringView& right);
	BLAZE_API StringUTF8 operator+(const StringView& left, const StringViewUTF8& right);
	BLAZE_API StringUTF8 operator+(const StringViewUTF8& left, const char* right);
	BLAZE_API StringUTF8 operator+(const char* left, const StringViewUTF8& right);
}