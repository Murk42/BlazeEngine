#include "pch.h"
#include "BlazeEngineCore/DataStructures/StringUTF8.h"
#include "BlazeEngineCore/DataStructures/String.h"
#include "BlazeEngineCore/DataStructures/StringView.h"
#include "BlazeEngineCore/DataStructures/StringViewUTF8.h"
#include "BlazeEngineCore/Debug/Logger.h"
#include "BlazeEngineCore/Memory/MemoryManager.h"

namespace Blaze
{		
	StringUTF8::StringUTF8()
		: buffer(nullptr), bufferSize(0), characterCount(0)
	{
	}
	StringUTF8::StringUTF8(const StringUTF8& s)
		: buffer(nullptr), bufferSize(s.bufferSize), characterCount(s.characterCount)
	{
		if (s.bufferSize > 0)
		{
			buffer = Memory::Allocate(bufferSize + 1);
			memcpy(buffer, s.buffer, bufferSize + 1);
		}		
	}
	StringUTF8::StringUTF8(StringUTF8&& s) noexcept
		: buffer(s.buffer), bufferSize(s.bufferSize), characterCount(s.characterCount)
	{
		s.buffer = nullptr;
		s.bufferSize = 0;
		s.characterCount = 0;
	}
	StringUTF8::StringUTF8(const StringViewUTF8& s)
		: buffer(nullptr), bufferSize(0), characterCount(0)
	{
		if (!s.Empty())
		{
			bufferSize = s.BufferSize();			
			buffer = Memory::Allocate(bufferSize + 1);
			memcpy(buffer, s.Buffer(), bufferSize);
			((char*)buffer)[bufferSize] = '\0';			

			characterCount = s.CharacterCount();			
		}		
	}
	StringUTF8::StringUTF8(const String& s)
		: StringUTF8(StringViewUTF8(s))
	{			
	}
	StringUTF8::StringUTF8(const StringView& s)
		: StringUTF8(StringViewUTF8(s))
	{
	}
	StringUTF8::StringUTF8(const void* buffer, uintMem size)
		: StringUTF8(StringViewUTF8(buffer, size))		
	{
	}	
	StringUTF8::StringUTF8(StringViewUTF8Iterator begin, StringViewUTF8Iterator end)
		: StringUTF8(StringViewUTF8(begin, end))
	{
	}
	StringUTF8::~StringUTF8()
	{
		if (buffer != nullptr)
			Memory::Free(buffer);
	}
	void StringUTF8::Clear()
	{
		if (buffer != nullptr)
			Memory::Free(buffer);
		buffer = nullptr;
		bufferSize = 0;
		characterCount = 0;
	}	
	StringUTF8& StringUTF8::Resize(uintMem newCharacterCount, UnicodeChar character)
	{
		const uintMem charSize = character.UTF8Size();
		uint32 charUTF8Sequence = 0;
		character.ToUTF8(&charUTF8Sequence, sizeof(charUTF8Sequence));

		StringUTF8 oldString = std::move(*this);

		if (oldString.CharacterCount() > newCharacterCount)
		{
			const StringViewUTF8Iterator begin = FirstIterator();
			StringViewUTF8Iterator end = begin;
			for (uintMem i = 0; i < newCharacterCount; ++i)
				++end;

			operator=(StringUTF8(begin, end));
		}
		else
		{
			bufferSize = oldString.BufferSize() + (newCharacterCount - characterCount) * charSize;

			buffer = Memory::Allocate(bufferSize + 1);
			memcpy(buffer, oldString.Buffer(), oldString.BufferSize());
			for (uintMem i = 0; i < newCharacterCount - characterCount; ++i)
				memcpy((char*)buffer + oldString.BufferSize() + i * charSize, &charUTF8Sequence, charSize);

			characterCount = newCharacterCount;
			((char*)buffer)[bufferSize] = '\0';
		}

		return *this;
	}
	StringViewUTF8 StringUTF8::SubString(intMem start, intMem count) const
	{
		if (start < 0)
			start = (intMem)characterCount + start;

		if (start < 0)
		{
			BLAZE_ENGINE_CORE_FATAL("Accesing a string outside its bounds");
			return StringViewUTF8();
		}

		if (start > characterCount)
		{
			BLAZE_ENGINE_CORE_FATAL("Accesing a string outside its bounds");
			return StringViewUTF8();
		}

		StringViewUTF8Iterator it = FirstIterator();
		for (uintMem i = 0; i < start; ++i) ++it;

		return SubString(it, count);
	}	
	StringViewUTF8 StringUTF8::SubString(StringViewUTF8Iterator start, intMem count) const
	{
		if (count == 0)
			return StringViewUTF8();

		const uintMem characterCount = CharacterCount();

		if (start.Ptr() < buffer || start > BehindIterator())
		{
			BLAZE_ENGINE_CORE_FATAL("Accesing a string outside its bounds");
			return StringViewUTF8();
		}

		if (count < 0)
			count = CharacterCount() + count;

		auto begin = start;		
		auto end = begin;
		for (uintMem i = 0; i != count; ++i)
		{
			if (end == BehindIterator())
			{
				BLAZE_ENGINE_CORE_FATAL("Accessing a string outside its bounds");
				return StringViewUTF8();
			}

			++end;
		}

		return StringViewUTF8(begin, end);
	}
	void StringUTF8::Insert(intMem index, StringViewUTF8 string)
	{				
		if (string.Empty())
			return;

		if (index < 0)
			index = (intMem)characterCount + index;
		if (index < 0)
		{
			BLAZE_ENGINE_CORE_FATAL("Accessing a string outside its bounds");
			return;
		}		

		StringViewUTF8Iterator it = FirstIterator();
		for (uintMem i = 0; i < index; ++i) ++it;

		Insert(it, string);
	}
	void StringUTF8::Insert(StringViewUTF8Iterator it, StringViewUTF8 string)
	{
		if (string.Empty())
			return;

		if (it.Ptr() < buffer || it > BehindIterator())
		{
			BLAZE_ENGINE_CORE_FATAL("Accesing a string outside its bounds");
			return;
		}

		if (it == BehindIterator())
		{
			*this += string;
			return;
		}
		if (it == FirstIterator())
		{
			*this = string + *this;
			return;
		}

		StringUTF8 oldString = std::move(*this);

		bufferSize = oldString.BufferSize() + string.BufferSize();
		buffer = Memory::Allocate(bufferSize + 1);
		characterCount = oldString.CharacterCount() + string.CharacterCount();

		StringViewUTF8Iterator it1 = oldString.FirstIterator();
		StringViewUTF8Iterator it2 = it;

		uintMem size = (byte*)it2.Ptr() - (byte*)it1.Ptr();
		memcpy(buffer, oldString.Buffer(), size);

		uintMem offset = size;
		memcpy((byte*)buffer + offset, string.Buffer(), string.BufferSize());

		offset += string.BufferSize();
		size = oldString.BufferSize() - size;
		memcpy((char*)buffer + offset, it2.Ptr(), size);
		((char*)buffer)[bufferSize] = '\0';
	}
	void StringUTF8::EraseSubString(intMem index, uintMem count)
	{
		if (count == 0)
			return;

		if (index < 0)
			index = static_cast<intMem>(characterCount) + index;

		if (index < 0)
		{
			BLAZE_ENGINE_CORE_FATAL("Accessing a string outside its bounds");
			return;
		}

		StringViewUTF8Iterator begin = FirstIterator();
		for (uintMem i = 0; i < index; ++i) ++begin;

		EraseSubString(begin, count);
	}
	void StringUTF8::EraseSubString(StringViewUTF8Iterator begin, uintMem count)
	{
		if (count == 0)
			return;

		if (begin.IsNull())
		{
			BLAZE_ENGINE_CORE_FATAL("Erasing a sub string using a null iterator");
			return;
		}		
		
		StringViewUTF8Iterator end = begin;

		for (uintMem i = 0; i < count; ++i)		
			++end;		

		EraseSubString(begin, end);
	}
	void StringUTF8::EraseSubString(StringViewUTF8Iterator begin, StringViewUTF8Iterator end)
	{
		if (begin == end)
			return;

		if (begin.IsNull() || end.IsNull())
		{
			BLAZE_ENGINE_CORE_FATAL("Erasing a sub string using a null iterator");
			return;
		}

		if (begin > LastIterator() || end > BehindIterator())
		{
			BLAZE_ENGINE_CORE_FATAL("Accesing a string outside its bounds");
			return;
		}				

		if (begin == FirstIterator())
		{
			*this = StringViewUTF8(end, BehindIterator());
			return;
		}

		if (end == BehindIterator())
		{
			*this = StringViewUTF8(FirstIterator(), begin);
			return;
		}

		const StringUTF8 oldString = std::move(*this);

		uintMem length1 = (byte*)begin.Ptr() - oldString.buffer;
		uintMem length2 = (byte*)end.Ptr() - begin.Ptr();
		uintMem length3 = (byte*)oldString.buffer + oldString.bufferSize - end.Ptr();

		bufferSize = oldString.BufferSize() - length2;
		buffer = Memory::Allocate(bufferSize + 1);		
		characterCount = oldString.CharacterCount() - StringViewUTF8(begin, end).CharacterCount();

		memcpy(buffer, oldString.Buffer(), length1);
		memcpy((byte*)buffer + length1, (byte*)oldString.Buffer() + length1 + length2, length3);
		((char*)buffer)[bufferSize] = '\0';
	}
	uint32 StringUTF8::Hash() const
	{
		return static_cast<uint32>(std::hash<StringUTF8>()(*this));
	}
	UnicodeChar StringUTF8::First() const
	{
#ifdef BLAZE_INVALID_ITERATOR_CHECK
		if (bufferSize == 0)
		{
			BLAZE_ENGINE_CORE_FATAL("Accessing a empty string");
			return UnicodeChar();
		}
#endif
		return UnicodeChar(buffer, bufferSize);
	}
	UnicodeChar StringUTF8::Last() const
	{
#ifdef BLAZE_INVALID_ITERATOR_CHECK
		if (bufferSize == 0)
		{
			BLAZE_ENGINE_CORE_FATAL("Accessing a empty string");
			return UnicodeChar();
		}
#endif
		return StringViewUTF8Iterator((byte*)buffer + bufferSize).Decrement(*this).ToUnicode();
	}
	StringViewUTF8Iterator StringUTF8::FirstIterator() const
	{
		if (buffer == nullptr)
			return StringViewUTF8Iterator();
		return StringViewUTF8Iterator(buffer);
	}
	StringViewUTF8Iterator StringUTF8::LastIterator() const
	{
		if (buffer == nullptr)
			return StringViewUTF8Iterator();
		return StringViewUTF8Iterator((byte*)buffer + bufferSize).Decrement(*this);
	}
	StringViewUTF8Iterator StringUTF8::AheadIterator() const
	{
		if (buffer == nullptr)
			return StringViewUTF8Iterator();
		return StringViewUTF8Iterator((byte*)buffer).Decrement(*this);
	}
	StringViewUTF8Iterator StringUTF8::BehindIterator() const
	{
		if (buffer == nullptr)
			return StringViewUTF8Iterator();
		return StringViewUTF8Iterator((byte*)buffer + bufferSize);
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
		return operator=(StringUTF8(s));
	}
	StringUTF8& StringUTF8::operator=(const StringUTF8& s)
	{ 
		return operator=(StringUTF8(s));
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
	StringUTF8& StringUTF8::operator=(const StringView& s)
	{
		return operator=(StringUTF8(s));
	}
	StringUTF8& StringUTF8::operator=(const String& s)
	{
		return operator=(StringUTF8(s));
	}

	StringUTF8 operator+(const StringViewUTF8& left, const StringViewUTF8& right)
	{
		if (left.Empty())
			return right;
		if (right.Empty())
			return left;
		uintMem lSize = left.BufferSize();
		uintMem rSize = right.BufferSize();
		StringUTF8 out;
		out.bufferSize = lSize + rSize;
		out.buffer = Memory::Allocate(out.bufferSize + 1);
		out.characterCount = left.CharacterCount() + right.CharacterCount();
		memcpy(out.buffer, left.Buffer(), lSize);
		memcpy((byte*)out.buffer + lSize, right.Buffer(), rSize);
		((char*)out.buffer)[out.bufferSize] = '\0';
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