#pragma once
#include "BlazeEngine/DataStructures/UnicodeChar.h"

namespace Blaze
{
	class StringUTF8;
	class StringView;	
	class String;

	class BLAZE_API StringViewUTF8
	{
		const void* buffer;		
		uintMem bufferSize;
		uintMem characterCount;
	public:
		class BLAZE_API Iterator
		{
			const void* ptr;
			UnicodeChar value;
			Iterator(const void* ptr, uintMem size);
		public:
			Iterator();
			Iterator(const Iterator&);

			UnicodeChar ToUnicode() const;

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

			UnicodeChar operator*() const;
			Iterator& operator=(const Iterator&);

			friend class StringViewUTF8;
		};
		StringViewUTF8();
		StringViewUTF8(const StringViewUTF8&);

		StringViewUTF8(const char* ptr);
		StringViewUTF8(const char* ptr, uintMem size);
		StringViewUTF8(const StringUTF8&);
		StringViewUTF8(const StringView&);		
		StringViewUTF8(const String&);

		const void* Buffer() const { return buffer; }
		uintMem BufferSize() const { return bufferSize; }
		uintMem CharacterCount() const { return characterCount; }

		Iterator begin() const;
		Iterator end() const;

		StringViewUTF8& operator= (const StringViewUTF8&);

		bool operator==(const StringViewUTF8& s) const;
		bool operator!=(const StringViewUTF8& s) const;
	};
}
