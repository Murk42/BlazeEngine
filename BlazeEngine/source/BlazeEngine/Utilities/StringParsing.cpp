#include "pch.h"
#include "BlazeEngine/Utilities/StringParsing.h"

namespace Blaze::StringParsing
{
	bool SplitAtFirst(StringView _value, String& first, String& second, char ch)
	{
		String value = _value;
		const char* begin = value.Ptr();
		const char* end = begin + value.Size();
		const char* it = begin;

		while (it != end && *it != ch)
			++it;

		if (it == end)
			return false;

		first = String(begin, it - begin);
		second = String(it + 1, end - it - 1);
		return true;
	}

	bool GetAfterFirst(StringView _value, String& out, char ch)
	{
		String value = _value;
		const char* begin = value.Ptr();
		const char* end = begin + value.Size();
		const char* it = begin;

		while (it != end && *it != ch)
			++it;

		if (it == end)
			return false;
		
		out = String(it + 1, end - it - 1);
		return true;
	}
	bool GetBeforeFirst(StringView _value, String& out, char ch)
	{
		String value = _value;
		const char* begin = value.Ptr();
		const char* end = begin + value.Size();
		const char* it = begin;

		while (it != end && *it != ch)
			++it;

		if (it == end)
			return false;

		begin = value.begin();
		out = String(begin, it - begin);
		return true;
	}
	bool GetAfterLast(StringView _value, String& out, char ch)
	{
		String value = _value;
		const char* end = value.Ptr() - 1;
		const char* begin = end + value.Size();
		const char* it = begin;

		while (it != end && *it != ch)
			--it;

		if (it == end)
			return false;

		end = value.end();
		out = String(it + 1, end - it - 1);
		return true;
	}
	bool GetBeforeLast(StringView _value, String& out, char ch)
	{
		String value = _value;
		const char* end = value.Ptr() - 1;
		const char* begin = end + value.Size();
		const char* it = begin;

		while (it != end && *it != ch)
			++it;

		if (it == end)
			return false;

		begin = value.begin();
		out = String(begin, it - begin);
		return true;
	}

	bool RemoveQuotes(StringView value, String& out)
	{		
		if (!GetAfterFirst(value, out, '"')) return false;
		if (!GetBeforeFirst(out, out, '"'))
		{
			out = value;
			return false;
		}
		return true;
	}
	bool RemoveBrackets(StringView value, String& out, char right, char left)
	{
		if (!GetAfterFirst(value, out, right)) return false;
		if (!GetBeforeLast(out, out, left))
		{
			out = value;
			return false;
		}
		return true;
	}
	void RemoveSpace(StringView value, String& out)
	{
		const char* begin = value.Ptr();
		const char* end = begin + value.Size();
		const char* it1 = begin;
		const char* it2 = end - 1;

		while (it1 != end && isspace(*it1)) ++it1;

		if (it1 == end)
		{
			out = value;
			return;
		}

		while (isspace(*it2)) --it2;

		out = String(it1, it2 - it1 + 1);
	}

	Array<String> Split(StringView value, char ch)
	{
		size_t count = value.Count(ch) + 1;
		Array<String> out(count);

		String current = value;
		for (uint i = 0; i < count; ++i)
			SplitAtFirst(current, out[i], current, ch);

		return out;
	}

	template<std::integral T> StringView IntegerTypeName();
	template<> StringView IntegerTypeName<int64>() { return "int64"; }
	template<> StringView IntegerTypeName<uint64>() { return "uint64"; }
	template<> StringView IntegerTypeName<int32>() { return "int32"; }
	template<> StringView IntegerTypeName<uint32>() { return "uint32"; }
	template<> StringView IntegerTypeName<int16>() { return "int16"; }
	template<> StringView IntegerTypeName<uint16>() { return "uint16"; }
	template<> StringView IntegerTypeName<int8>() { return "int8"; }
	template<> StringView IntegerTypeName<uint8>() { return "uint8"; }	

	template<std::floating_point T> StringView FloatTypeName();
	template<> StringView FloatTypeName<float>() { return "float"; }
	template<> StringView FloatTypeName<double>() { return "double"; }	

	std::chars_format ToStdCharsFormat(FloatStringFormat format)
	{
		switch (format)
		{
		case Blaze::StringParsing::FloatStringFormat::Scientific: return std::chars_format::scientific;
		case Blaze::StringParsing::FloatStringFormat::Fixed: return std::chars_format::fixed;
		case Blaze::StringParsing::FloatStringFormat::Hex: return std::chars_format::hex;
		case Blaze::StringParsing::FloatStringFormat::Default: return std::chars_format::general;
		default:
			throw;
		}
	}

	template<std::integral T>
	Result CharsToNumber(const char* str, uint length, T& value, int base, uint* count) 
	{ 		
		auto [end, err] = std::from_chars(str, str + length, value, base);

		if (err == std::errc::result_out_of_range)
		{
			value = 0;
			*count = 0;
			return BLAZE_ERROR_RESULT("Blaze Engine", "The number in the string cannot be stored in a " + IntegerTypeName<T>() + ". The string was" + StringView(str, length));
		}
		else if (count != nullptr)
			*count = end - str;

		return Result();		
	}	
	template<std::floating_point T>
	Result CharsToNumber(const char* str, uint length, T& value, FloatStringFormat format, uint* count)
	{
		auto [end, err] = std::from_chars(str, str + length, value, ToStdCharsFormat(format));

		if (err == std::errc::result_out_of_range)
		{
			value = 0;
			*count = 0;
			return BLAZE_ERROR_RESULT("Blaze Engine", "The number in the string cannot be stored in a " + FloatTypeName<T>() + ". The string was" + StringView(str, length));
		}
		else if (count != nullptr)
			*count = end - str;

		return Result();
	}	


	template<std::integral T>
	Result NumberToChars(T value, char* str, uint maxCount, int base, uint* count)
	{
		auto [end, err] = std::to_chars(str, str + maxCount, value, base);
		if (err == std::errc::value_too_large)
		{			
			*count = 0;
			return BLAZE_ERROR_RESULT("Blaze Engine", "The value is too large to be represented in a string with " + Convert(maxCount) + " characters");
		}
		else if (count != nullptr)
			*count = end - str;

		return Result();
	}	
	template<std::floating_point T>
	Result NumberToChars(T value, char* str, uint maxCount, FloatStringFormat format, uint* count)
	{
		auto res = std::to_chars(str, str + maxCount, value, ToStdCharsFormat(format));
		if (res.ec == std::errc::value_too_large)
		{			
			*count = 0;
			return BLAZE_ERROR_RESULT("Blaze Engine", "The value is too large to be represented in a string with " + Convert(maxCount) + " characters");
		}
		else if (count != nullptr)
			*count = res.ptr - str;

		return Result();
	}		

	Result Convert(const StringView& from, uint64& to, uint base, uint* count) { return CharsToNumber(from.Ptr(), from.Size(), to, base, count); }
	Result Convert(const StringView& from, int64&  to, uint base, uint* count) { return CharsToNumber(from.Ptr(), from.Size(), to, base, count); }
	Result Convert(const StringView& from, uint32& to, uint base, uint* count) { return CharsToNumber(from.Ptr(), from.Size(), to, base, count); }
	Result Convert(const StringView& from, int32&  to, uint base, uint* count) { return CharsToNumber(from.Ptr(), from.Size(), to, base, count); }
	Result Convert(const StringView& from, uint16& to, uint base, uint* count) { return CharsToNumber(from.Ptr(), from.Size(), to, base, count); }
	Result Convert(const StringView& from, int16&  to, uint base, uint* count) { return CharsToNumber(from.Ptr(), from.Size(), to, base, count); }
	Result Convert(const StringView& from, uint8&  to, uint base, uint* count) { return CharsToNumber(from.Ptr(), from.Size(), to, base, count); }
	Result Convert(const StringView& from, int8&   to, uint base, uint* count) { return CharsToNumber(from.Ptr(), from.Size(), to, base, count); }
	Result Convert(const StringView& from, float&  to, FloatStringFormat format, uint* count) { return CharsToNumber(from.Ptr(), from.Size(), to, format, count); }
	Result Convert(const StringView& from, double& to, FloatStringFormat format, uint* count) { return CharsToNumber(from.Ptr(), from.Size(), to, format, count); }


	template<std::integral T>
	String _Convert(T value, uint base, uint* count)
	{
		char buffer[64];

		uint _count;
		if (Result r = NumberToChars(value, buffer, 64, base, &_count))		
			return "";		

		if (count != nullptr)
			*count = _count;

		return String(buffer, _count);
	}
	template<std::floating_point T>
	String _Convert(T value, FloatStringFormat format, uint* count)
	{		
		char buffer[64];
		
		uint _count;
		if (Result r = NumberToChars(value, buffer, 64, format, &_count))				
			return "";

		if (count != nullptr)
			*count = _count;
		
		return String(buffer, _count);
	}
	
	String Convert(uint64 from, uint base, uint* count) { return _Convert(from, base, count); }
	String Convert(int64  from, uint base, uint* count) { return _Convert(from, base, count); }
	String Convert(uint32 from, uint base, uint* count) { return _Convert(from, base, count); }
	String Convert(int32  from, uint base, uint* count) { return _Convert(from, base, count); }
	String Convert(uint16 from, uint base, uint* count) { return _Convert(from, base, count); }
	String Convert(int16  from, uint base, uint* count) { return _Convert(from, base, count); }
	String Convert(uint8  from, uint base, uint* count) { return _Convert(from, base, count); }
	String Convert(int8   from, uint base, uint* count) { return _Convert(from, base, count); }
	String Convert(float  from, FloatStringFormat format, uint* count) { return _Convert(from, format, count); }
	String Convert(double from, FloatStringFormat format, uint* count) { return _Convert(from, format, count); }
}