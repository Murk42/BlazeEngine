#pragma once
#include "BlazeEngine/Core/Common/Algorithm.h"
#include "BlazeEngine/Core/Common/Tuple.h"
#include "BlazeEngine/Core/String/StringIterator.h"
#include "BlazeEngine/Core/String/StringHash.h"
#include "BlazeEngine/Core/String/StringTypeConverter.h"
#include "BlazeEngine/Core/String/UnicodeChar.h"
#include <charconv>
#include "StringView.h"

namespace Blaze
{
	template<typename Char>
	constexpr auto begin(const GenericStringView<Char>& string)
	{
		return string.FirstIterator();
	}
	template<typename Char>
	constexpr auto end(const GenericStringView<Char>& string)
	{
		return string.BehindIterator();
	}

	template<typename Char>
	template<TriviallyConvertibleCharacter<Char> Char2>
	inline constexpr GenericStringView<Char>::GenericStringView(GenericStringIterator<const Char2> begin, GenericStringIterator<const Char2> end)
		: ptr(nullptr), count(0)
	{
		Assign(reinterpret_cast<const Char*>(begin.Ptr()), end - begin);
	}
	template<typename Char>
	template<TriviallyConvertibleCharacter<Char> Char2>
	inline constexpr GenericStringView<Char>::GenericStringView(const Char2* begin, const Char2* end)
		: ptr(nullptr), count(0)
	{
		Assign(reinterpret_cast<const Char*>(begin), end - begin);
	}
	template<typename Char>
	template<TriviallyConvertibleCharacter<Char> Char2>
	inline constexpr GenericStringView<Char>::GenericStringView(GenericStringIterator<const Char2> begin, uintMem count)
		: ptr(nullptr), count(0)
	{
		Assign(reinterpret_cast<const Char*>(begin.Ptr()), count);
	}
	template<typename Char>
	template<TriviallyConvertibleCharacter<Char> Char2>
	inline constexpr GenericStringView<Char>::GenericStringView(const Char2* begin, uintMem count)
		: ptr(nullptr), count(0)
	{
		Assign(reinterpret_cast<const Char*>(begin), count);
	}
	template<typename Char>
	template<TriviallyConvertibleCharacter<Char> Char2, uintMem Size>
	constexpr GenericStringView<Char>::GenericStringView(const Char2(&arr)[Size])
		: ptr(nullptr), count(0)
	{
		Assign(reinterpret_cast<const Char*>(arr), Size);
	}
	template<typename Char>
	template<ContainerConvertibleToString<const Char> Container> requires ContiguousContainer<Container>
	inline constexpr GenericStringView<Char>::GenericStringView(const Container& container)
		: ptr(nullptr), count(0)
	{
		Assign(reinterpret_cast<const Char*>(container.Ptr()), container.Count());
	}
	template<typename Char>
	constexpr bool GenericStringView<Char>::Empty() const
	{
		return count == 0;
	}
	template<typename Char>
	constexpr uint64 GenericStringView<Char>::Hash() const
	{
		return Blaze::Hash<GenericStringView<Char>>().Compute(*this);
	}
	template<typename Char>
	constexpr void GenericStringView<Char>::Clear()
	{
		ptr = nullptr;
		count = 0;
	}
	template<typename Char>
	constexpr GenericStringView<Char> GenericStringView<Char>::SubString(intMem start, intMem end) const
	{
		if (start < 0)
			start += static_cast<intMem>(count);

		if (end < 0)
			end += static_cast<intMem>(count) + 1;

		if (start < 0 || end > static_cast<intMem>(count) || start > end)
			BLAZE_LOG_FATAL_BASIC("Invalid substring");

		return GenericStringView(ptr + start, static_cast<uintMem>(end - start));
	}
	template<typename Char>
	template<AllocatorType ArrayAllocator>
	inline Array<GenericStringView<Char>, ArrayAllocator> GenericStringView<Char>::Split(GenericStringView separation, uintMem maxSplit) const
	{
		Array<GenericStringView<Char>, ArrayAllocator> out;

		if (maxSplit == 0)
			maxSplit = SIZE_MAX;

		uintMem offset = 0;
		while (out.Count() < maxSplit)
		{
			uintMem i = Find(separation, offset);

			if (i == SIZE_MAX)
				break;

			out.AddBack(SubString(offset, i));

			offset = i + separation.Count();
		}

		out.AddBack(SubString(offset, count));

		return out;
	}
	template<typename Char>
	template<AllocatorType ArrayAllocator>
	inline Array<GenericStringView<Char>, ArrayAllocator> GenericStringView<Char>::SplitReverse(GenericStringView separation, uintMem maxSplit) const
	{
		Array<GenericStringView<Char>, ArrayAllocator> out;

		if (maxSplit == 0)
			maxSplit = SIZE_MAX;

		uintMem offset = count;
		while (out.Count() < maxSplit)
		{
			uintMem i = FindReverse(separation, static_cast<intMem>(offset) - 1);

			if (i == SIZE_MAX)
				break;

			out.AddAt(0, SubString(i + separation.Count(), static_cast<intMem>(offset)));

			offset = i;
		}

		out.AddAt(0, SubString(0, offset));

		return out;
	}
	template<typename Char>
	constexpr uintMem GenericStringView<Char>::Find(const GenericStringView& value, intMem start) const
	{
		if (start < 0)
			start += count + 1;

		if (start < 0 || static_cast<uintMem>(start) > count)
			BLAZE_LOG_FATAL_BASIC("Invalid index");

		if (start == count)
			return SIZE_MAX;

		uintMem index = Blaze::Algorithm::FindRange(GenericStringView(ptr + start, ptr + count), value).Ptr() - ptr;

		if (index == count)
			return SIZE_MAX;

		return index;
	}
	template<typename Char>
	inline constexpr uintMem GenericStringView<Char>::FindReverse(const GenericStringView& value, intMem start) const
	{
		if (start < 0)
			start += count + 1;

		if (start < 0 || static_cast<uintMem>(start) > count)
			BLAZE_LOG_FATAL_BASIC("Invalid index");

		uintMem index = Blaze::Algorithm::FindRangeReverse(GenericStringView(ptr + start, ptr + count), value).Ptr() - ptr;

		if (index == count)
			return SIZE_MAX;

		return index;
	}
	template<typename Char>
	inline constexpr bool GenericStringView<Char>::Contains(const GenericStringView& value, intMem start) const
	{
		if (start < 0)
			start += count + 1;

		if (start < 0 || start > count)
			BLAZE_LOG_FATAL_BASIC("Invalid index");

		return Blaze::Algorithm::ContainsRange(GenericStringView(ptr + start, ptr + count), value);
	}
	template<typename Char>
	inline constexpr bool GenericStringView<Char>::StartsWith(const GenericStringView& value) const
	{
		return Blaze::Algorithm::StartsWithRange(*this, value);
	}
	template<typename Char>
	inline constexpr bool GenericStringView<Char>::EndsWith(const GenericStringView& value) const
	{
		return Blaze::Algorithm::EndsWithRange(*this, value);
	}
	template<typename Char>
	inline constexpr uintMem GenericStringView<Char>::CountOf(const GenericStringView& value) const
	{
		return Algorithm::CountOfRange(*this, value);
	}
	template<typename Char>
	inline constexpr GenericStringView<Char> GenericStringView<Char>::Strip(const GenericStringView& characters) const
	{
		return StripLeft(characters).StripRight(characters);
	}
	template<typename Char>
	inline constexpr GenericStringView<Char> GenericStringView<Char>::StripRight(const GenericStringView& characters) const
	{
		if (count == 0)
			return { };

		uintMem r = count;
		while (r > 0)
		{
			bool found = false;
			for (auto& ch : characters)
				if (ch == ptr[r - 1])
				{
					found = true;
					break;
				}

			if (!found)
				break;

			--r;
		}

		return { ptr, ptr + r };
	}
	inline constexpr GenericStringView<char8_t> GenericStringView<char8_t>::StripRight(const GenericStringView<char8_t>& characters) const
	{
		UnicodeChar a;
		return { };
	}
	inline constexpr GenericStringView<char16_t> GenericStringView<char16_t>::StripRight(const GenericStringView<char16_t>& characters) const;
	inline constexpr GenericStringView<char32_t> GenericStringView<char32_t>::StripRight(const GenericStringView<char32_t>& characters) const;
	template<typename Char>
	inline constexpr GenericStringView<Char> GenericStringView<Char>::StripLeft(const GenericStringView& characters) const
	{
		if (count == 0)
			return { };

		uintMem l = 0;
		while (l < count)
		{
			bool found = false;
			for (auto& ch : characters)
				if (ch == ptr[l])
				{
					found = true;
					break;
				}

			if (!found)
				break;

			++l;
		}

		return { ptr + l, ptr + count};
	}
	template<typename Char>
	template<AllocatorType StringAllcoator>
	inline GenericString<Char, StringAllcoator> GenericStringView<Char>::LowerCase() const requires SameAs<Char, char>
	{
		GenericString<Char, StringAllcoator> out{ count };

		for (uintMem i = 0; i < count; ++i)
			out[i] = ptr[i] >= 'A' && ptr[i] <= 'Z' ? ptr[i] + ('a' - 'A') : ptr[i];

		return out;
	}
	template<typename Char>
	template<AllocatorType StringAllcoator>
	inline GenericString<Char, StringAllcoator> GenericStringView<Char>::UpperCase() const requires SameAs<Char, char>
	{
		GenericString<Char, StringAllcoator> out{ count };

		for (uintMem i = 0; i < count; ++i)
			out[i] = ptr[i] >= 'a' && ptr[i] <= 'z' ? ptr[i] - ('a' - 'A') : ptr[i];

		return out;
	}
	template<typename Char>
	template<AllocatorType StringAllocator, ContainerType Container>
	inline GenericString<Char, StringAllocator> GenericStringView<Char>::Join(const Container& container)
	{
		if (begin(container) == end(container))
			return { };

		uintMem newCount = 0;

		for (auto& str : container)
			newCount += str.Count() + count;

		newCount -= count;

		GenericString<Char, StringAllocator> out{ newCount };

		uintMem offset = 0;

		auto it = begin(container);
		uintMem i = 0;
		for (uintMem i = 0; i < GenericStringView<Char>(*it).Count(); ++i)
			out[offset + i] = GenericStringView<Char>(*it)[i];

		offset += GenericStringView<Char>(*it).Count();

		++it;
		for (; it != end(container); ++it)
		{
			GenericStringView<Char> str = *it;

			memcpy(out.Ptr() + offset, ptr, count * sizeof(Char));
			offset += count;
			memcpy(out.Ptr() + offset, str.Ptr(), str.Count() * sizeof(Char));
			offset += str.Count();
		}

		return out;
	}
	template<typename Char>
	template<AllocatorType StringAllocator, IsConvertibleTo<GenericStringView<Char>> ...Args>
	inline GenericString<Char, StringAllocator> GenericStringView<Char>::Join(const Args & ...args)
	{
		return Join<StringAllocator>(std::initializer_list{ GenericStringView<Char>(args)... });
	}
	template<typename Char>
	template<AllocatorType StringAllocator, ContainerType Container> requires IsConvertibleTo<ContainerValueType<Container>, GenericStringView<Char>>
	inline GenericString<Char, StringAllocator> GenericStringView<Char>::Concat(const Container& container)
	{
		uintMem totalWidth = 0;
		for (auto& str : container)
			totalWidth += GenericStringView<Char>(str).Count();

		GenericString<Char, StringAllocator> out{ totalWidth };

		totalWidth = 0;
		for (auto& value : container)
		{
			auto string = GenericStringView<Char>(value);
			memcpy(out.Ptr() + totalWidth, string.Ptr(), string.Count() * sizeof(Char));
			totalWidth += string.Count();
		}

		return out;
	}
	template<typename Char>
	template<AllocatorType StringAllocator, IsConvertibleTo<GenericStringView<Char>> ...Args>
	inline GenericString<Char, StringAllocator> GenericStringView<Char>::Concat(const Args & ...args)
	{
		Tuple<const Args&...> containedArgs = { args... };

		uintMem totalWidth = 0;
		containedArgs.ForEach([&]<uintMem I>(const auto& value)
		{
			totalWidth += GenericStringView<Char>(value).Count();
		});

		GenericString<Char, StringAllocator> out{ totalWidth };

		totalWidth = 0;
		containedArgs.ForEach([&]<uintMem I>(const auto& value)
		{
			auto string = GenericStringView<Char>(value);
			memcpy(out.Ptr() + totalWidth, string.Ptr(), string.Count() * sizeof(Char));
			totalWidth += string.Count();
		});

		return out;
		return GenericString<Char, StringAllocator>();
	}
	template<typename Char>
	template<AllocatorType StringAllcoator>
	inline GenericString<Char, StringAllcoator> GenericStringView<Char>::AlignRight(uintMem width, Char fill)
	{
		GenericString<Char, StringAllcoator> out{ width };

		if (width > count)
		{
			for (uintMem i = 0; i < width - count; ++i)
				out[i] = fill;

			for (uintMem i = width - count; i < width; ++i)
				out[i] = ptr[i - (width - count)];
		}
		else
		{
			for (uintMem i = 0; i < width; ++i)
				out[i] = ptr[i + count - width];
		}

		return out;
	}
	template<typename Char>
	template<AllocatorType StringAllcoator>
	inline GenericString<Char, StringAllcoator> GenericStringView<Char>::AlignLeft(uintMem width, Char fill)
	{
		GenericString<Char, StringAllcoator> out{ width };

		if (width > count)
		{
			for (uintMem i = 0; i < count; ++i)
				out[i] = ptr[i];

			for (uintMem i = count; i < width; ++i)
				out[i] = fill;
		}
		else
		{
			for (uintMem i = 0; i < width; ++i)
				out[i] = ptr[i];
		}

		return out;
	}
	template<typename Char>
	template<AllocatorType StringAllcoator>
	inline GenericString<Char, StringAllcoator> GenericStringView<Char>::AlignCenter(uintMem width, Char fill)
	{
		GenericString<Char, StringAllcoator> out{ width };

		if (width > count)
		{
			uintMem offset = (width - count) >> 1;

			for (uintMem i = 0; i < offset; ++i)
				out[i] = fill;

			for (uintMem i = offset; i < offset + count; ++i)
				out[i] = ptr[i - offset];

			for (uintMem i = offset + count; i < width; ++i)
				out[i] = fill;
		}
		else
		{
			uintMem offset = (count - width) >> 1;

			for (uintMem i = 0; i < width; ++i)
				out[i] = ptr[i + offset];
		}

		return out;
	}
	template<typename Char>
	template<StringTypeConvertible<Char> Char2>
	inline bool GenericStringView<Char>::ConvertibleToStringType() const
	{
		StringTypeConverter<Char2, Char> converter;
		return converter.CanConvert(ptr, count);
	}
	template<typename Char>
	template<StringTypeConvertible<Char> Char2, AllocatorType Allocator>
	inline GenericString<Char2, Allocator> GenericStringView<Char>::ConvertToStringType() const
	{
		StringTypeConverter<Char2, Char> converter;
		if (converter.CanConvert(ptr, count))
		{
			GenericString<Char2, Allocator> out{ converter.ConvertedSize(ptr, count) };
			converter.Convert(out.Ptr(), out.Count(), ptr, count);
			return out;
		}
		else
		{
			BLAZE_LOG_ERROR_BASIC("String is not convertible");
			return { };
		}
	}
	template<typename Char>
	template<IntegerType T>
	inline bool GenericStringView<Char>::ConvertToInteger(T& to, uint base, GenericStringView* remainingString) const
	{
		if constexpr (!(SameAs<Char, char> || SameAs<Char, char8_t>))
		{
			GenericString<char8_t> converted = ConvertToStringType<char8_t>(*this);
			GenericStringView<char8_t> _remainingString;
			if (converted.ConvertToInteger(to, base, &_remainingString))
			{
				StringTypeConverter<Char, char8_t> converter;
				uintMem offset = converter.ConvertedSize(converted.Ptr(), _remainingString.Ptr() - converted.Ptr());

				if (offset > count)
					offset = count;

				*remainingString = GenericStringView(ptr + offset, ptr + count);
				return true;
			}

			return false;
		}
		else
		{
			auto [end, err] = std::from_chars(
				reinterpret_cast<const char*>(ptr),
				reinterpret_cast<const char*>(ptr + count)
				, to, base);

			if (err == std::errc::value_too_large)
				return false;
			else if (remainingString != nullptr)
				*remainingString = GenericStringView(reinterpret_cast<const Char*>(end), ptr + count);

			return true;
		}
	}
	template<typename Char>
	template<DecimalType T>
	inline bool GenericStringView<Char>::ConvertToDecimal(T& to, FloatStringConvertFormat format, GenericStringView* remainingString) const
	{
		if constexpr (!(SameAs<Char, char> || SameAs<Char, char8_t>))
		{
			GenericString<char8_t> converted = this->ConvertToStringType<char8_t>();
			GenericStringView<char8_t> _remainingString;
			if (converted.ConvertToDecimal(to, format, &_remainingString))
			{
				StringTypeConverter<Char, char8_t> converter;
				uintMem offset = converter.ConvertedSize(converted.Ptr(), _remainingString.Ptr() - converted.Ptr());

				if (offset > count)
					offset = count;

				*remainingString = GenericStringView(ptr + offset, ptr + count);
				return true;
			}

			return false;
		}
		else
		{
			std::chars_format _format = std::chars_format();

			if ((format & FloatStringConvertFormat::Scientific) != static_cast<FloatStringConvertFormat>(0))
				_format |= std::chars_format::scientific;
			if ((format & FloatStringConvertFormat::Fixed) != static_cast<FloatStringConvertFormat>(0))
				_format |= std::chars_format::fixed;
			if ((format & FloatStringConvertFormat::Hex) != static_cast<FloatStringConvertFormat>(0))
				_format |= std::chars_format::hex;

			auto [end, err] = std::from_chars(
				reinterpret_cast<const char*>(ptr),
				reinterpret_cast<const char*>(ptr + count)
				, to, _format);

			if (err == std::errc::result_out_of_range || err == std::errc::invalid_argument)
				return false;
			else if (remainingString != nullptr)
				*remainingString = GenericStringView(reinterpret_cast<const Char*>(end), ptr + count);

			return true;
		}
	}
	template<typename Char>
	template<IntegerType T, AllocatorType StringAllocator>
	inline GenericString<Char, StringAllocator> GenericStringView<Char>::Parse(const T& value, uint base, bool uppercase, bool showPositiveSign)
	{
		char buffer[128]{ };

		char* parseIt = buffer;
		if (showPositiveSign && value >= 0)
		{
			*parseIt = '+';
			++parseIt;
		}

		auto [end, err] = std::to_chars(parseIt, buffer + _countof(buffer), value, base);

		if (err != std::errc())
			return { };

		if (base > 10 && uppercase)
		{
			for (char* it = parseIt; it != end; ++it)
				if (*it >= 'a' && *it <= 'f')
					*it = *it - ('a' - 'A');
		}

		return StringView(buffer, end).ConvertToStringType<Char, StringAllocator>();
	}
	template<typename Char>
	template<DecimalType T, AllocatorType StringAllocator>
	inline GenericString<Char, StringAllocator> GenericStringView<Char>::Parse(const T& value, FloatStringParseFormat format)
	{
		char buffer[128]{ };
		char* parseIt = buffer;

		std::chars_format _format = std::chars_format();

		if (format == FloatStringParseFormat::Scientific)
			_format = std::chars_format::scientific;
		else if (format == FloatStringParseFormat::Fixed)
			_format = std::chars_format::fixed;
		else if (format == FloatStringParseFormat::Hex)
			_format = std::chars_format::hex;
		else if (format == FloatStringParseFormat::General)
			_format = std::chars_format::general;

		auto [end, err] = std::to_chars(parseIt, buffer + _countof(buffer), value, _format);

		if (err != std::errc())
			return { };

		return StringView(buffer, end).ConvertToStringType<Char, StringAllocator>();
	}
	template<typename Char>
	constexpr const Char* GenericStringView<Char>::Ptr() const
	{
		return ptr;
	}
	template<typename Char>
	constexpr uintMem GenericStringView<Char>::Count() const
	{
		return count;
	}
	template<typename Char>
	constexpr const Char* GenericStringView<Char>::Begin() const
	{
		return ptr;
	}
	template<typename Char>
	constexpr const Char* GenericStringView<Char>::End() const
	{
		return ptr + count;
	}
	template<typename Char>
	constexpr const Char& GenericStringView<Char>::First() const
	{
		if (count == 0)
			BLAZE_LOG_FATAL_BASIC("Accesing an empty string");

		return *ptr;
	}
	template<typename Char>
	constexpr const Char& GenericStringView<Char>::Last() const
	{
		if (count == 0)
			BLAZE_LOG_FATAL_BASIC("Accesing an empty string");

		return *(ptr + count - 1);
	}
	template<typename Char>
	constexpr auto GenericStringView<Char>::FirstIterator() const -> ConstIteratorType
	{
		return { ptr };
	}
	template<typename Char>
	constexpr auto GenericStringView<Char>::LastIterator() const -> ConstIteratorType
	{
		return { ptr + count - 1};
	}
	template<typename Char>
	constexpr auto GenericStringView<Char>::AheadIterator() const -> ConstIteratorType
	{
		return { ptr - 1};
	}
	template<typename Char>
	constexpr auto GenericStringView<Char>::BehindIterator() const -> ConstIteratorType
	{
		return { ptr + count };
	}
	template<typename Char>
	constexpr const Char& GenericStringView<Char>::operator[](intMem index) const
	{
		if (index < 0)
			index += count;
		else if (static_cast<uintMem>(index) >= count)
			BLAZE_LOG_FATAL_BASIC("Accesing an string outside its bounds");

		return ptr[index];
	}
	template<typename Char>
	constexpr bool GenericStringView<Char>::operator==(const GenericStringView& other) const
	{
		if (count != other.count)
			return false;

		const Char* end1 = ptr + count;
		const Char* ptr1 = ptr;
		const Char* ptr2 = other.ptr;

		while (ptr1 != end1)
		{
			if (*ptr1 != *ptr2)
				return false;

			++ptr1;
			++ptr2;
		}

		return true;
	}
	template<typename Char>
	constexpr bool GenericStringView<Char>::operator!=(const GenericStringView& other) const
	{
		return !(*this == other);
	}
	template<typename Char>
	template<TriviallyConvertibleCharacter<Char> Char2, uintMem Size>
	inline constexpr GenericStringView<Char>& GenericStringView<Char>::operator=(const Char2(&arr)[Size])
	{
		ptr = nullptr;
		count = 0;
		Assign((const Char*)arr, Size);
		return *this;
	}
	template<typename Char>
	template<ContainerConvertibleToString<const Char> Container> requires ContiguousContainer<Container>
	inline constexpr GenericStringView<Char>& GenericStringView<Char>::operator=(const Container& container)
	{
		ptr = nullptr;
		count = 0;
		Assign((const Char*)&*begin(container), end(container) - begin(container));
		return *this;
	}
	template<typename Char>
	inline constexpr void GenericStringView<Char>::Assign(const Char* ptr, uintMem bufferSize)
	{
		if (bufferSize == 0)
			return;

		if (ptr[bufferSize - 1] == '\0')
		{
			if (bufferSize == 1)
				return;

			count = bufferSize - 1;
		}
		else
			count = bufferSize;

		this->ptr = ptr;
	}

	template<typename Char, AllocatorType Allocator = DefaultAllocator>
	GenericString<Char, Allocator> operator+(const GenericStringView<Char>& a, const GenericStringView<Char>& b)
	{
		return GenericStringView<Char>::Concat(a, b);
	}
	template<typename Char, AllocatorType Allocator = DefaultAllocator>
	GenericString<Char, Allocator> operator+(const GenericStringView<Char>& a, const GenericString<Char, Allocator>& b)
	{
		return GenericStringView<Char>(a) + GenericStringView<Char>(b);
	}
	template<typename Char, AllocatorType Allocator = DefaultAllocator, TriviallyConvertibleCharacter<Char> Char2, uintMem Size>
	GenericString<Char, Allocator> operator+(const GenericStringView<Char>& a, const Char2(&b)[Size])
	{
		return GenericStringView<Char>(a) + GenericStringView<Char>(b);
	}
	template<typename Char, AllocatorType Allocator = DefaultAllocator, TriviallyConvertibleCharacter<Char> Char2, uintMem Size>
	GenericString<Char, Allocator> operator+(const Char2(&a)[Size], const GenericStringView<Char>& b)
	{
		return GenericStringView<Char>(a) + GenericStringView<Char>(b);
	}
}