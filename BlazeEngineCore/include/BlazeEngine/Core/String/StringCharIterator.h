#pragma once
#include "BlazeEngine/Core/String/StringCommon.h"
#include "BlazeEngine/Core/String/UnicodeChar.h"

namespace Blaze
{
	template<typename T>
	class GenericStringIterator;

	template<typename Char>
	class BLAZE_API GenericStringCharIterator
	{
	public:
		using ValueType = UnicodeChar;

		//STD compatibility
		using value_type = UnicodeChar;
		using difference_type = intMem;

		constexpr GenericStringCharIterator() = default;
		constexpr GenericStringCharIterator(const GenericStringCharIterator& other) = default;
		constexpr GenericStringCharIterator(GenericStringCharIterator&& other) noexcept = default;
		constexpr GenericStringCharIterator(GenericStringIterator<const Char> it, GenericStringView<Char> string);

		constexpr const Char* Ptr() const;
		constexpr bool IsNull() const;

		constexpr UnicodeChar operator*() const;

		constexpr GenericStringCharIterator& operator++();
		constexpr GenericStringCharIterator operator++(int);
		constexpr GenericStringCharIterator& operator--();
		constexpr GenericStringCharIterator operator--(int);

		constexpr bool operator>(const GenericStringCharIterator& other) const;
		constexpr bool operator>=(const GenericStringCharIterator& other) const;
		constexpr bool operator<(const GenericStringCharIterator& other) const;
		constexpr bool operator<=(const GenericStringCharIterator& other) const;

		constexpr bool operator==(const GenericStringCharIterator& other) const = default;
		constexpr bool operator!=(const GenericStringCharIterator& other) const = default;

		constexpr operator const Char* () const;
		constexpr operator GenericStringIterator<const Char> () const;

		constexpr GenericStringCharIterator& operator=(const GenericStringCharIterator& other) = default;
		constexpr GenericStringCharIterator& operator=(GenericStringCharIterator&& other) noexcept = default;
		constexpr GenericStringCharIterator& operator=(const GenericStringIterator<Char>& other);

		static constexpr GenericStringCharIterator AheadIterator(const GenericStringView<Char>& string);
		static constexpr GenericStringCharIterator FirstIterator(const GenericStringView<Char>& string);
		static constexpr GenericStringCharIterator LastIterator(const GenericStringView<Char>& string);
		static constexpr GenericStringCharIterator BehindIterator(const GenericStringView<Char>& string);
	private:
		const Char* begin;
		const Char* end;
		const Char* ptr;
	};

	using StringCharIterator = GenericStringCharIterator<char>;
	using u8StringCharIterator = GenericStringCharIterator<char8_t>;
	using u16StringCharIterator = GenericStringCharIterator<char16_t>;
	using u32StringCharIterator = GenericStringCharIterator<char32_t>;
}

#include "BlazeEngine/Core/String/StringCharIteratorImpl.h"