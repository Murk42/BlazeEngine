#pragma once
#include "BlazeEngine/Core/BlazeEngineCoreDefines.h"

namespace Blaze
{
	template<typename>
	class GenericStringView;

	template<typename>
	class Hash;

	template<>
	struct BLAZE_API Hash<GenericStringView<char>>
	{
	public:
		using Type = GenericStringView<char>;

		uint64 Compute(const GenericStringView<char>& string);
	};

	template<>
	struct BLAZE_API Hash<GenericStringView<char8_t>>
	{
	public:
		using Type = GenericStringView<char8_t>;

		uint64 Compute(const GenericStringView<char8_t>& string);
	};

	template<>
	struct BLAZE_API Hash<GenericStringView<char16_t>>
	{
	public:
		using Type = GenericStringView<char16_t>;

		uint64 Compute(const GenericStringView<char16_t>& string);
	};

	template<>
	struct BLAZE_API Hash<GenericStringView<char32_t>>
	{
	public:
		using Type = GenericStringView<char32_t>;

		uint64 Compute(const GenericStringView<char32_t>& string);
	};
}