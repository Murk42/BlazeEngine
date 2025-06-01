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

#define structptr(s, m, p) ((s*)(((char*)p) - (offsetof(s, m))))

#define ENUM_CLASS_BITWISE_OPERATIONS(name) \
	constexpr name operator|(name left, name right) { return static_cast<name>(static_cast<uint64>(left) | static_cast<uint64>(right)); } \
	constexpr name operator&(name left, name right) { return static_cast<name>(static_cast<uint64>(left) & static_cast<uint64>(right)); } \
	constexpr name operator^(name left, name right) { return static_cast<name>(static_cast<uint64>(left) ^ static_cast<uint64>(right)); } \
	constexpr name& operator|=(name& left, name right) { return (left = static_cast<name>(static_cast<uint64>(left) | static_cast<uint64>(right))); } \
	constexpr name& operator&=(name& left, name right) { return (left = static_cast<name>(static_cast<uint64>(left) & static_cast<uint64>(right))); } \
	constexpr name& operator^=(name& left, name right) { return (left = static_cast<name>(static_cast<uint64>(left) ^ static_cast<uint64>(right))); } \
	constexpr name operator~(name value) { return static_cast<name>(~static_cast<uint64>(value)); } 

#define BLAZE_ENGINE_CORE_DEBUG(message, ...) ::Blaze::Debug::Logger::LogDebug("Blaze Engine Core", message, __VA_ARGS__)
#define BLAZE_ENGINE_CORE_INFO(message, ...) ::Blaze::Debug::Logger::LogInfo("Blaze Engine Core", message, __VA_ARGS__)
#define BLAZE_ENGINE_CORE_WARNING(message, ...) ::Blaze::Debug::Logger::LogWarning("Blaze Engine Core", message, __VA_ARGS__)
#define BLAZE_ENGINE_CORE_ERROR(message, ...) ::Blaze::Debug::Logger::LogError("Blaze Engine Core", message, __VA_ARGS__)
#define BLAZE_ENGINE_CORE_FATAL(message, ...) ::Blaze::Debug::Logger::LogFatal("Blaze Engine Fatal", message, __VA_ARGS__)

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

	template<uint byteCount>
	struct UintWithByteSize_s { };
	template<> struct UintWithByteSize_s<1> { using type = uint8; };
	template<> struct UintWithByteSize_s<2> { using type = uint16; };
	template<> struct UintWithByteSize_s<4> { using type = uint32; };
	template<> struct UintWithByteSize_s<8> { using type = uint64; };
	template<uint byteCount>
	using UintWithByteSize = typename UintWithByteSize_s<byteCount>::type;

	template<uint byteCount>
	struct IntWithByteSize_s {};
	template<> struct IntWithByteSize_s<1> { using type = int8; };
	template<> struct IntWithByteSize_s<2> { using type = int16; };
	template<> struct IntWithByteSize_s<4> { using type = int32; };
	template<> struct IntWithByteSize_s<8> { using type = int64; };
	template<uint byteCount>
	using IntWithByteSize = typename IntWithByteSize_s<byteCount>::type;

	template<typename T>
	using MakeUnsigned = UintWithByteSize<sizeof(T)>;
	template<typename T>
	using MakeSigned = IntWithByteSize<sizeof(T)>;

	template<bool Value, typename A, typename B>
	struct Conditional_s { };
	template<typename A, typename B> struct Conditional_s<true, A, B> { using type = A; };
	template<typename A, typename B> struct Conditional_s<false, A, B> { using type = B; };
	template<bool Value, typename A, typename B>
	using Conditional = typename Conditional_s<Value, A, B>::type;

	template<uint64 MaxValue>
	using FittedUint = 
		Conditional<MaxValue <= ((1Ui64 << 8Ui64) - 1Ui64) , uint8,
		Conditional<MaxValue <= ((1Ui64 << 16Ui64) - 1Ui64), uint16,
		Conditional<MaxValue <= ((1Ui64 << 32Ui64) - 1Ui64), uint32,
		uint64>>>;		

	using uintMem = FittedUint<1Ui64 << ((sizeof(void*) * 8) - 1)>;
	using intMem = MakeSigned<uintMem>;

	enum byte : char { };
	enum ubyte : unsigned char { };

	template<typename R, typename ... T>
	using Function = R(*)(T...);

	template<typename R, typename C, typename ... A>
	using MemberFunction = R(C::*)(A...);

	template<class Base, class Derived>
	constexpr uintMem BaseOffset()
	{
		return reinterpret_cast<char*>(static_cast<Base*>(reinterpret_cast<Derived*>(0xf0000000Ui64))) - reinterpret_cast<char*>(reinterpret_cast<Derived*>(0xf0000000Ui64));
	}
}