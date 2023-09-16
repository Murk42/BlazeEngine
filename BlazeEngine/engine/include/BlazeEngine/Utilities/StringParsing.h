#pragma once

namespace Blaze::StringParsing
{
	BLAZE_API bool SplitAtFirst(StringView value, String& first, String& second, char ch);
	
	BLAZE_API bool GetAfterFirst(StringView value, String& out, char ch);
	BLAZE_API bool GetBeforeFirst(StringView value, String& out, char ch);
	
	BLAZE_API bool GetAfterLast(StringView value, String& out, char ch);
	BLAZE_API bool GetBeforeLast(StringView value, String& out, char ch);
	
	BLAZE_API bool RemoveQuotes(StringView value, String& out);
	BLAZE_API bool RemoveBrackets(StringView value, String& out, char right = '(', char left = ')');
	BLAZE_API void RemoveSpace(StringView value, String& out);

	BLAZE_API Array<String> Split(StringView value, char ch);

	enum class FloatStringFormat
	{
		Scientific = 1,
		Fixed = 2,
		Hex = 4,
		Default = Fixed | Scientific
	};

	ENUM_CLASS_BITWISE_OPERATIONS(FloatStringFormat)

	inline String Convert(const String& value) { return value; }
	inline String Convert(String&& value) { return value; }
	inline String Convert(const StringView& value) { return value; }
	inline String Convert(const char* ptr) { return ptr; }	

	Result BLAZE_API Convert(const StringView& from, uint64& to, uint base = 10, uint* count = nullptr);
	Result BLAZE_API Convert(const StringView& from, int64&  to, uint base = 10, uint* count = nullptr);
	Result BLAZE_API Convert(const StringView& from, uint32& to, uint base = 10, uint* count = nullptr);
	Result BLAZE_API Convert(const StringView& from, int32&  to, uint base = 10, uint* count = nullptr);
	Result BLAZE_API Convert(const StringView& from, uint16& to, uint base = 10, uint* count = nullptr);
	Result BLAZE_API Convert(const StringView& from, int16&  to, uint base = 10, uint* count = nullptr);
	Result BLAZE_API Convert(const StringView& from, uint8&  to, uint base = 10, uint* count = nullptr);
	Result BLAZE_API Convert(const StringView& from, int8&   to, uint base = 10, uint* count = nullptr);
	Result BLAZE_API Convert(const StringView& from, float&  to, FloatStringFormat format = FloatStringFormat::Default, uint* count = nullptr);
	Result BLAZE_API Convert(const StringView& from, double& to, FloatStringFormat format = FloatStringFormat::Default, uint* count = nullptr);

	String BLAZE_API Convert(uint64 value, uint base = 10, uint* count = nullptr);
	String BLAZE_API Convert(int64  value, uint base = 10, uint* count = nullptr);
	String BLAZE_API Convert(uint32 value, uint base = 10, uint* count = nullptr);
	String BLAZE_API Convert(int32  value, uint base = 10, uint* count = nullptr);
	String BLAZE_API Convert(uint16 value, uint base = 10, uint* count = nullptr);
	String BLAZE_API Convert(int16  value, uint base = 10, uint* count = nullptr);
	String BLAZE_API Convert(uint8  value, uint base = 10, uint* count = nullptr);
	String BLAZE_API Convert(int8   value, uint base = 10, uint* count = nullptr);
	String BLAZE_API Convert(float  value, FloatStringFormat format = FloatStringFormat::Default, uint* count = nullptr);
	String BLAZE_API Convert(double value, FloatStringFormat format = FloatStringFormat::Default, uint* count = nullptr);

	template<typename T>
	concept IsConvertibleToString = requires (T x) {
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
}