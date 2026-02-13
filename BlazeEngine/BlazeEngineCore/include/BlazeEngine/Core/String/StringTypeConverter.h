#pragma once
#include "BlazeEngine/Core/String/StringCommon.h"

namespace Blaze
{
	template<>
	struct BLAZE_API StringTypeConverter<char, char>
	{
		inline bool CanConvert(const char* src, uintMem srcSize) { return true; }
		uintMem ConvertedSize(const char* str, uintMem count);
		uintMem Convert(char* dst, uintMem dstSize, const char* src, uintMem srcSize);
	};
	template<>
	struct BLAZE_API StringTypeConverter<char8_t, char>
	{
		inline bool CanConvert(const char* src, uintMem srcSize) { return true; }
		uintMem ConvertedSize(const char* str, uintMem count);
		uintMem Convert(char8_t* dst, uintMem dstSize, const char* src, uintMem srcSize);
	};
	template<>
	struct BLAZE_API StringTypeConverter<char16_t, char>
	{
		inline bool CanConvert(const char* src, uintMem srcSize) { return true; }
		uintMem ConvertedSize(const char* str, uintMem count);
		uintMem Convert(char16_t* dst, uintMem dstSize, const char* src, uintMem srcSize);
	};
	template<>
	struct BLAZE_API StringTypeConverter<char32_t, char>
	{
		inline bool CanConvert(const char* src, uintMem srcSize) { return true; }
		uintMem ConvertedSize(const char* str, uintMem count);
		uintMem Convert(char32_t* dst, uintMem dstSize, const char* src, uintMem srcSize);
	};

	template<>
	struct BLAZE_API StringTypeConverter<char, char8_t>
	{
		bool CanConvert(const char8_t* src, uintMem srcSize);
		uintMem ConvertedSize(const char8_t* str, uintMem count);
		uintMem Convert(char* dst, uintMem dstSize, const char8_t* src, uintMem srcSize);
	};
	template<>
	struct BLAZE_API StringTypeConverter<char8_t, char8_t>
	{
		inline bool CanConvert(const char8_t* src, uintMem srcSize) { return true; }
		uintMem ConvertedSize(const char8_t* str, uintMem count);
		uintMem Convert(char8_t* dst, uintMem dstSize, const char8_t* src, uintMem srcSize);
	};
	template<>
	struct BLAZE_API StringTypeConverter<char16_t, char8_t>
	{
		inline bool CanConvert(const char8_t* src, uintMem srcSize) { return true; }
		uintMem ConvertedSize(const char8_t* str, uintMem count);
		uintMem Convert(char16_t* dst, uintMem dstSize, const char8_t* src, uintMem srcSize);
	};
	template<>
	struct BLAZE_API StringTypeConverter<char32_t, char8_t>
	{
		inline bool CanConvert(const char8_t* src, uintMem srcSize) { return true; }
		uintMem ConvertedSize(const char8_t* str, uintMem count);
		uintMem Convert(char32_t* dst, uintMem dstSize, const char8_t* src, uintMem srcSize);
	};

	template<>
	struct BLAZE_API StringTypeConverter<char, char16_t>
	{
		bool CanConvert(const char16_t* src, uintMem srcSize);
		uintMem ConvertedSize(const char16_t* str, uintMem count);
		uintMem Convert(char* dst, uintMem dstSize, const char16_t* src, uintMem srcSize);
	};
	template<>
	struct BLAZE_API StringTypeConverter<char8_t, char16_t>
	{
		inline bool CanConvert(const char16_t* src, uintMem srcSize) { return true; }
		uintMem ConvertedSize(const char16_t* str, uintMem count);
		uintMem Convert(char8_t* dst, uintMem dstSize, const char16_t* src, uintMem srcSize);
	};
	template<>
	struct BLAZE_API StringTypeConverter<char16_t, char16_t>
	{
		inline bool CanConvert(const char16_t* src, uintMem srcSize) { return true; }
		uintMem ConvertedSize(const char16_t* str, uintMem count);
		uintMem Convert(char16_t* dst, uintMem dstSize, const char16_t* src, uintMem srcSize);
	};
	template<>
	struct BLAZE_API StringTypeConverter<char32_t, char16_t>
	{
		inline bool CanConvert(const char16_t* src, uintMem srcSize) { return true; }
		uintMem ConvertedSize(const char16_t* str, uintMem count);
		uintMem Convert(char32_t* dst, uintMem dstSize, const char16_t* src, uintMem srcSize);
	};

	template<>
	struct BLAZE_API StringTypeConverter<char, char32_t>
	{
		bool CanConvert(const char32_t* src, uintMem srcSize);
		uintMem ConvertedSize(const char32_t* str, uintMem count);
		uintMem Convert(char* dst, uintMem dstSize, const char32_t* src, uintMem srcSize);
	};
	template<>
	struct BLAZE_API StringTypeConverter<char8_t, char32_t>
	{
		inline bool CanConvert(const char32_t* src, uintMem srcSize) { return true; }
		uintMem ConvertedSize(const char32_t* str, uintMem count);
		uintMem Convert(char8_t* dst, uintMem dstSize, const char32_t* src, uintMem srcSize);
	};
	template<>
	struct BLAZE_API StringTypeConverter<char16_t, char32_t>
	{
		inline bool CanConvert(const char32_t* src, uintMem srcSize) { return true; }
		uintMem ConvertedSize(const char32_t* str, uintMem count);
		uintMem Convert(char16_t* dst, uintMem dstSize, const char32_t* src, uintMem srcSize);
	};
	template<>
	struct BLAZE_API StringTypeConverter<char32_t, char32_t>
	{
		inline bool CanConvert(const char32_t* src, uintMem srcSize) { return true; }
		uintMem ConvertedSize(const char32_t* str, uintMem count);
		uintMem Convert(char32_t* dst, uintMem dstSize, const char32_t* src, uintMem srcSize);
	};
}