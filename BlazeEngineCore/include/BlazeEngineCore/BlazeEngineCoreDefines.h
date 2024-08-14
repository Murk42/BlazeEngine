#pragma once

#pragma warning (disable : 4996 )

#ifdef BLAZE_BUILD
#ifdef BLAZE_STATIC
#define BLAZE_CORE_API
#else
#define BLAZE_CORE_API __declspec(dllexport)
#endif
#else
#ifdef BLAZE_STATIC
#define BLAZE_CORE_API
#else
#define BLAZE_CORE_API __declspec(dllexport)
#endif
#endif

#ifdef _WIN32 
#define BLAZE_PLATFORM_WINDOWS
#elif __linux__
#define BLAZE_PLATFORM_LINUX
#elif __APPLE__
#define BLAZE_PLATFORM_APPLE
#endif

#if defined(_MSC_VER) && !defined(__INTEL_COMPILER)
#define BLAZE_COMPILER_MSVC
#elif
#error
#endif

#ifdef _DEBUG
#define BLAZE_DEBUG
#endif

#ifdef BLAZE_DEBUG
#define BLAZE_NULL_ITERATOR_CHECK
#define BLAZE_INVALID_ITERATOR_CHECK
#endif

#ifndef BLAZE_SDL_WINDOW_DEFAULT
#define BLAZE_SDL_WINDOW_DEFAULT
#endif

#define BLAZE_OK 1
#define BLAZE_YES 1
#define BLAZE_NO 0
#define BLAZE_ERROR -1

#include <type_traits>

namespace Blaze
{
	struct InvalidType
	{
	};

	using uint = unsigned int;

	using int8 = signed char;
	using int16 = signed short;
	using int32 = signed int;
	using int64 = signed long long;

	using uint8 = unsigned char;
	using uint16 = unsigned short;
	using uint32 = unsigned int;
	using uint64 = unsigned long long;

	using uintMem = std::make_unsigned_t<decltype(sizeof(int8))>;
	using intMem = std::make_signed_t<decltype(sizeof(int8))>;

	template<uint64 MaxValue>
	using fitted_uint =
		std::conditional_t<MaxValue <= std::numeric_limits<uint8>::max(), uint8,
		std::conditional_t<MaxValue <= std::numeric_limits<uint16>::max(), uint16,
		std::conditional_t<MaxValue <= std::numeric_limits<uint32>::max(), uint32,
		std::conditional_t<MaxValue <= std::numeric_limits<uint64>::max(), uint64,
		void>>>>;

	enum byte : char { };
	enum ubyte : unsigned char { };

	template<typename R, typename ... T>
	using Function = R(*)(T...);

	template<class Base, class Derived>
	inline uintMem BaseOffset()
	{
		return reinterpret_cast<char*>(static_cast<Base*>(reinterpret_cast<Derived*>(0xf0000000Ui64))) - reinterpret_cast<char*>(reinterpret_cast<Derived*>(0xf0000000Ui64));
	}
}

#define structptr(s, m, p) ((s*)(((char*)p) - (offsetof(s, m))))

#define ENUM_CLASS_BITWISE_OPERATIONS(name) \
	constexpr name operator|(name left, name right) { return (name)((int)left | (int)right); } \
	constexpr name operator&(name left, name right) { return (name)((int)left & (int)right); } \
	constexpr name operator^(name left, name right) { return (name)((int)left ^ (int)right); } \
	constexpr name& operator|=(name& left, name right) { return (left = (name)((int)left | (int)right)); } \
	constexpr name& operator&=(name& left, name right) { return (left = (name)((int)left & (int)right)); } \
	constexpr name& operator^=(name& left, name right) { return (left = (name)((int)left ^ (int)right)); } \
	constexpr name operator~(name value) { return (name)(~(int)value); } 