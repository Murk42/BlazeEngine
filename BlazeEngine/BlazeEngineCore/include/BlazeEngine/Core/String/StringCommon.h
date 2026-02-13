#pragma once
#include "BlazeEngine/Core/BlazeEngineCoreDefines.h"
#include "BlazeEngine/Core/Type/IteratorTraits.h"

namespace Blaze
{
	template<typename From, typename To>
	concept TriviallyConvertibleCharacter =
		SameAs<RemoveCVRef<From>, char>     && SameAs<RemoveCVRef<To>, char>     ||
		SameAs<RemoveCVRef<From>, char>     && SameAs<RemoveCVRef<To>, char8_t>  ||
		SameAs<RemoveCVRef<From>, char8_t>  && SameAs<RemoveCVRef<To>, char8_t>  ||
		SameAs<RemoveCVRef<From>, char16_t> && SameAs<RemoveCVRef<To>, char16_t> ||
		SameAs<RemoveCVRef<From>, char32_t> && SameAs<RemoveCVRef<To>, char32_t>;

	enum class FloatStringConvertFormat
	{
		Hex,
		Scientific,
		Fixed,
		General = Fixed | Scientific,
	};
	ENUM_CLASS_BITWISE_OPERATIONS(FloatStringConvertFormat)

	enum class FloatStringParseFormat
	{
		Hex,
		Scientific,
		Fixed,
		General,
	};

	template<typename Container, typename Char>
	concept ContainerConvertibleToString =
		ContainerType<Container> &&
		TriviallyConvertibleCharacter<ContainerValueType<Container>, Char>;

	template<typename DstChar, typename SrcChar>
	struct StringTypeConverter;

	template<typename DstChar, typename SrcChar>
	concept StringTypeConvertible = requires(StringTypeConverter<DstChar, SrcChar> converter, const SrcChar * src, DstChar * dst, uintMem srcSize, uintMem dstSize)
	{
		{ converter.ConvertedSize(src, srcSize) } -> SameAs<uintMem>;
		{ converter.Convert(dst, dstSize, src, srcSize) } -> SameAs<uintMem>;
		{ converter.CanConvert(src, srcSize) } -> SameAs<bool>;
	};

	namespace Details
	{
		template<typename Char1, typename Char2>
		struct StringDirectlyComparable
		{

		};

		template<> struct StringDirectlyComparable<char, char> { static constexpr bool value = true; };
		template<> struct StringDirectlyComparable<char, char8_t> { static constexpr bool value = true; };
		template<> struct StringDirectlyComparable<char, char16_t> { static constexpr bool value = true; };
		template<> struct StringDirectlyComparable<char, char32_t> { static constexpr bool value = true; };
		template<> struct StringDirectlyComparable<char8_t, char> { static constexpr bool value = true; };
		template<> struct StringDirectlyComparable<char16_t, char> { static constexpr bool value = true; };
		template<> struct StringDirectlyComparable<char32_t, char> { static constexpr bool value = true; };
	}

	template<typename Char1, typename Char2>
	concept StringDirectlyComparable = Details::StringDirectlyComparable<Char1, Char2>::value;
}