#pragma once

#pragma warning (disable : 4996 )

#ifdef BLAZE_BUILD
#ifdef BLAZE_STATIC
#define BLAZE_API
#else
#define BLAZE_API __declspec(dllexport)
#endif
#else
#ifdef BLAZE_STATIC
#define BLAZE_API
#else
#define BLAZE_API __declspec(dllexport)
#endif
#endif

#ifdef _WIN32
#define BLAZE_PLATFORM_WINDOWS
#elif __linux__
#error Not supported
#define BLAZE_PLATFORM_LINUX
#elif __APPLE__
#error Not supported
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

namespace Blaze
{
	//This function should NOT be defined anywhere
	template<typename T>
	T&& Declval() noexcept;

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

	using uintMem = decltype(sizeof(1));
	using intMem = decltype(Declval<char*>() - Declval<char*>());

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

	template<typename Char>
	class GenericStringView;

	namespace Debug::Logger
	{
		BLAZE_API void LogDebug(GenericStringView<char8_t>&& source, GenericStringView<char8_t>&& message, bool styledText = true);
		BLAZE_API void LogInfo(GenericStringView<char8_t>&& source, GenericStringView<char8_t>&& message, bool styledText = true);
		BLAZE_API void LogWarning(GenericStringView<char8_t>&& source, GenericStringView<char8_t>&& message, bool styledText = true);
		BLAZE_API void LogError(GenericStringView<char8_t>&& source, GenericStringView<char8_t>&& message, bool styledText = true);
		BLAZE_API void LogFatal(GenericStringView<char8_t>&& source, GenericStringView<char8_t>&& message, bool styledText = true);
	}
}

#define BLAZE_LOG_DEBUG_BASIC(message) ::Blaze::Debug::Logger::LogDebug("Blaze Engine Core", message, true)
#define BLAZE_LOG_INFO_BASIC(message) ::Blaze::Debug::Logger::LogInfo("Blaze Engine Core", message, true)
#define BLAZE_LOG_WARNING_BASIC(message) ::Blaze::Debug::Logger::LogWarning("Blaze Engine Core", message, true)
#define BLAZE_LOG_ERROR_BASIC(message) ::Blaze::Debug::Logger::LogError("Blaze Engine Core", message, true)
#define BLAZE_LOG_FATAL_BASIC(message) ::Blaze::Debug::Logger::LogFatal("Blaze Engine Core", message, true)