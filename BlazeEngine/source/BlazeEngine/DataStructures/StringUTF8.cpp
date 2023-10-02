#include "pch.h"
#include "BlazeEngine/DataStructures/StringUTF8.h"
#include "BlazeEngine/DataStructures/StringViewUTF8.h"

namespace Blaze
{
	StringUTF8::Iterator::Iterator()
		: ptr(nullptr)
	{
	}
	StringUTF8::Iterator::Iterator(const void* ptr, uint size)
		: ptr(ptr), value(ptr, size)
	{
	}
	StringUTF8::Iterator::Iterator(const Iterator& i)
		: ptr(i.ptr), value(i.value)
	{
	}
	UnicodeChar StringUTF8::Iterator::ToUnicode() const
	{		
 		return value;
	}
	const void* StringUTF8::Iterator::Ptr() const
	{
		return ptr;
	}
	StringUTF8::Iterator StringUTF8::Iterator::operator++()
	{
		Iterator old = *this;
		if (((*(uint8*)ptr) >> 7) == 0)
			ptr = (byte*)ptr + 1;
		else if (((*(uint8*)ptr) >> 5) == 0b110)
			ptr = (byte*)ptr + 2;
		else if (((*(uint8*)ptr) >> 4) == 0b1110)
			ptr = (byte*)ptr + 3;
		else if (((*(uint8*)ptr) >> 3) == 0b11110)
			ptr = (byte*)ptr + 4;
		return old;
	}
	StringUTF8::Iterator StringUTF8::Iterator::operator--()
	{
		Iterator old = *this;		
		if ((*((byte*)ptr - 1) & 0b10000000) == 0)
			ptr = (byte*)ptr - 1;
		else if ((*((byte*)ptr - 2) & 0b10000000) == 0)
			ptr = (byte*)ptr - 2;
		else if ((*((byte*)ptr - 3) & 0b10000000) == 0)
			ptr = (byte*)ptr - 3;
		else if ((*((byte*)ptr - 4) & 0b10000000) == 0)
			ptr = (byte*)ptr - 4;		
		return old;
	}
	StringUTF8::Iterator& StringUTF8::Iterator::operator++(int)
	{
		Iterator old = *this;
		if (((*(uint8*)ptr) >> 7) == 0)
			ptr = (byte*)ptr + 1;
		else if (((*(uint8*)ptr) >> 5) == 0b110)
			ptr = (byte*)ptr + 2;
		else if (((*(uint8*)ptr) >> 4) == 0b1110)
			ptr = (byte*)ptr + 3;
		else if (((*(uint8*)ptr) >> 3) == 0b11110)
			ptr = (byte*)ptr + 4;
		return *this;
	}
	StringUTF8::Iterator& StringUTF8::Iterator::operator--(int)
	{
		Iterator old = *this;
		if ((*((byte*)ptr - 1) & 0b10000000) == 0)
			ptr = (byte*)ptr - 1;
		else if ((*((byte*)ptr - 2) & 0b10000000) == 0)
			ptr = (byte*)ptr - 2;
		else if ((*((byte*)ptr - 3) & 0b10000000) == 0)
			ptr = (byte*)ptr - 3;
		else if ((*((byte*)ptr - 4) & 0b10000000) == 0)
			ptr = (byte*)ptr - 4;
		return *this;
	}
	bool StringUTF8::Iterator::operator<(const Iterator& i) const
	{
		return ptr < i.ptr;
	}
	bool StringUTF8::Iterator::operator>(const Iterator& i) const
	{
		return ptr > i.ptr;
	}
	bool StringUTF8::Iterator::operator<=(const Iterator& i) const
	{
		return ptr <= i.ptr;
	}
	bool StringUTF8::Iterator::operator>=(const Iterator& i) const
	{
		return ptr >= i.ptr;
	}
	bool StringUTF8::Iterator::operator==(const Iterator& i) const
	{
		return ptr == i.ptr;
	}
	bool StringUTF8::Iterator::operator!=(const Iterator& i) const
	{
		return ptr != i.ptr;
	}
	StringUTF8::Iterator& StringUTF8::Iterator::operator=(const Iterator& i)
	{
		ptr = i.ptr;
		value = i.value;
		return *this;
	}

	StringUTF8::StringUTF8()
		: buffer(nullptr), bufferSize(0), characterCount(0)
	{
	}

	StringUTF8::StringUTF8(const StringUTF8& s)
		: buffer(nullptr), bufferSize(s.bufferSize), characterCount(s.characterCount)
	{
		if (s.bufferSize > 0)
		{
			buffer = Memory::Allocate(bufferSize);
			memcpy(buffer, s.buffer, bufferSize);
		}
		else
			buffer = nullptr;
	}

	StringUTF8::StringUTF8(StringUTF8&& s) noexcept
		: buffer(s.buffer), bufferSize(s.bufferSize), characterCount(s.characterCount)
	{
		s.buffer = nullptr;
		s.bufferSize = 0;
		s.characterCount = 0;
	}

	StringUTF8::StringUTF8(const void* buffer, uint bufferSize)
		: buffer(nullptr), bufferSize(bufferSize + 1), characterCount(0)
	{
		if (bufferSize == 0 || buffer == nullptr)
		{
			this->bufferSize = 0;
		}
		else
		{
			this->buffer = Memory::Allocate(bufferSize + 1);
			memcpy(this->buffer, buffer, bufferSize);

			((char*)this->buffer)[bufferSize] = '\0';

			for (auto it = begin(); it != end(); ++it, ++characterCount);
		}
	}

	StringUTF8::StringUTF8(const char* ptr)
	{
		if (ptr != nullptr && (characterCount = strlen(ptr)) != 0)
		{			
			bufferSize = characterCount + 1;
			buffer = Memory::Allocate(bufferSize);
			memcpy(buffer, ptr, bufferSize);
		}
		else
		{
			buffer = nullptr;
			bufferSize = 0;
			characterCount = 0;
		}
	}

	StringUTF8::StringUTF8(const char* ptr, uint size)
		: buffer(nullptr), bufferSize(size + 1), characterCount(size)
	{
		if (ptr != nullptr && size != 0)
		{
			buffer = Memory::Allocate(bufferSize);
			memcpy(buffer, ptr, size);
			((char*)buffer)[size] = '\0';
		}
		else
		{
			buffer = nullptr;
			bufferSize = 0;
			characterCount = 0;
		}
	}

	StringUTF8::StringUTF8(const StringViewUTF8& s)
		: buffer(nullptr), bufferSize(s.BufferSize()), characterCount(s.CharacterCount())
	{
		if (s.Buffer() != nullptr)
		{
			buffer = Memory::Allocate(bufferSize);
			memcpy(buffer, s.Buffer(), bufferSize);
		}		
	}

	StringUTF8::StringUTF8(const StringView& s)
		: buffer(nullptr), bufferSize(s.Size() + 1), characterCount(s.Size())
	{
		if (s.Ptr() != nullptr)
		{
			buffer = Memory::Allocate(bufferSize);
			memcpy(buffer, s.Ptr(), bufferSize);
		}		
		else
		{
			bufferSize = 0;
			characterCount = 0;
		}
	}

	StringUTF8::StringUTF8(const String& s)
		: buffer(nullptr), bufferSize(s.Size() + 1), characterCount(s.Size())
	{
		if (s.Ptr() != nullptr)
		{
			buffer = Memory::Allocate(bufferSize);
			memcpy(buffer, s.Ptr(), bufferSize);
		}
		else
		{
			bufferSize = 0;
			characterCount = 0;
		}
	}

	StringUTF8::~StringUTF8()
	{
		Memory::Free(buffer);
	}

	void StringUTF8::Clear()
	{
		Memory::Free(buffer);
		buffer = nullptr;
		bufferSize = 0;
		characterCount = 0;
	}	

	StringUTF8::Iterator StringUTF8::begin() const
	{
		return Iterator(buffer, bufferSize - 1);
	}
	StringUTF8::Iterator StringUTF8::end() const
	{
		if (buffer == nullptr)
			return Iterator(nullptr, 0);
		return Iterator((byte*)buffer + bufferSize - 1, 0);
	}

	StringUTF8 StringUTF8::SubString(uint start, uint size) const
	{
		auto b = begin();
		for (uint i = 0; i != start; ++i, ++b);
		auto e = b;
		for (uint i = 0; i != size; ++i, ++e);

		return StringUTF8(b.ptr, (char*)e.ptr - (char*)b.ptr);
	}

	StringUTF8& StringUTF8::Resize(uint newCharacterCount, UnicodeChar fill)
	{ 
		size_t fillSize = fill.UTF8Size();
		void* old = buffer;

		size_t charCount = CharacterCount();		

		if (charCount > newCharacterCount)
		{
			size_t count = 0;
			auto it = begin();
			while(count != newCharacterCount)
			{				
				++count;
				++it;
			}

			size_t newBufferSize = (char*)it.ptr - (char*)buffer + 1;

			buffer = Memory::Allocate(newBufferSize);
			memcpy(buffer, old, newBufferSize - 1);
			Memory::Free(old);
			bufferSize = newBufferSize;
			characterCount = newCharacterCount;
			*((char*)buffer + bufferSize - 1) = '\0';
		}
		else
		{
			size_t newBufferSize = (bufferSize == 0 ? 1 : bufferSize) + fillSize * (newCharacterCount - charCount);
			buffer = Memory::Allocate(newBufferSize);			

			memcpy(buffer, old, newBufferSize - 1);

			uint bufferOffset = 0;
			for (int i = 0; i < newCharacterCount - charCount; ++i)
			{				
				fill.ToUTF8((char*)buffer + bufferSize - 1 + bufferOffset, newBufferSize - bufferSize + 1 - bufferOffset);
				bufferOffset += fillSize;
			}

			Memory::Free(old);
			bufferSize = newBufferSize;
			characterCount = newCharacterCount;
			*((char*)buffer + bufferSize - 1) = '\0';
		}	

		return *this;
	}

	StringUTF8& StringUTF8::operator=(const StringViewUTF8& s)
	{
		Memory::Free(buffer);

		if (s.Buffer() != nullptr)
		{
			bufferSize = s.BufferSize();
			characterCount = s.CharacterCount();
			buffer = Memory::Allocate(bufferSize);
			memcpy(buffer, s.Buffer(), bufferSize);
		}
		else
		{
			buffer = nullptr;
			bufferSize = 0;
			characterCount = 0;
		}
		return *this;
	}

	StringUTF8& StringUTF8::operator=(const StringUTF8& s)
	{ 
		Memory::Free(buffer);

		if (s.Buffer() != nullptr)
		{
			bufferSize = s.BufferSize();
			characterCount = s.CharacterCount();
			buffer = Memory::Allocate(bufferSize);
			memcpy(buffer, s.Buffer(), bufferSize);
		}
		else
		{
			buffer = nullptr;
			bufferSize = 0;
			characterCount = 0;
		}
		return *this;
	}

	StringUTF8& StringUTF8::operator=(StringUTF8&& s) noexcept
	{
		Memory::Free(buffer);
		bufferSize = s.bufferSize;
		characterCount = s.characterCount;
		buffer = s.buffer;
		s.buffer = nullptr;
		s.bufferSize = 0;
		s.characterCount = 0;
		return *this;
	}

	StringUTF8& StringUTF8::operator=(const StringView& s) noexcept
	{
		Memory::Free(buffer);

		if (s.Ptr() != nullptr)
		{
			bufferSize = s.Size() + 1;
			characterCount = s.Size();
			buffer = Memory::Allocate(bufferSize);
			memcpy(buffer, s.Ptr(), bufferSize);
		}
		else
		{
			buffer = nullptr;
			bufferSize = 0;
			characterCount = 0;
		}
		return *this;
	}

	StringUTF8& StringUTF8::operator=(const String& s) noexcept
	{
		Memory::Free(buffer);

		if (s.Ptr() != nullptr)
		{
			bufferSize = s.Size() + 1;
			characterCount = s.Size();
			buffer = Memory::Allocate(bufferSize);
			memcpy(buffer, s.Ptr(), bufferSize);
		}
		else
		{
			buffer = nullptr;
			bufferSize = 0;
			characterCount = 0;
		}
		return *this;
	}

	bool StringUTF8::operator==(const StringViewUTF8& s) const
	{
		if (buffer == nullptr && s.Buffer() == nullptr)
			return true;
		if (buffer == nullptr || s.Buffer() == nullptr || bufferSize != s.BufferSize())
			return false;		

		return memcmp(buffer, s.Buffer(), bufferSize) == 0;
	}

	bool StringUTF8::operator!=(const StringViewUTF8& s) const
	{
		return !(*this == s);
	}
	StringUTF8& StringUTF8::operator+=(const StringViewUTF8& other)
	{
		return *this = *this + other;		
	}

	StringUTF8& StringUTF8::operator+=(const char& ch)
	{
		return *this = *this + StringViewUTF8(&ch, 1);
	}


	StringUTF8 operator+(const StringViewUTF8& left, const StringViewUTF8& right)
	{
		if (left.BufferSize() == 0)
			return right;
		if (right.BufferSize() == 0)
			return left;
		size_t lSize = left.BufferSize();
		size_t rSize = right.BufferSize();
		StringUTF8 out;
		out.bufferSize = (lSize == 0 ? 1 : lSize) + (rSize == 0 ? 1 : rSize) - 1;
		out.buffer = Memory::Allocate(out.bufferSize);
		out.characterCount = left.CharacterCount() + right.CharacterCount();
		memcpy(out.buffer, left.Buffer(), lSize - 1);
		memcpy((byte*)out.buffer + lSize - 1, right.Buffer(), rSize);
		return out;
	}
	StringUTF8 operator+(const StringViewUTF8& left, const StringView& right)
	{
		return left + StringViewUTF8(right);
	}

	StringUTF8 operator+(const StringView& left, const StringViewUTF8& right)
	{
		return StringViewUTF8(left) + right;
	}

	StringUTF8 operator+(const StringViewUTF8& left, const char* right)
	{
		return left + StringViewUTF8(right);
	}

	StringUTF8 operator+(const char* left, const StringViewUTF8& right)
	{
		return StringViewUTF8(left) + right;
	}

}