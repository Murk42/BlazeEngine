#include "pch.h"
#include "BlazeEngineCore/DataStructures/StringUTF8.h"
#include "BlazeEngineCore/DataStructures/StringViewUTF8.h"
#include "BlazeEngineCore/Memory/MemoryManager.h"

namespace Blaze
{
	using Allocator = DefaultAllocator;
	
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
	StringUTF8::StringUTF8(const StringViewUTF8& s)
		: buffer(nullptr), bufferSize(s.BufferSize()), characterCount(s.CharacterCount())
	{
		if (s.Buffer() != nullptr)
		{
			buffer = Memory::Allocate(bufferSize);
			memcpy(buffer, s.Buffer(), bufferSize);
		}		
	}
	StringUTF8::StringUTF8(const String& s)
		: buffer(nullptr), bufferSize(s.Count() + 1), characterCount(s.Count())
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
	StringUTF8::StringUTF8(const StringView& s)
		: buffer(nullptr), bufferSize(s.Count() + 1), characterCount(s.Count())
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
	StringUTF8::StringUTF8(const void* buffer, uintMem size)
		: buffer(nullptr), bufferSize(0), characterCount(0)
	{
		if (size != 0 && buffer != nullptr)		
		{
			bufferSize = size + 1;
			this->buffer = Memory::Allocate(bufferSize);
			memcpy(this->buffer, buffer, size);			
			((char*)this->buffer)[size] = '\0';
			for (auto it = FirstIterator(); it != BehindIterator(); ++it, ++characterCount);
		}
	}
	StringUTF8::StringUTF8(const char* ptr, uintMem count)
		: buffer(nullptr), bufferSize(0), characterCount(0)
	{
		if (ptr != nullptr && count != 0)
		{
			bufferSize = count + 1;
			characterCount = count;
			buffer = Memory::Allocate(bufferSize);
			memcpy(buffer, ptr, count);
			((char*)buffer)[count] = '\0';
		}				
	}
	StringUTF8::StringUTF8(StringViewUTF8Iterator begin, StringViewUTF8Iterator end)
		: StringUTF8(begin.ptr, (char*)end.ptr - (char*)begin.ptr)
	{
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
	StringUTF8 StringUTF8::SubString(uintMem start, uintMem count) const
	{
		auto b = FirstIterator();
		for (uintMem i = 0; i != start; ++i, ++b);
		auto e = b;
		for (uintMem i = 0; i != count; ++i, ++e);
		
		return StringUTF8(b.ptr, (char*)e.ptr - (char*)b.ptr);
	}
	StringUTF8& StringUTF8::Resize(uintMem newCharacterCount, UnicodeChar fill)
	{ 
		size_t fillSize = fill.UTF8Size();
		void* old = buffer;

		size_t charCount = CharacterCount();		

		if (charCount > newCharacterCount)
		{
			size_t count = 0;
			auto it = FirstIterator();
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

			uintMem bufferOffset = 0;
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
	void StringUTF8::InsertString(intMem index, StringViewUTF8 string)
	{		
		if (string.Empty())
			return;

		if (index < 0)		
			index = (intMem)characterCount + index;		

		if (index > (intMem)characterCount || index < 0)
		{
			Debug::Logger::LogError("Blaze Engine", "Trying to insert a string at the index that is outside of the the target string");
			return;
		}
		else if (index == (intMem)characterCount)
		{
			*this += string;
			return;
		}
		else if (index == 0)
		{
			*this = string + *this;
			return;
		}

		void* oldBuffer = buffer;
		uintMem oldBufferSize = bufferSize;

		bufferSize += string.BufferSize() - 1;
		buffer = Memory::Allocate(bufferSize);
		characterCount += string.CharacterCount();

		void* it1 = oldBuffer;
		void* it2 = it1;

		uintMem offset = 0, size = 0;		
		for (uint i = 0; i < index; ++i, it2 = Advance(it2));

		size = (byte*)it2 - (byte*)it1;		
		memcpy(buffer, oldBuffer, size);
		offset += size;
		
		memcpy((char*)buffer + offset, string.Buffer(), string.BufferSize() - 1);
		offset += string.BufferSize() - 1;
		
		size = oldBufferSize - ((byte*)it2 - (byte*)oldBuffer);
		memcpy((char*)buffer + offset, it2, size);		

		Memory::Free(oldBuffer);
	}
	void StringUTF8::EraseSubString(intMem index, uintMem count)
	{
		if (count == 0 || bufferSize == 0)
			return;

		if (index < 0)
			index = (intMem)characterCount + index;

		if (index < 0)
		{
			if ((uintMem)(-index) <= count)
				return;

			count += index;
			index = 0;
		}

		if (index >= (intMem)characterCount)
			return;

		if (index + count > characterCount)		
			count = characterCount - index;		

		if (index == 0)
		{
			if (count >= characterCount)
			{
				Clear();
				return;
			}
						
			*this = SubString(count, characterCount - count);
			return;			
		}

		if (index == characterCount - count)
		{
			*this = SubString(0, characterCount - count);
			return;
		}

		void* oldBuffer = buffer;
		uintMem oldBufferSize = bufferSize;
	
		bufferSize -= count;
		buffer = Memory::Allocate(bufferSize);
		characterCount -= count;

		void* it1 = oldBuffer;
		void* it2 = it1;		

		uintMem offset = 0, size = 0;
		for (uint i = 0; i < index; ++i, it2 = Advance(it2));

		size = (byte*)it2 - (byte*)it1;
		memcpy(buffer, oldBuffer, size);
		offset += size;
		
		for (uint i = 0; i < count; ++i, it2 = Advance(it2));		

		size = oldBufferSize - ((byte*)it2 - (byte*)oldBuffer);
		memcpy((char*)buffer + offset, it2, size);		

		Memory::Free(oldBuffer);
	}
	uint32 StringUTF8::Hash() const
	{
		return static_cast<uint32>(std::hash<StringUTF8>()(*this));
	}
	UnicodeChar StringUTF8::First() const
	{
#ifdef BLAZE_INVALID_ITERATOR_CHECK
		if (bufferSize == 0)
			Debug::Logger::LogFatal("Blaze Engine", "String is empty");
#endif
		return UnicodeChar(buffer, bufferSize);
	}
	StringViewUTF8Iterator StringUTF8::FirstIterator() const
	{
		if (buffer == nullptr)
			return StringViewUTF8Iterator(nullptr, 0);
		return StringViewUTF8Iterator(buffer, bufferSize - 1);
	}
	StringViewUTF8Iterator StringUTF8::BehindIterator() const
	{
		if (buffer == nullptr)
			return StringViewUTF8Iterator(nullptr, 0);
		return StringViewUTF8Iterator((byte*)buffer + bufferSize - 1, 0);
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
		return *this = StringViewUTF8(*this) + other;
	}
	StringUTF8& StringUTF8::operator+=(const String& other)
	{
		return *this = StringViewUTF8(*this) + StringViewUTF8(other);
	}
	StringUTF8& StringUTF8::operator+=(const StringView& other)
	{
		return *this = StringViewUTF8(*this) + StringViewUTF8(other);
	}
	StringUTF8& StringUTF8::operator+=(const char& ch)
	{
		return *this = *this + StringViewUTF8(&ch, 1);
	}
	StringUTF8& StringUTF8::operator+=(const UnicodeChar& ch)
	{
		char buffer[4];
		uintMem count = ch.ToUTF8(buffer, 4);
		return *this = *this + StringViewUTF8((const void*)buffer, count);
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
	StringUTF8& StringUTF8::operator=(const StringView& s)
	{
		Memory::Free(buffer);

		if (s.Ptr() != nullptr)
		{
			bufferSize = s.Count() + 1;
			characterCount = s.Count();
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
	StringUTF8& StringUTF8::operator=(const String& s)
	{
		Memory::Free(buffer);

		if (s.Ptr() != nullptr)
		{
			bufferSize = s.Count() + 1;
			characterCount = s.Count();
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
	void* StringUTF8::Advance(void* ptr)
	{
		if (((*(uint8*)ptr) >> 7) == 0)
			ptr = (byte*)ptr + 1;
		else if (((*(uint8*)ptr) >> 5) == 0b110)
			ptr = (byte*)ptr + 2;
		else if (((*(uint8*)ptr) >> 4) == 0b1110)
			ptr = (byte*)ptr + 3;
		else if (((*(uint8*)ptr) >> 3) == 0b11110)
			ptr = (byte*)ptr + 4;		
		return ptr;
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
	StringUTF8 operator+(const StringViewUTF8& left, const StringUTF8& right)
	{
		return left + StringViewUTF8(right);
	}
	StringUTF8 operator+(const StringViewUTF8& left, const StringView& right)
	{
		return left + StringViewUTF8(right);
	}
	StringUTF8 operator+(const StringViewUTF8& left, const String& right)
	{
		return left + StringViewUTF8(right);
	}
	StringUTF8 operator+(const StringViewUTF8& left, const char& right)
	{
		return left + StringViewUTF8(&right, 1);
	}
	StringUTF8 operator+(const StringViewUTF8& left, const UnicodeChar& right)
	{
		char buffer[4];
		uintMem count = right.ToUTF8(buffer, 4);
		return left + StringViewUTF8((void*)buffer, count);
	}
	StringUTF8 operator+(const StringUTF8& left, const StringViewUTF8& right)
	{
		return StringViewUTF8(left) + right;
	}
	StringUTF8 operator+(const StringView& left, const StringViewUTF8& right)
	{
		return StringViewUTF8(left) + right;
	}
	StringUTF8 operator+(const String& left, const StringViewUTF8& right)
	{
		return StringViewUTF8(left) + right;
	}
	StringUTF8 operator+(const char& left, const StringViewUTF8& right)
	{
		return StringViewUTF8(&left, 1) + right;
	}
	StringUTF8 operator+(const UnicodeChar& left, const StringViewUTF8& right)
	{
		char buffer[4];
		uintMem count = left.ToUTF8(buffer, 4);
		return StringViewUTF8((void*)buffer, count) + right;
	}
	StringUTF8 operator+(const StringUTF8& left, const StringUTF8& right)
	{
		return StringViewUTF8(left) + StringViewUTF8(right);
	}
	StringUTF8 operator+(const StringUTF8& left, const StringView& right)
	{
		return StringViewUTF8(left) + StringViewUTF8(right);
	}
	StringUTF8 operator+(const StringUTF8& left, const String& right)
	{
		return StringViewUTF8(left) + StringViewUTF8(right);
	}
	StringUTF8 operator+(const StringUTF8& left, const char& right)
	{
		return StringViewUTF8(left) + StringViewUTF8(&right, 1);
	}
	StringUTF8 operator+(const StringUTF8& left, const UnicodeChar& right)
	{
		char buffer[4];
		uintMem count = right.ToUTF8(buffer, 4);
		return StringViewUTF8(left) + StringViewUTF8(buffer, count);
	}
	StringUTF8 operator+(const StringView& left, const StringUTF8& right)
	{
		return StringViewUTF8(left) + StringViewUTF8(right);
	}
	StringUTF8 operator+(const String& left, const StringUTF8& right)
	{
		return StringViewUTF8(left) + StringViewUTF8(right);
	}
	StringUTF8 operator+(const char& left, const StringUTF8& right)
	{
		return StringViewUTF8(&left, 1) + StringViewUTF8(right);
	}
	StringUTF8 operator+(const UnicodeChar& left, const StringUTF8& right)
	{
		char buffer[4];
		uintMem count = left.ToUTF8(buffer, 4);
		return StringViewUTF8(buffer, count) + StringViewUTF8(right);
	}	

	StringViewUTF8Iterator begin(const StringUTF8& string)
	{
		return string.FirstIterator();
	}
	StringViewUTF8Iterator end(const StringUTF8& string)
	{
		return string.BehindIterator();
	}
}