#include "pch.h"
#include "BlazeEngine/Core/String/StringTypeConverter.h"
#include "BlazeEngine/Core/String/UnicodeChar.h"

namespace Blaze
{
	static inline uint32_t DecodeUTF8(const char8_t* src, uintMem& i, uintMem size)
	{
		UnicodeChar ch{ static_cast<char32_t>(0xFFFD) };
		i += ch.FromChars(src + i, size - i);
		return static_cast<uint32_t>(ch.Value());
	}
	static inline uint32_t DecodeUTF16(const char16_t* src, uintMem& i, uintMem size)
	{
		UnicodeChar ch{ static_cast<char32_t>(0xFFFD) };
		i += ch.FromChars(src + i, size - i);
		return static_cast<uint32_t>(ch.Value());
	}
	static inline void EncodeUTF8(uint32_t cp, char8_t* dst, uintMem dstSize, uintMem& j)
	{
		UnicodeChar ch{ static_cast<char32_t>(cp) };
		auto result = ch.ToChars<char8_t>();
		for (uintMem i = 0; j < dstSize && i < result.count; ++j, ++i)
			dst[j] = result.chars[i];
	}
	static inline void EncodeUTF16(uint32_t cp, char16_t* dst, uintMem dstSize, uintMem& j)
	{
		UnicodeChar ch{ static_cast<char32_t>(cp) };
		auto result = ch.ToChars<char16_t>();
		for (uintMem i = 0; j < dstSize && i < result.count; ++j, ++i)
			dst[j] = result.chars[i];
	}
	static inline void EncodeUTF32(uint32_t cp, char32_t* dst, uintMem dstSize, uintMem& j)
	{
		if (j < dstSize) dst[j++] = static_cast<char32_t>(cp);
	}

	uintMem StringTypeConverter<char, char>::ConvertedSize(const char* str, uintMem count)
	{
		return count;
	}
	uintMem StringTypeConverter<char, char>::Convert(char* dst, uintMem dstSize, const char* src, uintMem srcSize)
	{
		if (dstSize < srcSize)
			srcSize = dstSize;

		for (uintMem i = 0; i < srcSize; ++i)
			dst[i] = src[i];

		return srcSize;
	}
	uintMem StringTypeConverter<char8_t, char>::ConvertedSize(const char* str, uintMem count)
	{
		return count;
	}
	uintMem StringTypeConverter<char8_t, char>::Convert(char8_t* dst, uintMem dstSize, const char* src, uintMem srcSize)
	{
		if (dstSize < srcSize)
			srcSize = dstSize;

		for (uintMem i = 0; i < srcSize; ++i)
			dst[i] = src[i];

		return srcSize;
	}
	uintMem StringTypeConverter<char16_t, char>::ConvertedSize(const char* str, uintMem count)
	{
		uintMem size = 0;
		uintMem i = 0;
		while (i < count)
		{
			uint32_t cp = DecodeUTF8(reinterpret_cast<const char8_t*>(str), i, count);
			size += (cp <= 0xFFFF ? 1 : 2);
		}
		return size;
	}
	uintMem StringTypeConverter<char16_t, char>::Convert(char16_t* dst, uintMem dstSize, const char* src, uintMem srcSize)
	{
		uintMem i = 0, j = 0;
		while (i < srcSize)
		{
			uint32_t cp = DecodeUTF8(reinterpret_cast<const char8_t*>(src), i, srcSize);
			EncodeUTF16(cp, dst, dstSize, j);
		}

		return j;
	}
	uintMem StringTypeConverter<char32_t, char>::ConvertedSize(const char* str, uintMem count)
	{
		uintMem size = 0, i = 0;
		while (i < count) { DecodeUTF8(reinterpret_cast<const char8_t*>(str), i, count); ++size; }
		return size;
	}
	uintMem StringTypeConverter<char32_t, char>::Convert(char32_t* dst, uintMem dstSize, const char* src, uintMem srcSize)
	{
		uintMem i = 0, j = 0;
		while (i < srcSize)
		{
			uint32_t cp = DecodeUTF8(reinterpret_cast<const char8_t*>(src), i, srcSize);
			EncodeUTF32(cp, dst, dstSize, j);
		}

		return j;
	}

	bool StringTypeConverter<char, char8_t>::CanConvert(const char8_t* src, uintMem srcSize)
	{
		for (uintMem i = 0; i < srcSize; ++i)
		{
			if (src[i] > 0x7F)
				return false;
		}
		return true;
	}
	uintMem StringTypeConverter<char, char8_t>::ConvertedSize(const char8_t* str, uintMem count)
	{
		return count;
	}
	uintMem StringTypeConverter<char, char8_t>::Convert(char* dst, uintMem dstSize, const char8_t* src, uintMem srcSize)
	{
		if (dstSize < srcSize)
			srcSize = dstSize;

		for (uintMem i = 0; i < srcSize; ++i)
			dst[i] = src[i];

		return srcSize;
	}
	uintMem StringTypeConverter<char8_t, char8_t>::ConvertedSize(const char8_t* str, uintMem count)
	{
		return count;
	}
	uintMem StringTypeConverter<char8_t, char8_t>::Convert(char8_t* dst, uintMem dstSize, const char8_t* src, uintMem srcSize)
	{
		if (dstSize < srcSize)
			srcSize = dstSize;

		for (uintMem i = 0; i < srcSize; ++i)
			dst[i] = src[i];

		return srcSize;
	}
	uintMem StringTypeConverter<char16_t, char8_t>::ConvertedSize(const char8_t* str, uintMem count)
	{
		uintMem size = 0, i = 0;
		while (i < count)
		{
			uint32_t cp = DecodeUTF8(str, i, count);
			size += (cp <= 0xFFFF ? 1 : 2);
		}
		return size;
	}
	uintMem StringTypeConverter<char16_t, char8_t>::Convert(char16_t* dst, uintMem dstSize, const char8_t* src, uintMem srcSize)
	{
		uintMem i = 0, j = 0;
		while (i < srcSize)
		{
			uint32_t cp = DecodeUTF8(src, i, srcSize);
			EncodeUTF16(cp, dst, dstSize, j);
		}

		return j;
	}
	uintMem StringTypeConverter<char32_t, char8_t>::ConvertedSize(const char8_t* str, uintMem count)
	{
		uintMem size = 0, i = 0;
		while (i < count) { DecodeUTF8(str, i, count); ++size; }
		return size;
	}
	uintMem StringTypeConverter<char32_t, char8_t>::Convert(char32_t* dst, uintMem dstSize, const char8_t* src, uintMem srcSize)
	{
		uintMem i = 0, j = 0;
		while (i < srcSize)
		{
			uint32_t cp = DecodeUTF8(src, i, srcSize);
			if (j < dstSize)
				dst[j++] = static_cast<char32_t>(cp);
		}

		return j;
	}

	bool StringTypeConverter<char, char16_t>::CanConvert(const char16_t* src, uintMem srcSize)
	{
		for (uintMem i = 0; i < srcSize; ++i)
		{
			if (src[i] > 0x7F)
				return false;
		}
		return true;
	}
	uintMem StringTypeConverter<char, char16_t>::ConvertedSize(const char16_t* src, uintMem srcSize)
	{
		return srcSize;
	}
	uintMem StringTypeConverter<char, char16_t>::Convert(char* dst, uintMem dstSize, const char16_t* src, uintMem srcSize)
	{
		if (dstSize < srcSize)
			srcSize = dstSize;

		for (uintMem i = 0; i < srcSize; ++i)
			dst[i] = static_cast<char>(src[i]);

		return srcSize;
	}
	uintMem StringTypeConverter<char8_t, char16_t>::ConvertedSize(const char16_t* str, uintMem count)
	{
		uintMem size = 0, i = 0;
		while (i < count)
		{
			uint32_t cp = DecodeUTF16(str, i, count);
			if (cp < 0x80) size += 1;
			else if (cp < 0x800) size += 2;
			else if (cp < 0x10000) size += 3;
			else size += 4;
		}
		return size;
	}
	uintMem StringTypeConverter<char8_t, char16_t>::Convert(char8_t* dst, uintMem dstSize, const char16_t* src, uintMem srcSize)
	{
		uintMem i = 0, j = 0;
		while (i < srcSize)
		{
			uint32_t cp = DecodeUTF16(src, i, srcSize);
			EncodeUTF8(cp, dst, dstSize, j);
		}

		return j;
	}
	uintMem StringTypeConverter<char16_t, char16_t>::ConvertedSize(const char16_t* str, uintMem count)
	{
		return count;
	}
	uintMem StringTypeConverter<char16_t, char16_t>::Convert(char16_t* dst, uintMem dstSize, const char16_t* src, uintMem srcSize)
	{
		if (dstSize < srcSize)
			srcSize = dstSize;

		for (uintMem i = 0; i < srcSize; ++i)
			dst[i] = src[i];

		return srcSize;
	}
	uintMem StringTypeConverter<char32_t, char16_t>::ConvertedSize(const char16_t* str, uintMem count)
	{
		uintMem size = 0;
		for (uintMem i = 0; i < count; ++i)
		{
			char16_t c = str[i];
			if (c >= 0xD800 && c <= 0xDBFF && i + 1 < count)
				++i; // surrogate pair
			++size;
		}
		return size;
	}
	uintMem StringTypeConverter<char32_t, char16_t>::Convert(char32_t* dst, uintMem dstSize, const char16_t* src, uintMem srcSize)
	{
		uintMem j = 0;
		for (uintMem i = 0; i < srcSize; ++i)
		{
			char16_t c = src[i];
			if (c >= 0xD800 && c <= 0xDBFF && i + 1 < srcSize)
			{
				char16_t c2 = src[++i];
				uint32_t cp = 0x10000 + (((c - 0xD800) << 10) | (c2 - 0xDC00));
				EncodeUTF32(cp, dst, dstSize, j);
			}
			else
			{
				EncodeUTF32(c, dst, dstSize, j);
			}
		}

		return j;
	}

	bool StringTypeConverter<char, char32_t>::CanConvert(const char32_t* src, uintMem srcSize)
	{
		for (uintMem i = 0; i < srcSize; ++i)
		{
			if (src[i] > 0x7F)
				return false;
		}
		return true;
	}
	uintMem StringTypeConverter<char, char32_t>::ConvertedSize(const char32_t* src, uintMem srcSize)
	{
		return srcSize;
	}
	uintMem StringTypeConverter<char, char32_t>::Convert(char* dst, uintMem dstSize, const char32_t* src, uintMem srcSize)
	{
		if (dstSize < srcSize)
			srcSize = dstSize;

		for (uintMem i = 0; i < srcSize; ++i)
			dst[i] = static_cast<char>(src[i]);

		return srcSize;
	}
	uintMem StringTypeConverter<char8_t, char32_t>::ConvertedSize(const char32_t* str, uintMem count)
	{
		uintMem size = 0;
		for (uintMem i = 0; i < count; ++i)
		{
			uint32_t cp = str[i];
			if (cp < 0x80) size += 1;
			else if (cp < 0x800) size += 2;
			else if (cp < 0x10000) size += 3;
			else size += 4;
		}
		return size;
	}
	uintMem StringTypeConverter<char8_t, char32_t>::Convert(char8_t* dst, uintMem dstSize, const char32_t* src, uintMem srcSize)
	{
		uintMem j = 0;
		for (uintMem i = 0; i < srcSize; ++i)
			EncodeUTF8(src[i], dst, dstSize, j);

		return j;
	}
	uintMem StringTypeConverter<char16_t, char32_t>::ConvertedSize(const char32_t* str, uintMem count)
	{
		uintMem size = 0;
		for (uintMem i = 0; i < count; ++i)
			size += (str[i] <= 0xFFFF ? 1 : 2);
		return size;
	}
	uintMem StringTypeConverter<char16_t, char32_t>::Convert(char16_t* dst, uintMem dstSize, const char32_t* src, uintMem srcSize)
	{
		uintMem j = 0;
		for (uintMem i = 0; i < srcSize; ++i)
			EncodeUTF16(src[i], dst, dstSize, j);

		return j;
	}
	uintMem StringTypeConverter<char32_t, char32_t>::ConvertedSize(const char32_t* str, uintMem count)
	{
		return count;
	}
	uintMem StringTypeConverter<char32_t, char32_t>::Convert(char32_t* dst, uintMem dstSize, const char32_t* src, uintMem srcSize)
	{
		if (dstSize < srcSize)
			srcSize = dstSize;

		for (uintMem i = 0; i < srcSize; ++i)
			dst[i] = src[i];

		return srcSize;
	}
}