#include "BlazeENgine/DataStructures/StringStream.h"
#include "BlazeEngine/DataStructures/String.h"
#include "BlazeEngine/DataStructures/StringView.h"
#include <sstream>

namespace Blaze
{
	inline void StringStream::ResizeToFit(int size)
	{
		if (data.size() < offset + size)
			data.resize(offset + size);
	}

	StringStream::StringStream()
		: ByteStream()
	{
	}
	StringStream::StringStream(const StringStream& ss) 
		: ByteStream(ss)
	{

	}
	StringStream::StringStream(StringStream&& ss)
		: ByteStream(std::move(ss))
	{

	}
	StringStream::StringStream(ByteStream&& bs)
		: ByteStream(std::move(bs))
	{
	}
	StringStream::StringStream(const String& s)
	{
		data.resize(s.Size());
		memcpy(data.data(), s.Ptr(), s.Size());
		offset = 0;
	}
	StringStream::StringStream(const StringView& s)
	{
		data.resize(s.Size());
		memcpy(data.data(), s.Ptr(), s.Size());
		offset = 0;
	}
	StringStream::~StringStream()
	{

	}

	inline String StringStream::GetAsString() const
	{
		return String((char*)data.data(), data.size());
	}

	inline StringView StringStream::GetAsStringView() const
	{
		return StringView((char*)data.data(), data.size());
	}

	StringStream& StringStream::SkipUntil(char end)
	{
		while (data.data()[offset++] != end);
		offset--;
		return *this;
	}

	StringView StringStream::GetUntil(char end)
	{
		char* startPtr = (char*)data.data() + offset;
		char* endPtr = startPtr;
		while (*(endPtr++) != end);
		--endPtr;
		offset += endPtr - startPtr;
		return StringView(startPtr, endPtr - startPtr);
	}

	StringStream& StringStream::GetUntil(String& s, char end)
	{
		s = GetUntil(end);
		return *this;
	}

	StringStream& StringStream::GetUntil(StringView& s, char end)
	{
		s = GetUntil(end);
		return *this;
	}

	template<>
	StringStream& StringStream::GetLine(String& value)
	{
		register char* startPtr = (char*)data.data() + offset;
		register char* endPtr = startPtr;
		register char* lastPtr = (char*)data.data() + data.size() - 1;
		while (*endPtr != '\n' && endPtr != lastPtr) endPtr++;
		endPtr++;
		value.Resize(endPtr - startPtr);
		memcpy(value.Ptr(), startPtr, endPtr - startPtr);
		offset += endPtr - ((char*)data.data() + offset);
		return *this;		
	}

	template<>
	StringStream& StringStream::GetLine(StringView& value)
	{
		register char* startPtr = (char*)data.data() + offset;
		register char* endPtr = startPtr;
		register char* lastPtr = (char*)data.data() + data.size() - 1;
		while (*endPtr != '\n' && endPtr != lastPtr) endPtr++;
		endPtr++;
		value = StringView(startPtr, endPtr - startPtr);				
		offset += endPtr - ((char*)data.data() + offset);
		return *this;
	}

	template<>
	StringStream& StringStream::Get(char& value)
	{
		char* ptr = (char*)data.data() + offset;
		while (*ptr < ' ' + 1) ptr++;
		value = *ptr;
		offset += ptr - ((char*)data.data() + offset) + 1;
		return *this;		
	}

	template<>
	StringStream& StringStream::Set(const char& value)
	{
		ResizeToFit(1);
		((char*)data.data())[offset] = value;
		offset++;
		return *this;
	}

	template<>
	StringStream& StringStream::Get(String& value)
	{
		register char* startPtr = (char*)data.data() + offset;
		while (*startPtr < ' ' + 1) ++startPtr;
		register char* endPtr = startPtr;
		register char* lastPtr = (char*)data.data() + data.size();
		while (endPtr != lastPtr && *endPtr > ' ') endPtr++;

		value.Resize(endPtr - startPtr);
		memcpy(value.Ptr(), startPtr, endPtr - startPtr);		
		offset += endPtr - ((char*)data.data() + offset);
		return *this;
	}
	template<>
	StringStream& StringStream::Set(const String& value)
	{
		ResizeToFit(value.Size());		
		memcpy(data.data() + offset, value.Ptr(), value.Size());
		offset += value.Size();
		return *this;
	}
	template<>
	StringStream& StringStream::Get(StringView& value)
	{
		register char* startPtr = (char*)data.data() + offset;
		while (*startPtr < ' ' + 1) ++startPtr;
		register char* endPtr = startPtr + 1;
		register char* lastPtr = (char*)data.data() + data.size();
		while (endPtr != lastPtr && *endPtr > ' ') endPtr++;

		value = StringView(startPtr, endPtr - startPtr);		
		offset += endPtr - ((char*)data.data() + offset);
		return *this;
	}
	template<>
	StringStream& StringStream::Set(const StringView& value)
	{
		ResizeToFit(value.Size());
		memcpy(data.data() + offset, value.Ptr(), value.Size());
		offset += value.Size();
		return *this;
	}
	template<>
	StringStream& StringStream::Get(uint8& value)
	{		
		value = Get<uint64>();
		return *this;		
	}
	template<>
	StringStream& StringStream::Set(const uint8& value)
	{
		return Set<uint64>(value);
	}
	template<>
	StringStream& StringStream::Get(uint16& value)
	{
		value = Get<uint64>();
		return *this;
	}
	template<>
	StringStream& StringStream::Set(const uint16& value)
	{
		return Set<uint64>(value);
	}
	template<>
	StringStream& StringStream::Get(uint32& value)
	{
		value = Get<uint64>();
		return *this;
	}
	template<>
	StringStream& StringStream::Set(const uint32& value)
	{
		return Set<uint64>(value);
	}
	template<>
	StringStream& StringStream::Get(uint64& value)
	{
		value = 0;
		char* ptr = (char*)data.data() + offset;
		while (*ptr < '!') ++ptr;
		while (*ptr > '0' - 1 && *ptr < '9' + 1)
			value = value * 10 + *(ptr++) - '0';
		offset = ptr - (char*)data.data() + 1;
		return *this;
	}
	template<>
	StringStream& StringStream::Set(const uint64& value)
	{
		std::stringstream ss;
		ss << value;
		size_t count = ss.gcount();
		ResizeToFit(count);
		ss.get((char*)data.data() + offset, count);
		offset += count;
		return *this;
	}
	template<>
	StringStream& StringStream::Get(int8& value)
	{
		value = Get<int64>();
		return *this;
	}
	template<>
	StringStream& StringStream::Set(const int8& value)
	{
		return Set<int64>(value);
	}
	template<>
	StringStream& StringStream::Get(int16& value)
	{
		value = Get<int64>();
		return *this;
	}
	template<>
	StringStream& StringStream::Set(const int16& value)
	{
		return Set<int64>(value);
	}
	template<>
	StringStream& StringStream::Get(int32& value)
	{
		value = Get<int64>();
		return *this;
	}
	template<>
	StringStream& StringStream::Set(const int32& value)
	{
		return Set<int64>(value);
	}
	template<>
	StringStream& StringStream::Get(int64& value)
	{
		value = 0;
		bool negative = false;
		char* ptr = (char*)data.data() + offset;
		while (*ptr < '!') ++ptr;
		while (*ptr != '-' && (*ptr < '0' || *ptr > '9'));
		negative = (*ptr == '-');
		ptr += negative;
		while (*ptr > '0' - 1 && *ptr < '9' + 1)
			value = value * 10 + *ptr - '0';
		value = (negative * 2 - 1) * value;
		offset = ptr - (char*)data.data() + 1;
		return *this;
	}
	template<>
	StringStream& StringStream::Set(const int64& value)
	{
		std::stringstream ss;
		ss << value;
		size_t count = ss.gcount();
		ResizeToFit(count);
		ss.get((char*)data.data() + offset, count);
		offset += count;
		return *this;
	}
	template<> 
	StringStream& StringStream::Get<float>(float& value)
	{
		char* end;
		value = strtof((char*)data.data() + offset, &end);
		offset = (uint8*)end - data.data();
		return *this;
	}
	template<> 
	StringStream& StringStream::Set<float>(const float& value)
	{
		std::stringstream ss;
		ss << value;
		size_t count = ss.gcount();
		ResizeToFit(count);
		ss.get((char*)data.data() + offset, count);
		offset += count;
		return *this;
	}
	template<> 
	StringStream& StringStream::Get<double>(double& value)
	{
		char* end;
		value = strtod((char*)data.data() + offset, &end);
		offset = (uint8*)end - data.data();
		return *this;
	}
	template<> 
	StringStream& StringStream::Set<double>(const double& value)
	{
		std::stringstream ss;
		ss << value;
		size_t count = ss.gcount();
		ResizeToFit(count);
		ss.get((char*)data.data() + offset, count);
		offset += count;
		return *this;
	}
	template<> 
	StringStream& StringStream::Get<long double>(long double& value)
	{
		char* end;
		value = strtold((char*)data.data() + offset, &end);
		offset = (uint8*)end - data.data();
		return *this;
	}
	template<> 
	StringStream& StringStream::Set<long double>(const long double& value)
	{
		std::stringstream ss;
		ss << value;
		size_t count = ss.gcount();
		ResizeToFit(count);
		ss.get((char*)data.data() + offset, count);
		offset += count;
		return *this;
	}

	void StringStream::operator=(const StringStream& ss)
	{
		ByteStream::operator=(ss);
	}
	void StringStream::operator=(StringStream&& ss)
	{
		ByteStream::operator=(std::move(ss));
	}
}