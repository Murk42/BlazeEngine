#pragma once
#include "BlazeEngine/Core/EngineCore.h"
#include "BlazeEngine/DataStructures/Common.h"
#include "BlazeEngine/DataStructures/UnicodeChar.h"

namespace Blaze
{
	class StringUTF8;
	class StringView;	

	class BLAZE_API StringViewUTF8
	{
		const void* buffer;		
		size_t bufferSize;
		size_t characterCount;
	public:
		class BLAZE_API Iterator
		{
			const void* ptr;
			Iterator(const void* ptr);
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

			Iterator& operator=(const Iterator&);

			friend class StringViewUTF8;
		};
		StringViewUTF8();
		StringViewUTF8(const StringViewUTF8&);

		StringViewUTF8(const char* ptr);
		StringViewUTF8(const char* ptr, size_t size);		
		StringViewUTF8(const StringUTF8&);
		StringViewUTF8(const StringView&);		

		const void* Buffer() const { return buffer; }
		size_t BufferSize() const { return bufferSize; }
		size_t CharacterCount() const { return characterCount; }

		Iterator begin() const;
		Iterator end() const;

		StringViewUTF8& operator= (const StringViewUTF8&);

		bool operator==(const StringViewUTF8& s) const;
		bool operator!=(const StringViewUTF8& s) const;
	};
}
