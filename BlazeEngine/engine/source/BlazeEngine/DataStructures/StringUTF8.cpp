#include "BlazeEngine/DataStructures/StringUTF8.h"
#include "BlazeEngine/DataStructures/StringViewUTF8.h"
#include "BlazeEngine/DataStructures/StringView.h"
#include "BlazeEngine/Core/MemoryManager.h"
#include <cstring>
#include <cstdio>
#include <cctype>
#include <memory>

namespace Blaze
{
	StringUTF8::Iterator::Iterator()
		: ptr(nullptr)
	{
	}
	StringUTF8::Iterator::Iterator(void* ptr)
		: ptr(ptr)
	{
	}
	StringUTF8::Iterator::Iterator(const Iterator& i)
		: ptr(i.ptr)
	{
	}
	UnicodeChar StringUTF8::Iterator::ToUnicode() const
	{
		return UnicodeChar((const char*)ptr);
	}
	StringUTF8::Iterator StringUTF8::Iterator::operator++()
	{
		Iterator old = *this;
		if (((*(uint8*)ptr) >> 6) == 0)
			ptr = (byte*)ptr + 1;
		else if (((*(uint8*)((byte*)ptr + 1)) >> 4) == 0)
			ptr = (byte*)ptr + 2;
		else if (((*(uint8*)((byte*)ptr + 2)) >> 2) == 0)
			ptr = (byte*)ptr + 3;
		else if (((*(uint8*)((byte*)ptr + 3)) >> 5) == 0)
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
		if (((*(uint8*)ptr) >> 6) == 0)
			ptr = (byte*)ptr + 1;
		else if (((*(uint8*)((byte*)ptr + 1)) >> 4) == 0)
			ptr = (byte*)ptr + 2;
		else if (((*(uint8*)((byte*)ptr + 2)) >> 2) == 0)
			ptr = (byte*)ptr + 3;
		else if (((*(uint8*)((byte*)ptr + 3)) >> 5) == 0)
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
		return *this;
	}

	StringUTF8::StringUTF8()
		: buffer(nullptr), size(0)
	{
	}

	StringUTF8::StringUTF8(const StringUTF8& s)
		: size(s.Size())
	{
		if (s.size > 0)
		{
			buffer = Memory::Allocate(size + 1);
			memcpy(buffer, s.buffer, size + 1);
		}
		else
			buffer = nullptr;
	}

	StringUTF8::StringUTF8(StringUTF8&& s) noexcept
		: buffer(s.buffer), size(s.size)
	{
		s.buffer = nullptr;
		s.size = 0;
	}

	StringUTF8::StringUTF8(const char* ptr)
	{
		if (ptr != nullptr)
		{
			size = strlen(ptr);
			buffer = Memory::Allocate(size + 1);
			memcpy(buffer, ptr, size + 1);
		}
		else
		{
			buffer = nullptr;
			size = 0;
		}
	}

	StringUTF8::StringUTF8(const char* ptr, size_t size)
		: size(size)
	{
		if (ptr != nullptr && size != 0)
		{
			buffer = Memory::Allocate(size + 1);
			memcpy(buffer, ptr, size);
			((char*)buffer)[size] = '\0';
		}
		else
		{
			buffer = nullptr;
			size = 0;
		}
	}

	StringUTF8::StringUTF8(const StringViewUTF8& s)
		: size(s.Size())
	{
		if (s.Buffer() == nullptr)
		{
			buffer = Memory::Allocate(size + 1);
			memcpy(buffer, s.Buffer(), size + 1);
		}
		else
			buffer = nullptr;
	}

	StringUTF8::StringUTF8(const StringView& s)
		: size(s.Size())
	{
		if (s.Ptr() == nullptr)
		{
			buffer = Memory::Allocate(size + 1);
			memcpy(buffer, s.Ptr(), size + 1);
		}
		else
			buffer = nullptr;
	}

	StringUTF8::~StringUTF8()
	{
		Memory::Free(buffer);
	}

	void StringUTF8::Clear()
	{
		Memory::Free(buffer);
		buffer = nullptr;
		size = 0;
	}

	StringUTF8::Iterator StringUTF8::begin() const
	{
		return Iterator(buffer);
	}
	StringUTF8::Iterator StringUTF8::end() const
	{
		return Iterator((byte*)buffer + size);
	}

	StringUTF8& StringUTF8::operator=(const StringViewUTF8& s)
	{
		if (s.Buffer() != nullptr)
		{
			size = s.Size();
			buffer = Memory::Allocate(size + 1);
			memcpy(buffer, s.Buffer(), size + 1);
		}
		else
		{
			size = 0;
			buffer = nullptr;
		}
		return *this;
	}

	StringUTF8& StringUTF8::operator=(StringUTF8&& s) noexcept
	{
		size = s.size;
		buffer = s.buffer;
		s.buffer = nullptr;
		s.size = 0;
		return *this;
	}

	bool StringUTF8::operator==(const StringViewUTF8& s) const
	{
		if (size != s.Size())
			return false;

		return memcmp(buffer, s.Buffer(), size) == 0;
	}

	bool StringUTF8::operator!=(const StringViewUTF8& s) const
	{
		if (size != s.Size())
			return true;

		return memcmp(buffer, s.Buffer(), size) != 0;
	}

	extern char buffer[128];
	bool SpaceTillEnd(const char* begin, const char* end);

	StringUTF8 StringUTF8::Convert(uint32 value)
	{
		int size = sprintf_s(Blaze::buffer, 128, "%u", value);
		return StringUTF8(Blaze::buffer, size);
	}
	StringUTF8 StringUTF8::Convert(int32 value)
	{
		int size = sprintf_s(Blaze::buffer, 128, "%i", value);
		return StringUTF8(Blaze::buffer, size);
	}
	StringUTF8 StringUTF8::Convert(uint64 value)
	{
		int size = sprintf_s(Blaze::buffer, 128, "%llu", value);
		return StringUTF8(Blaze::buffer, size);
	}
	StringUTF8 StringUTF8::Convert(int64 value)
	{
		int size = sprintf_s(Blaze::buffer, 128, "%ll", value);
		return StringUTF8(Blaze::buffer, size);
	}
	StringUTF8 StringUTF8::Convert(float value)
	{
		int size = sprintf_s(Blaze::buffer, 128, "%f", value);
		return StringUTF8(Blaze::buffer, size);
	}
	StringUTF8 StringUTF8::Convert(double value)
	{
		int size = sprintf_s(Blaze::buffer, 128, "%lf", value);
		return StringUTF8(Blaze::buffer, size);
	}
	
	template<> bool StringUTF8::ConvertTo<uint32>(const StringViewUTF8& sv, uint32& value)
	{
		char* ptr;
		uint32 temp = strtoul((char*)sv.Buffer(), &ptr, 10);
		if (SpaceTillEnd(ptr, (char*)sv.Buffer() + sv.Size()))
		{
			value = temp;
			return true;
		}
		return false;
	}
	template<> bool StringUTF8::ConvertTo<int32 >(const StringViewUTF8& sv, int32& value)
	{
		char* ptr;
		int32 temp = strtol((char*)sv.Buffer(), &ptr, 10);
		if (SpaceTillEnd(ptr, (char*)sv.Buffer() + sv.Size()))
		{
			value = temp;
			return true;
		}
		return false;
	}
	template<> bool StringUTF8::ConvertTo<uint64>(const StringViewUTF8& sv, uint64& value)
	{
		char* ptr;
		uint64 temp = strtoull((char*)sv.Buffer(), &ptr, 10);
		if (SpaceTillEnd(ptr, (char*)sv.Buffer() + sv.Size()))
		{
			value = temp;
			return true;
		}
		return false;
	}
	template<> bool StringUTF8::ConvertTo<int64 >(const StringViewUTF8& sv, int64& value)
	{
		char* ptr;
		int64 temp = strtoll((char*)sv.Buffer(), &ptr, 10);
		if (SpaceTillEnd(ptr, (char*)sv.Buffer() + sv.Size()))
		{
			value = temp;
			return true;
		}
		return false;
	}
	template<> bool StringUTF8::ConvertTo<float >(const StringViewUTF8& sv, float& value)
	{
		char* ptr;
		float temp = strtof((char*)sv.Buffer(), &ptr);
		if (SpaceTillEnd(ptr, (char*)sv.Buffer() + sv.Size()))
		{
			value = temp;
			return true;
		}
		return false;
	}
	template<> bool StringUTF8::ConvertTo<double>(const StringViewUTF8& sv, double& value)
	{
		char* ptr;
		double temp = strtod((char*)sv.Buffer(), &ptr);
		if (SpaceTillEnd(ptr, (char*)sv.Buffer() + sv.Size()))
		{
			value = temp;
			return true;
		}
		return false;
	}


	StringUTF8 operator+(const StringViewUTF8& left, const StringViewUTF8& right)
	{
		if (left.Size() == 0)
			return right;
		if (right.Size() == 0)
			return left;
		StringUTF8 out;
		out.size = left.Size() + right.Size();
		out.buffer = Memory::Allocate(out.size + 1);
		memcpy(out.buffer, left.Buffer(), left.Size());
		memcpy((byte*)out.buffer + left.Size(), right.Buffer(), right.Size() + 1);
		return out;
	}	
}