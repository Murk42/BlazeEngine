#pragma once
#include "BlazeEngine/Core/Memory/Allocator.h"
#include "BlazeEngine/Core/String/StringCommon.h"
#include "BlazeEngine/Core/Type/IntegerTraits.h"


namespace Blaze
{
	template<typename Char>
	class GenericStringIterator;

	template<typename Char>
	class GenericStringView;

	template<typename T, AllocatorType Allocator>
	class Array;

	template<typename Char, AllocatorType Allocator = DefaultAllocator>
	class BLAZE_API GenericString
	{
	public:
		using CharType = Char;
		using IteratorType = GenericStringIterator<Char>;
		using ConstIteratorType = GenericStringIterator<const Char>;

		GenericString();
		GenericString(GenericString&& other) noexcept;
		GenericString(const GenericString& other);
		GenericString(uintMem size, Char fill = ' ');
		template<TriviallyConvertibleCharacter<Char> Char2>
		GenericString(GenericStringIterator<const Char2> begin, GenericStringIterator<const Char2> end);
		template<TriviallyConvertibleCharacter<Char> Char2>
		GenericString(const Char2* begin, const Char2* end);
		template<TriviallyConvertibleCharacter<Char> Char2>
		GenericString(GenericStringIterator<const Char2> begin, uintMem count);
		template<TriviallyConvertibleCharacter<Char> Char2>
		GenericString(const Char2* begin, uintMem count);
		template<TriviallyConvertibleCharacter<Char> Char2, uintMem Size>
		GenericString(const Char2(&arr)[Size]);
		template<ContainerType Container> requires TriviallyConvertibleCharacter<ContainerValueType<Container>, Char>
		GenericString(const Container& container);
		~GenericString();

		bool Empty() const;
		uint64 Hash() const;

		void Clear();

		void Resize(uintMem newBufferSize, Char fill = ' ');
		void Append(GenericStringView<Char> other);
		void Insert(intMem pos, GenericStringView<Char> other);
		void Erase(intMem pos, uintMem countToErase = 1);
		void Replace(GenericStringView<Char> target, GenericStringView<Char> value);

		GenericStringView<Char> SubString(intMem start, intMem end) const;
		template<AllocatorType ArrayAllocator = Allocator>
		Array<GenericStringView<Char>, ArrayAllocator> Split(GenericStringView<Char> separation, uintMem maxSplit = 0) const;
		template<AllocatorType ArrayAllocator = Allocator>
		Array<GenericStringView<Char>, ArrayAllocator> SplitReverse(GenericStringView<Char> separation, uintMem maxSplit = 0) const;

		uintMem Find(const GenericStringView<Char>& value, intMem start = 0) const;
		uintMem FindReverse(const GenericStringView<Char>& value, intMem start = -1) const;
		bool Contains(const GenericStringView<Char>& value, intMem start = 0) const;
		bool StartsWith(const GenericStringView<Char>& value) const;
		bool EndsWith(const GenericStringView<Char>& value) const;
		uintMem CountOf(const GenericStringView<Char>& value) const;
		GenericStringView<Char> Strip(const GenericStringView<Char>& characters) const;
		GenericStringView<Char> StripRight(const GenericStringView<Char>& characters) const;
		GenericStringView<Char> StripLeft(const GenericStringView<Char>& characters) const;
		template<AllocatorType StringAllocator = Allocator>
		GenericString<Char, StringAllocator > LowerCase() const requires SameAs<Char, char>;
		template<AllocatorType StringAllocator = Allocator>
		GenericString<Char, StringAllocator > UpperCase() const requires SameAs<Char, char>;
		template<AllocatorType StringAllocator1 = Allocator, ContainerTypeOf<GenericStringView<Char>> Container>
		GenericString<Char, StringAllocator1> Join(const Container& container);
		template<AllocatorType StringAllocator1 = Allocator, IsConvertibleTo<GenericStringView<Char>> ... Args>
		GenericString<Char, StringAllocator1> Join(const Args& ... args);

		template<AllocatorType StringAllocator = Allocator, ContainerType Container> requires IsConvertibleTo<ContainerValueType<Container>, GenericStringView<Char>>
		static GenericString<Char, StringAllocator> Concat(const Container& container);
		template<AllocatorType StringAllocator = Allocator, IsConvertibleTo<GenericStringView<Char>> ... Args>
		static GenericString<Char, StringAllocator> Concat(const Args& ... args);

		template<AllocatorType StringAllocator = Allocator>
		GenericString<Char, StringAllocator > AlignRight(uintMem width, Char fill = ' ');
		template<AllocatorType StringAllocator = Allocator>
		GenericString<Char, StringAllocator > AlignLeft(uintMem width, Char fill = ' ');
		template<AllocatorType StringAllocator = Allocator>
		GenericString<Char, StringAllocator > AlignCenter(uintMem width, Char fill = ' ');

		template<StringTypeConvertible<Char> Char2>
		bool ConvertibleToStringType() const;
		template<StringTypeConvertible<Char> Char2, AllocatorType Allocator2 = Allocator>
		GenericString<Char2, Allocator2> ConvertToStringType() const;
		template<IntegerType T>
		bool ConvertToInteger(T& to, uint base = 10, GenericStringView<Char>* remainingString = nullptr) const;
		template<DecimalType T>
		bool ConvertToDecimal(T& to, FloatStringConvertFormat format, GenericStringView<Char>* remainingString = nullptr) const;

		template<IntegerType T, AllocatorType StringAllocator = Allocator>
		static GenericString<Char, StringAllocator> Parse(const T& value, uint base = 10, bool uppercase = false, bool showPositiveSign = false);
		template<DecimalType T, AllocatorType StringAllocator = Allocator>
		static GenericString<Char, StringAllocator> Parse(const T& value, FloatStringParseFormat format = FloatStringParseFormat::General);

		Char* Ptr();
		const Char* Ptr() const;
		uintMem Count() const;

		Char* Begin();
		const Char* Begin() const;
		Char* End();
		const Char* End() const;

		Char& First();
		const Char& First() const;
		Char& Last();
		const Char& Last() const;

		IteratorType FirstIterator();
		ConstIteratorType FirstIterator() const;
		IteratorType LastIterator();
		ConstIteratorType LastIterator() const;
		IteratorType AheadIterator();
		ConstIteratorType AheadIterator() const;
		IteratorType BehindIterator();
		ConstIteratorType BehindIterator() const;

		Char& operator[](intMem index);
		const Char& operator[](intMem index) const;

		bool operator==(const GenericString& s) const;
		bool operator!=(const GenericString& s) const;

		template<AllocatorType OtherAllocator>
		GenericString& operator+=(const GenericString<Char, OtherAllocator>& other);
		GenericString& operator+=(const GenericStringView<Char>& other);
		template<uintMem Size>
		GenericString& operator+=(const Char(&other)[Size]);
		GenericString& operator+=(const Char& other);

		GenericString& operator=(const GenericString& other);
		GenericString& operator=(GenericString&& other) noexcept;
		template<TriviallyConvertibleCharacter<Char> Char2, uintMem Size>
		GenericString& operator=(const Char2(&arr)[Size]);
		template<ContainerConvertibleToString<const Char> Container>
		GenericString& operator=(const Container& container);
	private:
		Char* ptr;
		uintMem count;
		BLAZE_ALLOCATOR_ATTRIBUTE Allocator allocator;

		void AssignUnsafe(const Char* ptr, uintMem bufferSize);
	};

	template<typename Char, uintMem Size>
	GenericString(const Char(&arr)[Size]) -> GenericString<Char>;

	using String = GenericString<char>;
	using u8String = GenericString<char8_t>;
	using u16String = GenericString<char16_t>;
	using u32String = GenericString<char32_t>;
}

#include "BlazeEngine/Core/String/StringImpl.h"