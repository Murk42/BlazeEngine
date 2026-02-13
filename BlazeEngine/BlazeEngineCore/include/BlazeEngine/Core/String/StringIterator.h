#pragma once
#include "BlazeEngine/Core/BlazeEngineCoreDefines.h"
#include "BlazeEngine/Core/String/StringCommon.h"

namespace Blaze
{
	template<typename Char>
	class BLAZE_API GenericStringIterator
	{
	public:
		using ValueType = Char;

		//STD compatibility
		using value_type = Char;
		using difference_type = intMem;

		constexpr GenericStringIterator() = default;
		constexpr GenericStringIterator(const GenericStringIterator& other) = default;
		constexpr GenericStringIterator(GenericStringIterator&& other) noexcept = default;
		template<TriviallyConvertibleCharacter<Char> Char2>
		constexpr GenericStringIterator(Char2* ptr);

		constexpr Char* Ptr() const;
		constexpr bool IsNull() const;

		constexpr Char& operator*() const;

		constexpr GenericStringIterator& operator++();
		constexpr GenericStringIterator operator++(int);
		constexpr GenericStringIterator& operator--();
		constexpr GenericStringIterator operator--(int);

		constexpr GenericStringIterator operator+(intMem) const;
		constexpr GenericStringIterator operator-(intMem) const;

		constexpr uintMem operator-(const GenericStringIterator& other) const;

		constexpr GenericStringIterator& operator+=(intMem offset);
		constexpr GenericStringIterator& operator-=(intMem offset);

		constexpr bool operator>(const GenericStringIterator& other) const;
		constexpr bool operator>=(const GenericStringIterator& other) const;
		constexpr bool operator<(const GenericStringIterator& other) const;
		constexpr bool operator<=(const GenericStringIterator& other) const;

		constexpr bool operator==(const GenericStringIterator& other) const = default;
		constexpr bool operator!=(const GenericStringIterator& other) const = default;

		constexpr operator Char* () const;

		constexpr operator GenericStringIterator<const Char>() const;

		constexpr GenericStringIterator& operator=(const GenericStringIterator& other) = default;
		constexpr GenericStringIterator& operator=(GenericStringIterator&& other) noexcept = default;
	private:
		Char* ptr;
	};

	using StringViewIterator = GenericStringIterator<const char>;
	using u8StringViewIterator = GenericStringIterator<const char8_t>;
	using u16StringViewIterator = GenericStringIterator<const char16_t>;
	using u32StringViewIterator = GenericStringIterator<const char32_t>;

	using StringIterator = GenericStringIterator<char>;
	using u8StringIterator = GenericStringIterator<char8_t>;
	using u16StringIterator = GenericStringIterator<char16_t>;
	using u32StringIterator = GenericStringIterator<char32_t>;
}

#include "BlazeEngine/Core/String/StringIteratorImpl.h"