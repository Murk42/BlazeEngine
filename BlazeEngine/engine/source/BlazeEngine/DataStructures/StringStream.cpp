#include "BlazeENgine/DataStructures/StringStream.h"
#include "BlazeEngine/DataStructures/StringView.h"
#include "BlazeEngine/DataStructures/String.h"
#include <sstream>

namespace Blaze
{
	inline void StringStream::ResizeToFit(size_t size)
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
	StringStream::StringStream(StringStream&& ss) noexcept
		: ByteStream(std::move(ss))
	{

	}
	StringStream::StringStream(ByteStream&& bs) noexcept
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

	inline bool StringStream::IsEndOfLine() const
	{
		char* ptr = (char*)data.data() + offset;
		char* endPtr = (char*)data.data() + data.size();
		while (ptr != endPtr && isspace(*ptr) && *ptr != '\n') ++ptr;
		return ptr != endPtr && *ptr == '\n';
	}

	inline String StringStream::GetAsString() const
	{
		return String((char*)data.data(), data.size());
	}

	inline StringView StringStream::GetAsStringView() const
	{
		return StringView((char*)data.data(), data.size());
	}

	StringStream& StringStream::SkipUntil(char ch)
	{
		char* const end = (char*)data.end()._Ptr;
		char* const begin = (char*)data.begin()._Ptr;
		char* ptr = begin + offset;		

		while (ptr != end && *ptr != ch) ++ptr;

		offset = ptr - begin;
		return *this;
	}

	StringView StringStream::GetUntil(char ch)
	{
		char* begin = (char*)data.begin()._Ptr;
		char* end = (char*)data.end()._Ptr;
		char* beginOut = begin + offset;
		char* endOut = beginOut;		
		while (endOut != end && *endOut != ch) ++endOut;
		offset = endOut - begin;		
		return StringView(beginOut, endOut);
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

	StringView StringStream::GetLine()
	{
		char* const end = (char*)data.end()._Ptr;
		char* const begin = (char*)data.begin()._Ptr;
		char* outBegin = begin + offset;
		char* outEnd = outBegin;				

		while (outEnd != end && *outEnd != '\n') ++outEnd;

		offset = outEnd - begin;

		return StringView(outBegin, outEnd);
	}

	StringView StringStream::GetWord()
	{		
		char* const end = (char*)data.end()._Ptr;
		char* const begin = (char*)data.begin()._Ptr;
		char* outBegin = begin + offset;
		char* outEnd;

		while (outBegin != end && isspace(*outBegin)) ++outBegin;
		outEnd = outBegin;
		while (outEnd != end && !isspace(*outEnd)) ++outEnd;

		offset = outEnd - begin;

		return StringView(outBegin, outEnd);
	}

	template<>
	char StringStream::Get()
	{
		char value;
		char* ptr = (char*)data.data() + offset;
		while (*ptr < ' ' + 1) ptr++;
		value = *ptr;
		offset += ptr - ((char*)data.data() + offset) + 1;
		return value;		
	}
	template<>
	void StringStream::Set(const char& value)
	{
		ResizeToFit(1);
		((char*)data.data())[offset] = value;
		offset++;		
	}
	template<>
	uint StringStream::Get()
	{
		uint value;
		value = 0;
		char* ptr = (char*)data.data() + offset;
		while (*ptr < '!') ++ptr;
		while (*ptr > '0' - 1 && *ptr < '9' + 1)
			value = value * 10 + *(ptr++) - '0';
		offset = ptr - (char*)data.data() + 1;
		return value;
	}
	template<>
	void StringStream::Set(const uint& value)
	{
		std::stringstream ss;
		ss << value;
		size_t count = ss.gcount();
		ResizeToFit(count);
		ss.get((char*)data.data() + offset, count);
		offset += count;
	}
	template<>
	int StringStream::Get()
	{
		int value = 0;
		bool negative = false;
		char* ptr = (char*)data.data() + offset;
		while (*ptr < '!') ++ptr;
		while (*ptr != '-' && (*ptr < '0' || *ptr > '9')) ++ptr;
		negative = (*ptr == '-');
		ptr += negative;
		while (*ptr > '0' - 1 && *ptr < '9' + 1)
			value = value * 10 + *(ptr++) - '0';
		value = (int64(negative) * -2 + 1) * value;
		offset = ptr - (char*)data.data() + 1;
		return value;
	}
	template<>
	void StringStream::Set(const int& value)
	{
		std::stringstream ss;
		ss << value;
		size_t count = ss.gcount();
		ResizeToFit(count);
		ss.get((char*)data.data() + offset, count);
		offset += count;		
	}
	template<> 
	float StringStream::Get()
	{
		float value;
		char* end;
		value = strtof((char*)data.data() + offset, &end);
		offset = (uint8*)end - data.data();
		return value;
	}
	template<> 
	void StringStream::Set(const float& value)
	{
		std::stringstream ss;
		ss << value;
		size_t count = ss.gcount();
		ResizeToFit(count);
		ss.get((char*)data.data() + offset, count);
		offset += count;		
	}
	template<> 
	double StringStream::Get()
	{
		double value;
		char* end;
		value = strtod((char*)data.data() + offset, &end);
		offset = (uint8*)end - data.data();
		return value;
	}
	template<> 
	void StringStream::Set(const double& value)
	{
		std::stringstream ss;
		ss << value;
		size_t count = ss.gcount();
		ResizeToFit(count);
		ss.get((char*)data.data() + offset, count);
		offset += count;		
	}
	template<> 
	long double StringStream::Get()
	{
		long double value;
		char* end;
		value = strtold((char*)data.data() + offset, &end);
		offset = (uint8*)end - data.data();
		return value;
	}
	template<> 
	void StringStream::Set(const long double& value)
	{
		std::stringstream ss;
		ss << value;
		size_t count = ss.gcount();
		ResizeToFit(count);
		ss.get((char*)data.data() + offset, count);
		offset += count;		
	}

	void StringStream::operator=(const StringStream& ss)
	{
		ByteStream::operator=(ss);
	}
	void StringStream::operator=(StringStream&& ss) noexcept
	{
		ByteStream::operator=(std::move(ss));
	}
}