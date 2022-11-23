#include "BlazeEngine/Utilities/StringParsing.h"

#include <cctype>

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

	bool ConvertTo(const StringView& sv, uint64& value, uint base)
	{
		String t;
		RemoveSpace(sv, t);

		if (t.Size() == 0) return false;

		char* ptr;
		uint64 temp = strtoull(t.Ptr(), &ptr, base);

		if (ptr == t.end())
		{
			value = temp;
			return true;
		}

		return false;
	}
}