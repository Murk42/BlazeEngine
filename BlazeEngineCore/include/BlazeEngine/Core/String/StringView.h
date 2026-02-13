#pragma once
#include "BlazeEngine/Core/BlazeEngineCoreDefines.h"
#include "BlazeEngine/Core/Memory/Allocator.h"
#include "BlazeEngine/Core/String/StringCommon.h"
#include "BlazeEngine/Core/Type/IntegerTraits.h"

namespace Blaze
{
	template<typename Char>
	class GenericStringIterator;

	template<typename Char, AllocatorType Allocator>
	class GenericString;

	template<typename T, AllocatorType Allocator>
	class Array;

	template<typename Char>
	class BLAZE_API GenericStringView
	{
	public:
		using CharType = Char;
		using ConstIteratorType = GenericStringIterator<const Char>;

		constexpr GenericStringView() = default;
		constexpr GenericStringView(const GenericStringView& other) = default;
		constexpr GenericStringView(GenericStringView&& other) noexcept = default;
		template<TriviallyConvertibleCharacter<Char> Char2>
		constexpr GenericStringView(GenericStringIterator<const Char2> begin, GenericStringIterator<const Char2> end);
		template<TriviallyConvertibleCharacter<Char> Char2>
		constexpr GenericStringView(const Char2* begin, const Char2* end);
		template<TriviallyConvertibleCharacter<Char> Char2>
		constexpr GenericStringView(GenericStringIterator<const Char2> begin, uintMem count);
		template<TriviallyConvertibleCharacter<Char> Char2>
		constexpr GenericStringView(const Char2* begin, uintMem count);
		template<TriviallyConvertibleCharacter<Char> Char2, uintMem Size>
		constexpr GenericStringView(const Char2(&arr)[Size]);
		template<ContainerConvertibleToString<const Char> Container> requires ContiguousContainer<Container>
		constexpr GenericStringView(const Container& container);

		constexpr bool Empty() const;
		constexpr uint64 Hash() const;

		constexpr void Clear();

		constexpr GenericStringView SubString(intMem start, intMem end) const;
		template<AllocatorType ArrayAllocator = DefaultAllocator>
		Array<GenericStringView, ArrayAllocator> Split(GenericStringView separation, uintMem maxSplit = 0) const;
		template<AllocatorType ArrayAllocator = DefaultAllocator>
		Array<GenericStringView, ArrayAllocator> SplitReverse(GenericStringView separation, uintMem maxSplit = 0) const;

		constexpr uintMem Find(const GenericStringView& value, intMem start = 0) const;
		constexpr uintMem FindReverse(const GenericStringView& value, intMem start = 0) const;
		constexpr bool Contains(const GenericStringView& value, intMem start = 0) const;
		constexpr bool StartsWith(const GenericStringView& value) const;
		constexpr bool EndsWith(const GenericStringView& value) const;
		constexpr uintMem CountOf(const GenericStringView& value) const;
		constexpr GenericStringView Strip(const GenericStringView& characters) const;
		constexpr GenericStringView StripRight(const GenericStringView& characters) const;
		constexpr GenericStringView StripLeft(const GenericStringView& characters) const;
		template<AllocatorType StringAllocator = DefaultAllocator>
		GenericString<Char, StringAllocator> LowerCase() const requires SameAs<Char, char>;
		template<AllocatorType StringAllocator = DefaultAllocator>
		GenericString<Char, StringAllocator> UpperCase() const requires SameAs<Char, char>;
		template<AllocatorType StringAllocator = DefaultAllocator, ContainerType Container>
		GenericString<Char, StringAllocator> Join(const Container& container);
		template<AllocatorType StringAllocator = DefaultAllocator, IsConvertibleTo<GenericStringView<Char>> ... Args>
		GenericString<Char, StringAllocator> Join(const Args& ... args);

		template<AllocatorType StringAllocator = DefaultAllocator, ContainerType Container> requires IsConvertibleTo<ContainerValueType<Container>, GenericStringView<Char>>
		static GenericString<Char, StringAllocator> Concat(const Container& container);
		template<AllocatorType StringAllocator = DefaultAllocator, IsConvertibleTo<GenericStringView<Char>> ... Args>
		static GenericString<Char, StringAllocator> Concat(const Args& ... args);

		template<AllocatorType StringAllocator = DefaultAllocator>
		GenericString<Char, StringAllocator > AlignRight(uintMem width, Char fill = ' ');
		template<AllocatorType StringAllocator = DefaultAllocator>
		GenericString<Char, StringAllocator > AlignLeft(uintMem width, Char fill = ' ');
		template<AllocatorType StringAllocator = DefaultAllocator>
		GenericString<Char, StringAllocator > AlignCenter(uintMem width, Char fill = ' ');

		template<StringTypeConvertible<Char> Char2>
		bool ConvertibleToStringType() const;
		template<StringTypeConvertible<Char> Char2, AllocatorType Allocator = DefaultAllocator>
		GenericString<Char2, Allocator> ConvertToStringType() const;
		template<IntegerType T>
		bool ConvertToInteger(T& to, uint base = 10, GenericStringView* remainingString = nullptr) const;
		template<DecimalType T>
		bool ConvertToDecimal(T& to, FloatStringConvertFormat format, GenericStringView* remainingString = nullptr) const;

		template<IntegerType T, AllocatorType StringAllocator = DefaultAllocator>
		static GenericString<Char, StringAllocator> Parse(const T& value, uint base = 10, bool uppercase = false, bool showPositiveSign = false);
		template<DecimalType T, AllocatorType StringAllocator = DefaultAllocator>
		static GenericString<Char, StringAllocator> Parse(const T& value, FloatStringParseFormat format = FloatStringParseFormat::General);

		constexpr const Char* Ptr() const;
		constexpr uintMem Count() const;

		constexpr const Char* Begin() const;
		constexpr const Char* End() const;

		constexpr const Char& First() const;
		constexpr const Char& Last() const;

		constexpr ConstIteratorType FirstIterator() const;
		constexpr ConstIteratorType LastIterator() const;
		constexpr ConstIteratorType AheadIterator() const;
		constexpr ConstIteratorType BehindIterator() const;

		constexpr const Char& operator[](intMem index) const;

		constexpr bool operator==(const GenericStringView& other) const;
		constexpr bool operator!=(const GenericStringView& other) const;

		constexpr GenericStringView& operator=(const GenericStringView& other) = default;
		constexpr GenericStringView& operator=(GenericStringView&& other) noexcept = default;
		template<TriviallyConvertibleCharacter<Char> Char2, uintMem Size>
		constexpr GenericStringView& operator=(const Char2(&arr)[Size]);
		template<ContainerConvertibleToString<const Char> Container> requires ContiguousContainer<Container>
		constexpr GenericStringView& operator=(const Container& container);
	private:
		const Char* ptr;
		uintMem count;

		constexpr void Assign(const Char* ptr, uintMem bufferSize);
	};

	template<typename Char, AllocatorType Allocator>
	GenericStringView(const GenericString<Char, Allocator>&) -> GenericStringView<Char>;
	template<typename Char, uintMem Size>
	GenericStringView(const Char(&arr)[Size]) -> GenericStringView<Char>;

	using StringView = GenericStringView<char>;
	using u8StringView = GenericStringView<char8_t>;
	using u16StringView = GenericStringView<char16_t>;
	using u32StringView = GenericStringView<char32_t>;
}

#include "BlazeEngine/Core/String/StringViewImpl.h"