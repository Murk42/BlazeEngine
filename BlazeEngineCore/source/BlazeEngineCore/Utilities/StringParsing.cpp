#include "pch.h"
#include "BlazeEngineCore/Utilities/StringParsing.h"

namespace Blaze::StringParsing
{
	bool SplitAtFirst(StringView value, String& first, String& second, char ch)
	{
		StringViewIterator end = value.BehindIterator();
		StringViewIterator it = value.FirstIterator();

		while (it != end && *it != ch)
			++it;

		if (it == end)
		{
			first = value;
			second.Clear();
			return false;
		}

		first = String(value.FirstIterator(), it);
		second = String(++it, end);
		return true;
	}
	bool SplitAtFirst(StringViewUTF8 value, StringUTF8& first, StringUTF8& second, UnicodeChar ch)
	{
		StringViewUTF8Iterator end = value.BehindIterator();
		StringViewUTF8Iterator it = value.FirstIterator();

		while (it != end && *it != ch)
			++it;

		if (it == end)
		{
			first = value;
			second.Clear();
			return false;
		}

		first = StringUTF8(value.FirstIterator(), it);
		second = StringUTF8(++it, end);
		return true;
	}
	bool SplitAtFirstGroup(StringView value, String& first, String& second, char ch)
	{
		StringViewIterator end = value.BehindIterator();
		StringViewIterator it = value.FirstIterator();

		while (it != end && *it != ch)
			++it;

		if (it == end)
		{
			first = value;
			second.Clear();
			return false;
		}

		first = String(value.FirstIterator(), it);

		while (it != end && *it == ch)
			++it;

		second = String(it, end);
		return true;
	}
	bool SplitAtFirstGroup(StringViewUTF8 value, StringUTF8& first, StringUTF8& second, UnicodeChar ch)
	{
		StringViewUTF8Iterator end = value.BehindIterator();
		StringViewUTF8Iterator it = value.FirstIterator();

		while (it != end && *it != ch)
			++it;

		if (it == end)
		{
			first = value;
			second.Clear();
			return false;
		}

		first = StringUTF8(value.FirstIterator(), it);

		while (it != end && *it == ch)
			++it;

		second = StringUTF8(it, end);
		return true;
	}

	bool GetAfterFirst(StringView value, String& out, char ch)
	{
		StringViewIterator end = value.BehindIterator();
		StringViewIterator it = value.FirstIterator();

		while (it != end && *it != ch)
			++it;

		if (it == end)
			return false;

		++it;

		out = String(it, end);
		return true;
	}
	bool GetAfterFirst(StringViewUTF8 value, StringUTF8& out, UnicodeChar ch)
	{
		StringViewUTF8Iterator end = value.BehindIterator();
		StringViewUTF8Iterator it = value.FirstIterator();

		while (it != end && *it != ch)
			++it;

		if (it == end)
			return false;

		++it;

		out = StringUTF8(it, end);
		return true;
	}
	bool GetAfterFirstGroup(StringView value, String& out, char ch)
	{
		StringViewIterator end = value.BehindIterator();
		StringViewIterator it = value.FirstIterator();

		while (it != end && *it != ch)
			++it;

		if (it == end)
		{
			out = value;
			return false;
		}

		while (it != end && *it == ch)
			++it;

		out = String(it, end);
		return true;
	}
	bool GetAfterFirstGroup(StringViewUTF8 value, StringUTF8& out, UnicodeChar ch)
	{
		StringViewUTF8Iterator end = value.BehindIterator();
		StringViewUTF8Iterator it = value.FirstIterator();

		while (it != end && *it != ch)
			++it;

		if (it == end)
		{
			out = value;
			return false;
		}

		while (it != end && *it == ch)
			++it;

		out = StringUTF8(it, end);
		return true;
	}
	bool GetBeforeFirst(StringView value, String& out, char ch)
	{
		StringViewIterator end = value.BehindIterator();
		StringViewIterator it = value.FirstIterator();

		while (it != end && *it != ch)
			++it;

		if (it == end)
		{
			out = value;
			return false;
		}

		out = String(value.FirstIterator(), it);
		return true;
	}
	bool GetBeforeFirst(StringViewUTF8 value, StringUTF8& out, UnicodeChar ch)
	{
		StringViewUTF8Iterator end = value.BehindIterator();
		StringViewUTF8Iterator it = value.FirstIterator();

		while (it != end && *it != ch)
			++it;

		if (it == end)
		{
			out = value;
			return false;
		}

		out = StringUTF8(value.FirstIterator(), it);
		return true;
	}

	bool GetAfterLast(StringView value, String& out, char ch)
	{
		StringViewIterator end = value.AheadIterator();
		StringViewIterator it = value.LastIterator();

		while (it != end && *it != ch)
			--it;

		if (it == end)
		{
			out = value;
			return false;
		}

		out = String(++it, value.BehindIterator());
		return true;
	}
	bool GetAfterLast(StringViewUTF8 value, StringUTF8& out, UnicodeChar ch)
	{
		if (value.Empty())
		{
			out.Clear();
			return false;
		}

		StringViewUTF8Iterator end = value.FirstIterator();
		StringViewUTF8Iterator it = --value.BehindIterator();

		while (it != end && *it != ch)
			--it;

		if (it == end)
		{
			if (*it == ch)
			{
				out.Clear();
				return true;
			}

			out = value;
			return false;
		}

		out = StringUTF8(++it, value.BehindIterator());
		return true;
	}
	bool GetBeforeLast(StringView value, String& out, char ch)
	{
		StringViewIterator end = value.AheadIterator();
		StringViewIterator it = value.LastIterator();

		while (it != end && *it != ch)
			--it;

		if (it == end)
		{
			out = value;
			return false;
		}

		out = String(value.FirstIterator(), it);
		return true;
	}
	bool GetBeforeLast(StringViewUTF8 value, StringUTF8& out, UnicodeChar ch)
	{
		if (value.Empty())
		{
			out.Clear();
			return false;
		}

		StringViewUTF8Iterator end = value.FirstIterator();
		StringViewUTF8Iterator it = --value.BehindIterator();

		while (it != end && *it != ch)
			--it;

		if (it == end)
		{
			if (*it == ch)
			{
				out.Clear();
				return true;
			}

			out = value;
			return false;
		}

		out = StringUTF8(value.FirstIterator(), it);
		return true;
	}
	bool GetBeforeLastGroup(StringView value, String& out, char ch)
	{
		StringViewIterator end = value.AheadIterator();
		StringViewIterator it = value.LastIterator();

		while (it != end && *it != ch)
			--it;

		if (it == end)
		{
			out = value;
			return false;
		}

		while (it != end && *it == ch)
			--it;

		out = String(value.FirstIterator(), it);
		return true;
	}
	bool GetBeforeLastGroup(StringViewUTF8 value, StringUTF8& out, UnicodeChar ch)
	{
		StringViewUTF8Iterator end = value.FirstIterator();
		StringViewUTF8Iterator it = --value.BehindIterator();

		while (it != end && *it != ch)
			--it;

		if (it == end)
		{
			if (*it == ch)
			{
				out.Clear();
				return true;
			}

			out = value;
			return false;
		}

		while (it != end && *it == ch)
			--it;

		out = StringUTF8(value.FirstIterator(), it);
		return true;
	}

	String RemoveSurrounding(StringView value, char ch)
	{
		if (value.Empty())
			return String();

		if (value.Count() == 1)
			if (value.First() == ch)
				return String();
			else
				return value;

		if (value.First() == ch)
			if (value.Last() == ch)
				return value.SubString(1, value.Count() - 2);
			else
				return value.SubString(1, value.Count() - 1);
		else
			if (value.Last() == ch)
				return value.SubString(0, value.Count() - 1);
			else
				return value;
	}
	StringUTF8 RemoveSurrounding(StringViewUTF8 value, UnicodeChar ch)
	{
		if (value.Empty())
			return StringUTF8();

		if (value.CharacterCount() == 1)
			if (value.First() == ch)
				return StringUTF8();
			else
				return value;

		auto last = *--value.BehindIterator();
		if (value.First() == ch)
			if (last == ch)
				return StringUTF8(++value.FirstIterator(), --value.BehindIterator());
			else
				return StringUTF8(++value.FirstIterator(), value.BehindIterator());
		else
			if (last == ch)
				return StringUTF8(value.FirstIterator(), --value.BehindIterator());
			else
				return value;
	}
	String RemoveSurroundingGroup(StringView value, char ch)
	{
		if (value.Empty())
			return String();

		auto begin = value.FirstIterator();
		auto end = value.BehindIterator();

		while (begin != end && *begin == ch)
			++begin;

		if (begin == end)
			return String();

		--end;

		while (begin != end && *end == ch)
			--end;

		if (end == begin)
			return String();

		++end;

		return String(begin, end);
	}
	StringUTF8 RemoveSurroundingGroup(StringViewUTF8 value, UnicodeChar ch)
	{
		if (value.Empty())
			return StringUTF8();

		auto begin = value.FirstIterator();
		auto end = value.BehindIterator();

		while (begin != end && *begin == ch)
			++begin;

		if (begin == end)
			return StringUTF8();

		--end;

		while (begin != end && *end == ch)
			--end;

		if (end == begin)
			return StringUTF8();

		++end;

		return StringUTF8(begin, end);
	}

	Array<String> Split(StringView value, char ch)
	{
		Array<String> out;

		auto it = value.FirstIterator();
		auto end = value.BehindIterator();

		while (true)
		{
			while (it != end && *it == ch)
				++it;

			if (it == end)
				return out;

			auto begin = it;

			while (it != end && *it != ch)
				++it;

			out.AddBack(String(begin, it));
		}
	}
	Array<StringUTF8> Split(StringViewUTF8 value, UnicodeChar ch)
	{
		Array<StringUTF8> out;

		auto it = value.FirstIterator();
		auto end = value.BehindIterator();

		while (true)
		{
			while (it != end && *it == ch)
				++it;

			if (it == end)
				return out;

			auto begin = it;

			while (it != end && *it != ch)
				++it;

			out.AddBack(StringUTF8(begin, it));
		}
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
	Result CharsToNumber(const char* str, uintMem length, T& value, int base, uintMem* count)
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
	Result CharsToNumber(const char* str, uintMem length, T& value, FloatStringFormat format, uintMem* count)
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
	Result NumberToChars(T value, char* str, uintMem maxCount, int base, uintMem* count)
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
	Result NumberToChars(T value, char* str, uintMem maxCount, FloatStringFormat format, uintMem* count)
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

	Result Convert(const StringView& from, uint64& to, uint base, uintMem* count) { return CharsToNumber(from.Ptr(), from.Count(), to, base, count); }
	Result Convert(const StringViewUTF8& from, uint64& to, uint base, uintMem* count) { return CharsToNumber((const char*)from.Buffer(), from.BufferSize() - 1, to, base, count); }
	Result Convert(const StringView& from, int64& to, uint base, uintMem* count) { return CharsToNumber(from.Ptr(), from.Count(), to, base, count); }
	Result Convert(const StringViewUTF8& from, int64& to, uint base, uintMem* count) { return CharsToNumber((const char*)from.Buffer(), from.BufferSize() - 1, to, base, count); }
	Result Convert(const StringView& from, uint32& to, uint base, uintMem* count) { return CharsToNumber(from.Ptr(), from.Count(), to, base, count); }
	Result Convert(const StringViewUTF8& from, uint32& to, uint base, uintMem* count) { return CharsToNumber((const char*)from.Buffer(), from.BufferSize() - 1, to, base, count); }
	Result Convert(const StringView& from, int32& to, uint base, uintMem* count) { return CharsToNumber(from.Ptr(), from.Count(), to, base, count); }
	Result Convert(const StringViewUTF8& from, int32& to, uint base, uintMem* count) { return CharsToNumber((const char*)from.Buffer(), from.BufferSize() - 1, to, base, count); }
	Result Convert(const StringView& from, uint16& to, uint base, uintMem* count) { return CharsToNumber(from.Ptr(), from.Count(), to, base, count); }
	Result Convert(const StringViewUTF8& from, uint16& to, uint base, uintMem* count) { return CharsToNumber((const char*)from.Buffer(), from.BufferSize() - 1, to, base, count); }
	Result Convert(const StringView& from, int16& to, uint base, uintMem* count) { return CharsToNumber(from.Ptr(), from.Count(), to, base, count); }
	Result Convert(const StringViewUTF8& from, int16& to, uint base, uintMem* count) { return CharsToNumber((const char*)from.Buffer(), from.BufferSize() - 1, to, base, count); }
	Result Convert(const StringView& from, uint8& to, uint base, uintMem* count) { return CharsToNumber(from.Ptr(), from.Count(), to, base, count); }
	Result Convert(const StringViewUTF8& from, uint8& to, uint base, uintMem* count) { return CharsToNumber((const char*)from.Buffer(), from.BufferSize() - 1, to, base, count); }
	Result Convert(const StringView& from, int8& to, uint base, uintMem* count) { return CharsToNumber(from.Ptr(), from.Count(), to, base, count); }
	Result Convert(const StringViewUTF8& from, int8& to, uint base, uintMem* count) { return CharsToNumber((const char*)from.Buffer(), from.BufferSize() - 1, to, base, count); }
	Result Convert(const StringView& from, float& to, FloatStringFormat format, uintMem* count) { return CharsToNumber(from.Ptr(), from.Count(), to, format, count); }
	Result Convert(const StringViewUTF8& from, float& to, FloatStringFormat format, uintMem* count) { return CharsToNumber((const char*)from.Buffer(), from.BufferSize() - 1, to, format, count); }
	Result Convert(const StringView& from, double& to, FloatStringFormat format, uintMem* count) { return CharsToNumber(from.Ptr(), from.Count(), to, format, count); }
	Result Convert(const StringViewUTF8& from, double& to, FloatStringFormat format, uintMem* count) { return CharsToNumber((const char*)from.Buffer(), from.BufferSize() - 1, to, format, count); }


	template<std::integral T>
	String _Convert(T value, uint base, uintMem* count)
	{
		char buffer[64];

		uintMem _count;
		if (Result r = NumberToChars(value, buffer, 64, base, &_count))
			return "";

		if (count != nullptr)
			*count = _count;

		return String(buffer, _count);
	}
	template<std::floating_point T>
	String _Convert(T value, FloatStringFormat format, uintMem* count)
	{
		char buffer[64];

		uintMem _count;
		if (Result r = NumberToChars(value, buffer, 64, format, &_count))
			return "";

		if (count != nullptr)
			*count = _count;

		return String(buffer, _count);
	}

	String Convert(uint64 from, uint base, uintMem* count) { return _Convert(from, base, count); }
	String Convert(int64  from, uint base, uintMem* count) { return _Convert(from, base, count); }
	String Convert(uint32 from, uint base, uintMem* count) { return _Convert(from, base, count); }
	String Convert(int32  from, uint base, uintMem* count) { return _Convert(from, base, count); }
	String Convert(uint16 from, uint base, uintMem* count) { return _Convert(from, base, count); }
	String Convert(int16  from, uint base, uintMem* count) { return _Convert(from, base, count); }
	String Convert(uint8  from, uint base, uintMem* count) { return _Convert(from, base, count); }
	String Convert(int8   from, uint base, uintMem* count) { return _Convert(from, base, count); }
	String Convert(float  from, FloatStringFormat format, uintMem* count) { return _Convert(from, format, count); }
	String Convert(double from, FloatStringFormat format, uintMem* count) { return _Convert(from, format, count); }
	StringView Convert(const String& string)
	{
		return string;
	}
	StringView Convert(const StringView& string)
	{
		return string;
	}	
}