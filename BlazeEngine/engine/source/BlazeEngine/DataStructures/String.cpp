#include "BlazeEngine/DataStructures/String.h"
#include "BlazeEngine/DataStructures/StringView.h"
#include <cstring>
#include <cstdio>
#include <cctype>
#include <memory>

namespace Blaze
{
	String::String()
		: ptr(nullptr), size(0)
	{
	}
	String::String(const String& s)
		: size(s.size)
	{
		if (s.ptr != nullptr)
		{
			ptr = new char[size + 1];
			memcpy(ptr, s.ptr, size + 1);
		}
		else
			ptr = nullptr;
	}
	String::String(String&& s) noexcept
	{
		ptr = s.ptr;
		size = s.size;
		s.ptr = nullptr;
		s.size = 0;
	}
	String::String(size_t size)
		: size(size)
	{
		ptr = new char[size + 1];
		ptr[size] = '\0';
	}	
	String::String(const char* ptr, size_t size)
		: size(size)
	{		
		this->ptr = new char[size + 1];
		memcpy(this->ptr, ptr, size);
		this->ptr[size] = '\0';
	}
	String::String(const StringView& sv)
		: size(sv.Size())
	{
		ptr = new char[size + 1];
		memcpy(ptr, sv.Ptr(), size + 1);
	}	
	String::String(const char* ptr)
		: String(ptr, strlen(ptr))
	{
	}
	String::~String()
	{
		delete[] ptr;
	}	
	String String::SubString(size_t start, size_t size) const
	{
		return String(ptr + start, size);
	}
	void String::Clear()
	{
		delete[] ptr;
		size = 0;
	}
	String& String::operator=(const StringView& s)
	{
		delete[] ptr;
		if (s == nullptr)
		{
			ptr = nullptr;
			size = 0;			
		}
		else
		{
			size = s.Size();
			ptr = new char[size + 1];
			memcpy(ptr, s.Ptr(), size + 1);
		}
		return *this;
	}
	String& String::operator= (const String& s)
	{
		delete[] ptr;
		size = s.Size();
		ptr = new char[size + 1];
		memcpy(ptr, s.Ptr(), size + 1);
		return *this;
	}
	String& String::operator=(String&& s) noexcept
	{
		delete[] ptr;
		ptr = s.ptr;
		size = s.size;
		s.ptr = nullptr;
		s.size = 0;
		return *this;
	}
	bool String::operator==(const StringView& s) const
	{
		return strcmp(ptr, s.Ptr()) == 0;
	}
	bool String::operator!=(const StringView& s) const
	{
		return strcmp(ptr, s.Ptr()) != 0;
	}

	String& String::operator+=(const StringView& string)
	{
		return *this = *this + string;		
	}
	String& String::operator+=(const char& ch)
	{
		return *this = *this + ch;
	}

	char buffer[128];
	String String::Convert(uint32 value)
	{
		int size = sprintf_s(buffer, 128, "%u", value);
		return String(buffer, size);
	}
	String String::Convert(int32 value)
	{				
		int size = sprintf_s(buffer, 128, "%i", value);				
		return String(buffer, size);
	}
	String String::Convert(uint64 value)
	{
		int size = sprintf_s(buffer, 128, "%llu", value);
		return String(buffer, size);
	}
	String String::Convert(int64 value)
	{
		int size = sprintf_s(buffer, 128, "%ll", value);
		return String(buffer, size);
	}
	String String::Convert(float value)
	{
		int size = sprintf_s(buffer, 128, "%f", value);
		return String(buffer, size);
	}
	String String::Convert(double value)
	{
		int size = sprintf_s(buffer, 128, "%lf", value);
		return String(buffer, size);
	}	

	bool SpaceTillEnd(const char* begin, const char* end)
	{
		while (begin != end)
			if (!isspace(*begin))
				return false;
			else
				++begin;
		return true;
	}
	template<> inline bool String::ConvertTo<uint32>(const StringView& sv, uint32& value)
	{
		char* ptr;
		uint32 temp = strtoul(sv.Ptr(), &ptr, 10);
		if (SpaceTillEnd(ptr, sv.Ptr() + sv.Size()))
		{
			value = temp;
			return true;
		}
		return false;
	}
	template<> inline bool String::ConvertTo<int32 >(const StringView& sv, int32&  value)
	{
		char* ptr;
		int32 temp = strtol(sv.Ptr(), &ptr, 10);
		if (SpaceTillEnd(ptr, sv.Ptr() + sv.Size()))
		{
			value = temp;
			return true;
		}
		return false;
	}
	template<> inline bool String::ConvertTo<uint64>(const StringView& sv, uint64& value)
	{
		char* ptr;
		uint64 temp = strtoull(sv.Ptr(), &ptr, 10);
		if (SpaceTillEnd(ptr, sv.Ptr() + sv.Size()))
		{
			value = temp;
			return true;
		}
		return false;
	}
	template<> inline bool String::ConvertTo<int64 >(const StringView& sv, int64&  value)
	{
		char* ptr;
		int64 temp = strtoll(sv.Ptr(), &ptr, 10);
		if (SpaceTillEnd(ptr, sv.Ptr() + sv.Size()))
		{
			value = temp;
			return true;
		}
		return false;
	}
	template<> inline bool String::ConvertTo<float >(const StringView& sv, float&  value)
	{
		char* ptr;
		float temp = strtof(sv.Ptr(), &ptr);
		if (SpaceTillEnd(ptr, sv.Ptr() + sv.Size()))
		{
			value = temp;
			return true;
		}
		return false;
	}
	template<> inline bool String::ConvertTo<double>(const StringView& sv, double& value)
	{
		char* ptr;
		double temp = strtod(sv.Ptr(), &ptr);
		if (SpaceTillEnd(ptr, sv.Ptr() + sv.Size()))
		{
			value = temp;
			return true;
		}
		return false;
	}	

	String operator+(const StringView& left, const StringView& right)
	{
		if (left.Ptr() == nullptr)
			return right;
		if (right.Ptr() == nullptr)
			return left;
		size_t size = left.Size() + right.Size();
		char* ptr = new char[size + 1];
		memcpy(ptr, left.Ptr(), left.Size());
		memcpy(ptr + left.Size(), right.Ptr(), right.Size() + 1);
		String out;
		out.ptr = ptr;
		out.size = size;
		return out;
	}
	String operator+(const StringView& left, const char& right)
	{
		if (left.Ptr() == nullptr)
			return String(&right, 1);

		size_t size = left.Size() + 1;
		char* ptr = new char[size + 1];

		memcpy(ptr, left.Ptr(), left.Size());
		ptr[size - 1] = right;
		ptr[size] = '\0';

		String out;
		out.ptr = ptr;
		out.size = size;
		return out;
	}
	String operator+(const char& left, const StringView& right)
	{
		if (right.Ptr() == nullptr)
			return String(&left , 1);

		size_t size = 1 + right.Size();
		char* ptr = new char[size + 1];

		ptr[0] = left;
		memcpy(ptr + 1, right.Ptr(), right.Size());
		ptr[size] = '\0';

		String out;
		out.ptr = ptr;
		out.size = size;
		return out;
	}

}