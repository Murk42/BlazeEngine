#pragma once
#include "BlazeEngine/DataStructures/Array.h"

namespace Blaze::StringParsing
{
	bool BLAZE_API SplitAtFirst(StringView value, String& first, String& second, char ch);

	bool BLAZE_API GetAfterFirst(StringView value, String& out, char ch);
	bool BLAZE_API GetBeforeFirst(StringView value, String& out, char ch);

	bool BLAZE_API GetAfterLast(StringView value, String& out, char ch);
	bool BLAZE_API GetBeforeLast(StringView value, String& out, char ch);

	bool BLAZE_API RemoveQuotes(StringView value, String& out);
	bool BLAZE_API RemoveBrackets(StringView value, String& out, char right = '(', char left = ')');
	void BLAZE_API RemoveSpace(StringView value, String& out);

	Array<String> BLAZE_API Split(StringView value, char ch);

	enum class FloatStringFormat
	{
		Scientific = 1,
		Fixed = 2,
		Hex = 4,
		Default = Fixed | Scientific
	};

	ENUM_CLASS_BITWISE_OPERATIONS(FloatStringFormat)

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

	ResultValue<String> BLAZE_API Convert(uint64 value, uint base = 10, uint* count = nullptr);
	ResultValue<String> BLAZE_API Convert(int64  value, uint base = 10, uint* count = nullptr);
	ResultValue<String> BLAZE_API Convert(uint32 value, uint base = 10, uint* count = nullptr);
	ResultValue<String> BLAZE_API Convert(int32  value, uint base = 10, uint* count = nullptr);
	ResultValue<String> BLAZE_API Convert(uint16 value, uint base = 10, uint* count = nullptr);
	ResultValue<String> BLAZE_API Convert(int16  value, uint base = 10, uint* count = nullptr);
	ResultValue<String> BLAZE_API Convert(uint8  value, uint base = 10, uint* count = nullptr);
	ResultValue<String> BLAZE_API Convert(int8   value, uint base = 10, uint* count = nullptr);
	ResultValue<String> BLAZE_API Convert(float  value, FloatStringFormat format = FloatStringFormat::Default, uint* count = nullptr);
	ResultValue<String> BLAZE_API Convert(double value, FloatStringFormat format = FloatStringFormat::Default, uint* count = nullptr);
}