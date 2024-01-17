#pragma once
#include "BlazeEngineCore/DataStructures/StringUTF8.h"
#include "BlazeEngineCore/DataStructures/Array.h"
#include "BlazeEngineCore/DataStructures/ArrayImpl.h"

namespace Blaze::StringParsing
{
	//If there is a occurrence of <ch>, <first> is set to the string before the first occurrence,
	//<second> is set to the string after the first occurrence of <ch> and true is returned.
	//If there is no occurrence of <ch>, <first> is set to <value>, and <second> is set to a empty string,
	//and false is returned
	BLAZE_CORE_API bool SplitAtFirst(StringView value, String& first, String& second, char ch);
	//If there is a occurrence of <ch>, <first> is set to the string before the first occurrence,
	//<second> is set to the string after the first occurrence of <ch> and true is returned.
	//If there is no occurrence of <ch>, <first> is set to <value>, and <second> is set to a empty string,
	//and false is returned
	BLAZE_CORE_API bool SplitAtFirst(StringViewUTF8 value, StringUTF8& first, StringUTF8& second, UnicodeChar ch);
	//If there is a occurrence of <ch>, <first> is set to the string before the first occurrence
	//and <second> is set to the string after the last occurrence in the first group and true is returned.
	//If there is no occurrence of <ch>, <first> is set to <value>, and <second> is set to a empty string,
	//and false is returned
	BLAZE_CORE_API bool SplitAtFirstGroup(StringView value, String& first, String& second, char ch);
	//If there is a occurrence of <ch>, <first> is set to the string before the first occurrence
	//and <second> is set to the string after the last occurrence in the first group and true is returned.
	//If there is no occurrence of <ch>, <first> is set to <value>, and <second> is set to a empty string
	//and false is returned
	BLAZE_CORE_API bool SplitAtFirstGroup(StringViewUTF8 value, StringUTF8& first, StringUTF8& second, UnicodeChar ch);
	
	//If there is a occurrence of <ch>, <out> is set to the string after the first occurrence and
	//true is returned. If there is no occurrence of <ch>, <out> is set to <value> and false is returned
	BLAZE_CORE_API bool GetAfterFirst(StringView value, String& out, char ch);
	//If there is a occurrence of <ch>, <out> is set to the string after the first occurrence and
	//true is returned. If there is no occurrence of <ch>, <out> is set to <value> and false is returned
	BLAZE_CORE_API bool GetAfterFirst(StringViewUTF8 value, StringUTF8& out, UnicodeChar ch);
	//If there is a occurrence of <ch>, <out> is set to the string after the last occurrence in the
	//first group and true is returned. If there is no occurrence of <ch>, <out> is set to <value> and 
	//false is returned
	BLAZE_CORE_API bool GetAfterFirstGroup(StringView value, String& out, char ch);
	//If there is a occurrence of <ch>, <out> is set to the string after the last occurrence in the
	//first group and true is returned. If there is no occurrence of <ch>, <out> is set to <value> and 
	//false is returned
	BLAZE_CORE_API bool GetAfterFirstGroup(StringViewUTF8 value, StringUTF8& out, UnicodeChar ch);
	//If there is a occurrence of <ch>, <out> is set to the string before the first occurrence and true 
	//is returned. If there is no occurrence of <ch>, <out> is set to <value> and false is returned
	BLAZE_CORE_API bool GetBeforeFirst(StringView value, String& out, char ch);
	//If there is a occurrence of <ch>, <out> is set to the string before the first occurrence and true 
	//is returned. If there is no occurrence of <ch>, <out> is set to <value> and false is returned
	BLAZE_CORE_API bool GetBeforeFirst(StringViewUTF8 value, StringUTF8& out, UnicodeChar ch);
	
	//If there is a occurrence of <ch>, <out> is set to the string after the last occurrence and
	//true is returned. If there is no occurrence of <ch>, <out> is set to <value> and false is returned
	BLAZE_CORE_API bool GetAfterLast(StringView value, String& out, char ch);
	//If there is a occurrence of <ch>, <out> is set to the string after the last occurrence and
	//true is returned. If there is no occurrence of <ch>, <out> is set to <value> and false is returned
	BLAZE_CORE_API bool GetAfterLast(StringViewUTF8 value, StringUTF8& out, UnicodeChar ch);
	//If there is a occurrence of <ch>, <out> is set to the string before the last occurrence and true 
	//is returned. If there is no occurrence of <ch>, <out> is set to <value> and false is returned
	BLAZE_CORE_API bool GetBeforeLast(StringView value, String& out, char ch);
	//If there is a occurrence of <ch>, <out> is set to the string before the last occurrence and true 
	//is returned. If there is no occurrence of <ch>, <out> is set to <value> and false is returned
	BLAZE_CORE_API bool GetBeforeLast(StringViewUTF8 value, StringUTF8& out, UnicodeChar ch);
	//If there is a occurrence of <ch>, <out> is set to the string before the first occurrence in the
	//last group and true is returned. If there is no occurrence of <ch>, <out> is set to <value> and 
	//false is returned
	BLAZE_CORE_API bool GetBeforeLastGroup(StringView value, String& out, char ch);
	//If there is a occurrence of <ch>, <out> is set to the string before the first occurrence in the
	//last group and true is returned. If there is no occurrence of <ch>, <out> is set to <value> and 
	//false is returned
	BLAZE_CORE_API bool GetBeforeLastGroup(StringViewUTF8 value, StringUTF8& out, UnicodeChar ch);
	
	//If the first and/or last characters of <value> are <ch>, they are removed
	BLAZE_CORE_API String RemoveSurrounding(StringView value, char ch);
	//If the first and/or last characters of <value> are <ch>, they are removed
	BLAZE_CORE_API StringUTF8 RemoveSurrounding(StringViewUTF8 value, UnicodeChar ch);
	//If the first and/or last characters of <value> are <ch>, the whole group
	//of <ch> characters is removed
	BLAZE_CORE_API String RemoveSurroundingGroup(StringView value, char ch);
	//If the first and/or last characters of <value> are <ch>, the whole group
	//of <ch> characters is removed
	BLAZE_CORE_API StringUTF8 RemoveSurroundingGroup(StringViewUTF8 value, UnicodeChar ch);

	//If there is a occurrence of <ch>, the string is split into substrings that don't containt
	//the character <ch>. If there is no occurrence of <ch>, array with <value> as the only element
	//is returned
	BLAZE_CORE_API Array<String> Split(StringView value, char ch);
	//If there is a occurrence of <ch>, the string is split into substrings that don't containt
	//the character <ch>. If there is no occurrence of <ch>, array with <value> as the only element
	//is returned
	BLAZE_CORE_API Array<StringUTF8> Split(StringViewUTF8 value, UnicodeChar ch);

	enum class FloatStringFormat
	{
		Scientific = 1,
		Fixed = 2,
		Hex = 4,
		Default = Fixed | Scientific
	};

	ENUM_CLASS_BITWISE_OPERATIONS(FloatStringFormat)	

	BLAZE_CORE_API Result Convert(const StringView& from, uint64& to, uint base = 10, uintMem* count = nullptr);
	BLAZE_CORE_API Result Convert(const StringViewUTF8& from, uint64& to, uint base = 10, uintMem* count = nullptr);
	BLAZE_CORE_API Result Convert(const StringView& from, int64& to, uint base = 10, uintMem* count = nullptr);
	BLAZE_CORE_API Result Convert(const StringViewUTF8& from, int64& to, uint base = 10, uintMem* count = nullptr);
	BLAZE_CORE_API Result Convert(const StringView& from, uint32& to, uint base = 10, uintMem* count = nullptr);
	BLAZE_CORE_API Result Convert(const StringViewUTF8& from, uint32& to, uint base = 10, uintMem* count = nullptr);
	BLAZE_CORE_API Result Convert(const StringView& from, int32& to, uint base = 10, uintMem* count = nullptr);
	BLAZE_CORE_API Result Convert(const StringViewUTF8& from, int32& to, uint base = 10, uintMem* count = nullptr);
	BLAZE_CORE_API Result Convert(const StringView& from, uint16& to, uint base = 10, uintMem* count = nullptr);
	BLAZE_CORE_API Result Convert(const StringViewUTF8& from, uint16& to, uint base = 10, uintMem* count = nullptr);
	BLAZE_CORE_API Result Convert(const StringView& from, int16& to, uint base = 10, uintMem* count = nullptr);
	BLAZE_CORE_API Result Convert(const StringViewUTF8& from, int16& to, uint base = 10, uintMem* count = nullptr);
	BLAZE_CORE_API Result Convert(const StringView& from, uint8& to, uint base = 10, uintMem* count = nullptr);
	BLAZE_CORE_API Result Convert(const StringViewUTF8& from, uint8& to, uint base = 10, uintMem* count = nullptr);
	BLAZE_CORE_API Result Convert(const StringView& from, int8& to, uint base = 10, uintMem* count = nullptr);
	BLAZE_CORE_API Result Convert(const StringViewUTF8& from, int8& to, uint base = 10, uintMem* count = nullptr);
	BLAZE_CORE_API Result Convert(const StringView& from, float& to, FloatStringFormat format = FloatStringFormat::Default, uintMem* count = nullptr);
	BLAZE_CORE_API Result Convert(const StringViewUTF8& from, float& to, FloatStringFormat format = FloatStringFormat::Default, uintMem* count = nullptr);
	BLAZE_CORE_API Result Convert(const StringView& from, double& to, FloatStringFormat format = FloatStringFormat::Default, uintMem* count = nullptr);
	BLAZE_CORE_API Result Convert(const StringViewUTF8& from, double& to, FloatStringFormat format = FloatStringFormat::Default, uintMem* count = nullptr);

	BLAZE_CORE_API String Convert(uint64 value, uint base = 10, uintMem* count = nullptr);
	BLAZE_CORE_API String Convert(int64  value, uint base = 10, uintMem* count = nullptr);
	BLAZE_CORE_API String Convert(uint32 value, uint base = 10, uintMem* count = nullptr);
	BLAZE_CORE_API String Convert(int32  value, uint base = 10, uintMem* count = nullptr);
	BLAZE_CORE_API String Convert(uint16 value, uint base = 10, uintMem* count = nullptr);
	BLAZE_CORE_API String Convert(int16  value, uint base = 10, uintMem* count = nullptr);
	BLAZE_CORE_API String Convert(uint8  value, uint base = 10, uintMem* count = nullptr);
	BLAZE_CORE_API String Convert(int8   value, uint base = 10, uintMem* count = nullptr);
	BLAZE_CORE_API String Convert(float  value, FloatStringFormat format = FloatStringFormat::Default, uintMem* count = nullptr);
	BLAZE_CORE_API String Convert(double value, FloatStringFormat format = FloatStringFormat::Default, uintMem* count = nullptr);	

	BLAZE_CORE_API StringView Convert(const String& string);
	BLAZE_CORE_API StringView Convert(const StringView& string);			
	template<uintMem C>
	StringView Convert(const char (&arr)[C]);

	template<typename T>
	concept IsConvertibleToString = requires (const T& x) {
		(String)StringParsing::Convert(x);
	} || requires (T x) {
		String(x);
	};

	template<typename T>
	concept IsConvertibleToStringUTF8 = requires (T x) {
		(StringUTF8)StringParsing::Convert(x);
	} || requires (T x) {
		StringUTF8(x);
	};

	template<typename ... T> requires (IsConvertibleToString<T> && ...)
	String Merge(T ... values)
	{
		return (((String)Convert(std::forward<T>(values))) + ...);
	}

	template<typename ... T> requires (IsConvertibleToStringUTF8<T> && ...)
	StringUTF8 MergeUTF8(T ... values)
	{
		return (((StringUTF8)Convert(values)) + ...);
	}

	template<uintMem C>
	StringView Convert(const char(&arr)[C])
	{
		return StringView(arr, C - 1);
	}
}