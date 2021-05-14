#pragma once
#include <cstdint>

namespace Blaze
{
	typedef unsigned uint;
	typedef int8_t int8;
	typedef int16_t int16;
	typedef int32_t int32;
	typedef int64_t int64;

	typedef uint8_t uint8;
	typedef uint16_t uint16;
	typedef uint32_t uint32;
	typedef uint64_t uint64;

	typedef int8 byte;
	typedef uint8 ubyte;	

	template<typename R, typename ... T>
	using Function = R(*)(T...);

	struct InvalidType
	{
	};
}

#define baseoffset(derived, base) (size_t)(base*)(derived*)(uint8_t*)1 - 1

#define NULL_COPY(a) a(const a&) = delete;
#define NULL_ASSIGN(a) void operator=(const a&) = delete;
#define NULL_COPY_AND_ASSIGN(a) \
NULL_COPY(a) \
NULL_ASSIGN(a)