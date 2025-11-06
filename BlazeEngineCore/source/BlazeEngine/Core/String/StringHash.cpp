#include "pch.h"
#include "BlazeEngine/Core/String/StringHash.h"

namespace Blaze
{
	constexpr uint64 FNV_OFFSET_BASIS = 1469598103934665603ULL;
	constexpr uint64 FNV_PRIME = 1099511628211ULL;


	static inline uint32_t DecodeUTF8(const char8_t* src, uintMem& i, uintMem size)
	{
		// If no bytes left, return replacement
		if (i >= size) return 0xFFFD;

		// Read first byte as an unsigned value
		uint32_t b0 = static_cast<uint8_t>(src[i++]);

		// 1-byte (ASCII)
		if (b0 < 0x80) return b0;

		// 2-byte
		if ((b0 >> 5) == 0x6)
		{
			if (i >= size) return 0xFFFD; // missing continuation
			uint32_t b1 = static_cast<uint8_t>(src[i]);
			if ((b1 & 0xC0) != 0x80) { ++i; return 0xFFFD; } // invalid continuation (consume it to avoid infinite loop)
			++i;
			uint32_t cp = ((b0 & 0x1F) << 6) | (b1 & 0x3F);
			// reject overlong (must be >= 0x80)
			if (cp < 0x80) return 0xFFFD;
			return cp;
		}

		// 3-byte
		if ((b0 >> 4) == 0xE)
		{
			if (i + 1 >= size) return 0xFFFD; // need two continuation bytes
			uint32_t b1 = static_cast<uint8_t>(src[i]);
			uint32_t b2 = static_cast<uint8_t>(src[i + 1]);
			if ((b1 & 0xC0) != 0x80 || (b2 & 0xC0) != 0x80)
			{
				// consume one invalid continuation to progress
				++i;
				return 0xFFFD;
			}
			i += 2;
			uint32_t cp = ((b0 & 0x0F) << 12) | ((b1 & 0x3F) << 6) | (b2 & 0x3F);
			// reject overlong and surrogates (surrogates are invalid in UTF-8)
			if (cp < 0x800 || (cp >= 0xD800 && cp <= 0xDFFF)) return 0xFFFD;
			return cp;
		}

		// 4-byte
		if ((b0 >> 3) == 0x1E)
		{
			if (i + 2 >= size) return 0xFFFD; // need three continuation bytes
			uint32_t b1 = static_cast<uint8_t>(src[i]);
			uint32_t b2 = static_cast<uint8_t>(src[i + 1]);
			uint32_t b3 = static_cast<uint8_t>(src[i + 2]);
			if ((b1 & 0xC0) != 0x80 || (b2 & 0xC0) != 0x80 || (b3 & 0xC0) != 0x80)
			{
				++i;
				return 0xFFFD;
			}
			i += 3;
			uint32_t cp = ((b0 & 0x07) << 18) | ((b1 & 0x3F) << 12) | ((b2 & 0x3F) << 6) | (b3 & 0x3F);
			// valid range: U+10000 .. U+10FFFF
			if (cp < 0x10000 || cp > 0x10FFFF) return 0xFFFD;
			return cp;
		}

		// otherwise invalid leading byte
		return 0xFFFD;
	}
	static inline uint32_t DecodeUTF16(const char16_t* src, uintMem& i, uintMem size)
	{
		if (i >= size) return 0xFFFD;
		uint32_t w1 = static_cast<uint16_t>(src[i++]);

		// high surrogate
		if (w1 >= 0xD800 && w1 <= 0xDBFF)
		{
			if (i < size)
			{
				uint32_t w2 = static_cast<uint16_t>(src[i]);
				if (w2 >= 0xDC00 && w2 <= 0xDFFF)
				{
					++i;
					return 0x10000 + (((w1 - 0xD800) << 10) | (w2 - 0xDC00));
				}
			}
			// unpaired high surrogate -> replacement
			return 0xFFFD;
		}

		// isolated low surrogate -> replacement
		if (w1 >= 0xDC00 && w1 <= 0xDFFF) return 0xFFFD;

		return w1;
	}

	inline uint64 HashUpdate(uint64 hash, char32_t codepoint)
	{
		// process as 4 bytes (LE) so that U+00000001 != U+00000100
		for (int i = 0; i < 4; ++i) {
			uint8 byte = (codepoint >> (i * 8)) & 0xFF;
			hash ^= byte;
			hash *= FNV_PRIME;
		}
		return hash;
	}

	uint64 HashUTF32(const char32_t* data, uintMem length)
	{
		uint64 hash = FNV_OFFSET_BASIS;
		for (size_t i = 0; i < length; ++i)
			hash = HashUpdate(hash, data[i]);
		return hash;
	}

	uint64 Hash<GenericStringView<char>>::Compute(const GenericStringView<char>& string)
	{
		uint64 hash = FNV_OFFSET_BASIS;
		for (uintMem i = 0; i < string.Count();)
			hash = HashUpdate(hash, DecodeUTF8(reinterpret_cast<const char8_t*>(string.Ptr()), i, string.Count()));
		return hash;
	}
	uint64 Hash<GenericStringView<char8_t>>::Compute(const GenericStringView<char8_t>& string)
	{
		uint64 hash = FNV_OFFSET_BASIS;
		for (uintMem i = 0; i < string.Count();)
			hash = HashUpdate(hash, DecodeUTF8(string.Ptr(), i, string.Count()));
		return hash;
	}
	uint64 Hash<GenericStringView<char16_t>>::Compute(const GenericStringView<char16_t>& string)
	{
		uint64 hash = FNV_OFFSET_BASIS;
		for (uintMem i = 0; i < string.Count();)
			hash = HashUpdate(hash, DecodeUTF16(string.Ptr(), i, string.Count()));
		return hash;
	}
	uint64 Hash<GenericStringView<char32_t>>::Compute(const GenericStringView<char32_t>& string)
	{
		uint64 hash = FNV_OFFSET_BASIS;
		for (uintMem i = 0; i < string.Count(); ++i)
			hash = HashUpdate(hash, string[i]);
		return hash;
	}
}