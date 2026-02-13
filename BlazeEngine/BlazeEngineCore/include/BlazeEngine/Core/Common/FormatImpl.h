#pragma once
#include "BlazeEngine/Core/Common/Format.h"

namespace Blaze
{
	template<typename Char>
	inline FormatParseContext<Char>::FormatParseContext(const GenericStringView<Char>& str)
		: formatString(str)
	{
	}
	template<typename Char>
	inline GenericStringView<Char> FormatParseContext<Char>::GetString() const
	{
		return formatString;
	}

	template<typename Char>
	inline FormatContext<Char>::FormatContext(Char* buffer, uintMem startPosition, uintMem endPosition)
		: buffer(buffer), position(startPosition), endPosition(endPosition)
	{
	}
	template<typename Char>
	inline uintMem FormatContext<Char>::Write(const GenericStringView<Char>& string)
	{
		return Write(string.Ptr(), string.Count());
	}
	template<typename Char>
	inline uintMem FormatContext<Char>::Write(const Char* str, uintMem len)
	{
		if (len + position > endPosition)
			len = endPosition - position;

		memcpy(buffer + position, str, len * sizeof(Char));

		position += len;
		return len;
	}
	template<typename Char>
	inline uintMem FormatContext<Char>::Write(Char c)
	{
		if (position == endPosition)
			return 0;

		buffer[position] = c;

		position += 1;
		return 1;
	}

	template<typename Char, Formattable<Char> ...Args>
	uintMem FormatSize(const GenericStringView<Char>& formatString, const Tuple<const Args& ...>& args, FormatArgumentData(&formatArgumentData)[sizeof...(Args)], Tuple<Formatter<Args, Char>...>& formatters)
	{
		if constexpr (sizeof ...(Args) == 0)
			return formatString;
		else
		{
			GenericStringView<Char> openBraceCharacter;
			GenericStringView<Char> closedBraceCharacter;
			GenericStringView<Char> colonCharacter;

			if constexpr (SameAs<Char, char>) { openBraceCharacter = "{"; closedBraceCharacter = "}"; colonCharacter = ":"; }
			else if constexpr (SameAs<Char, char8_t>) { openBraceCharacter = u8"{"; closedBraceCharacter = u8"}"; colonCharacter = u8":"; }
			else if constexpr (SameAs<Char, char16_t>) { openBraceCharacter = u"{"; closedBraceCharacter = u"}"; colonCharacter = u":"; }
			else if constexpr (SameAs<Char, char32_t>) { openBraceCharacter = U"{"; closedBraceCharacter = U"}"; colonCharacter = U":"; }

			uintMem parseStringPosition = 0;
			uintMem outSize = 0;

			formatters.ForEach(
				[&]<uintMem I, typename Arg>(Formatter<Arg, Char>&formatter) {
				formatArgumentData[I].outputStringPosition = outSize;
				formatArgumentData[I].formatStringParseBegin = parseStringPosition;
				formatArgumentData[I].formatStringParseEnd = parseStringPosition;

				uintMem openBraceIndex = formatString.Find(openBraceCharacter, parseStringPosition);

				if (openBraceIndex == SIZE_MAX)
					return;

				formatArgumentData[I].formatStringParseBegin = openBraceIndex;

				uintMem closedBraceIndex = formatString.Find(closedBraceCharacter, openBraceIndex);

				if (closedBraceIndex == SIZE_MAX || closedBraceIndex < openBraceIndex)
				{
					parseStringPosition = openBraceIndex;
					formatArgumentData[I].formatStringParseEnd = openBraceIndex;
					return;
				}

				auto parseSubString = formatString.SubString(openBraceIndex + 1, closedBraceIndex);
				uintMem colonIndex = parseSubString.Find(colonCharacter);

				auto specifiersSubString = parseSubString.SubString(colonIndex == SIZE_MAX ? 0 : colonIndex + 1, parseSubString.Count());

				const Arg& value = args.GetValueByIndex<I>();
				FormatParseContext<Char> parseContext(specifiersSubString);
				uintMem size = formatter.Parse(value, parseContext);

				formatArgumentData[I].formatedSize = size;

				outSize += openBraceIndex - parseStringPosition + size;

				parseStringPosition = closedBraceIndex + 1;
				formatArgumentData[I].formatStringParseEnd = closedBraceIndex + 1;
			});

			outSize += formatString.Count() - parseStringPosition;

			return outSize;
		}
	}
	template<typename Char, Formattable<Char> ...Args>
	uintMem FormatSize(const GenericStringView<Char>& formatString, const Args & ... args)
	{
		if constexpr (sizeof ...(Args) == 0)
			return formatString;
		else
		{
			FormatArgumentData formatArgumentData[sizeof...(args)];
			Tuple<Formatter<Args, Char>...> formatters;
			Tuple<const Args& ...> containedArgs{ args... };

			return FormatSize<Char, Args...>(formatString, containedArgs, formatArgumentData, formatters);
		}
	}

	template<typename Char, Formattable<Char> ...Args>
	uintMem FormatInto(Char* ptr, uintMem size, const GenericStringView<Char>& formatString, const Tuple<const Args& ...>& args, FormatArgumentData(&formatArgumentData)[sizeof...(Args)], Tuple<Formatter<Args, Char>...>& formatters)
	{
		if constexpr (sizeof ...(Args) == 0)
			return formatString;
		else
		{
			auto CopySafe = [](Char* dst, uintMem dstOffset, uintMem dstSize, const Char* src, uintMem srcSize) -> uintMem {
				if (dstOffset >= dstSize)
					return 0;

				if (dstOffset + srcSize > dstSize)
					srcSize = dstSize - dstOffset;

				memcpy(dst + dstOffset, src, srcSize * sizeof(Char));

				return srcSize;
				};

			if (formatArgumentData[0].formatStringParseBegin != 0 && CopySafe(ptr, 0, size, formatString.Ptr(), formatArgumentData[0].formatStringParseBegin) == 0)
				return size;

			uintMem outputStringPosition = formatArgumentData[0].formatStringParseBegin;

			formatters.ForEach(
				[&]<uintMem I, typename Arg>(Formatter<Arg, Char>&formatter) {
				if (outputStringPosition == size)
					return;

				const Arg& value = args.GetValueByIndex<I>();

				FormatContext<Char> context{ ptr, outputStringPosition, outputStringPosition + formatArgumentData[I].formatedSize };
				formatter.Format(value, context);

				if (outputStringPosition + formatArgumentData[I].formatedSize > size)
				{
					outputStringPosition = size;
					return;
				}
				outputStringPosition += formatArgumentData[I].formatedSize;

				uintMem nextParseStringBegin = formatString.Count();
				if constexpr (I + 1 < sizeof ... (Args))
					nextParseStringBegin = formatArgumentData[I + 1].formatStringParseBegin;

				if (nextParseStringBegin - formatArgumentData[I].formatStringParseEnd != 0 &&
					CopySafe(ptr, outputStringPosition, size, formatString.Ptr() + formatArgumentData[I].formatStringParseEnd, nextParseStringBegin - formatArgumentData[I].formatStringParseEnd) == 0)
				{
					outputStringPosition = size;
					return;
				}

				outputStringPosition += nextParseStringBegin - formatArgumentData[I].formatStringParseEnd;
			});

			return outputStringPosition;
		}
	}
	template<typename Char, Formattable<Char> ...Args>
	uintMem FormatInto(Char* ptr, uintMem size, const GenericStringView<Char>& formatString, const Args & ... args)
	{
		if constexpr (sizeof ...(Args) == 0)
			return formatString;
		else
		{
			Tuple<Formatter<Args, Char>...> formatters{ };
			FormatArgumentData formatArgumentData[sizeof ... (Args)];
			Tuple<const Args& ...> containedArgs{ args... };

			FormatSize(formatString, containedArgs, formatArgumentData, formatters);

			return FormatInto<Char, Args...>(ptr, size, formatString, containedArgs, formatArgumentData, formatters);
		}
	}

	template<typename Char, AllocatorType Allocator, Formattable<Char> ...Args>
	GenericString<Char, Allocator> Format(const GenericStringView<Char>& formatString, const Args & ... args)
	{
		if constexpr (sizeof ...(Args) == 0)
			return formatString;
		else
		{
			Tuple<Formatter<Args, Char>...> formatters{ };
			FormatArgumentData formatArgumentData[sizeof ... (Args)];
			Tuple<const Args& ...> containedArgs{ args... };

			GenericString<Char, Allocator> out(FormatSize<Char, Args...>(formatString, containedArgs, formatArgumentData, formatters));

			FormatInto<Char, Args...>(out.Ptr(), out.Count(), formatString, containedArgs, formatArgumentData, formatters);

			return out;
		}
	}
	template<typename Char, AllocatorType Allocator0, AllocatorType Allocator, Formattable<Char> ...Args >
	GenericString<Char, Allocator> Format(const GenericString<Char, Allocator0>& string, const Args & ... args)
	{
		return Format<Char, Allocator, Args...>(GenericStringView<Char>(string), args...);
	}
	template<typename Char, uintMem Size, AllocatorType Allocator, Formattable<Char> ...Args>
	GenericString<Char, Allocator> Format(const Char(&string)[Size], const Args & ... args)
	{
		return Format<Char, Allocator, Args...>(GenericStringView<Char>(string), args...);
	}

	template<typename Char> requires OneOf<Char, char, char8_t, char16_t, char32_t>
	inline uintMem BaseFormatter<Char>::ParseAlignment(GenericStringView<Char> string, const FormatParseContext<Char>& parseContext)
	{
		fill = static_cast<char32_t>(' ');
		align = 0;

		uint8 parsedAlign = 2;

		GenericStringView<Char> leftAlignString;
		GenericStringView<Char> rightAlignString;
		GenericStringView<Char> centerAlignString;

		if constexpr (SameAs<Char, char>) { leftAlignString = "<"; rightAlignString = ">"; centerAlignString = "^"; }
		else if constexpr (SameAs<Char, char8_t>) { leftAlignString = u8"<"; rightAlignString = u8">"; centerAlignString = u8"^"; }
		else if constexpr (SameAs<Char, char16_t>) { leftAlignString = u"<"; rightAlignString = u">"; centerAlignString = u"^"; }
		else if constexpr (SameAs<Char, char32_t>) { leftAlignString = U"<"; rightAlignString = U">"; centerAlignString = U"^"; }

		uintMem i = SIZE_MAX;

		if (i = string.FindReverse(leftAlignString); i != SIZE_MAX)
			parsedAlign = 0;
		else if (i = string.FindReverse(rightAlignString); i != SIZE_MAX)
			parsedAlign = 1;
		else if (i = string.FindReverse(centerAlignString); i != SIZE_MAX)
			parsedAlign = 2;

		if (i != SIZE_MAX)
		{
			if (i != 0)
			{
				//Read fill character
				UnicodeChar ch;
				if (uintMem j = ch.FromChars(string.Ptr(), i); j != i)
				{
					//Invalid format
					if (j == 0)
						BLAZE_LOG_WARNING_BASIC("Invalid integer format specifier. Alignment was used wit fill characters and it failed to convert to a unicode character");
					else
						BLAZE_LOG_WARNING_BASIC("Invalid integer format specifier. Alignment was used with multiple fill characters");
				}
				else
					fill = ch.Value();

			}

			align = parsedAlign;
			return i + 1;
		}

		return 0;
	}
	template<typename Char> requires OneOf<Char, char, char8_t, char16_t, char32_t>
	inline uintMem BaseFormatter<Char>::ParseWidth(GenericStringView<Char> string, const FormatParseContext<Char>& parseContext)
	{
		uintMem parsedCharacterWidth = 0;

		uintMem i = 0;
		while (i < string.Count())
		{
			UnicodeChar ch;
			uintMem charWidth = ch.FromChars(string.Ptr() + i, string.Count() - i);

			if (charWidth == 0)
			{
				BLAZE_LOG_WARNING_BASIC("Invalid integer format specifier. Failed to convert to a unicode character");
				break;
			}

			uintMem digit = 0;
			switch (ch.Value())
			{
			case static_cast<char32_t>('0'): digit = 0; break;
			case static_cast<char32_t>('1'): digit = 1; break;
			case static_cast<char32_t>('2'): digit = 2; break;
			case static_cast<char32_t>('3'): digit = 3; break;
			case static_cast<char32_t>('4'): digit = 4; break;
			case static_cast<char32_t>('5'): digit = 5; break;
			case static_cast<char32_t>('6'): digit = 6; break;
			case static_cast<char32_t>('7'): digit = 7; break;
			case static_cast<char32_t>('8'): digit = 8; break;
			case static_cast<char32_t>('9'): digit = 9; break;
			default: digit = 10; break;
			}

			if (digit > 9)
				break;

			parsedCharacterWidth = parsedCharacterWidth * 10 + digit;

			i += charWidth;
		}

		characterWidth = parsedCharacterWidth;
		return i;
	}

	template<typename SrcChar, typename DstChar>
	inline uintMem Formatter<GenericStringView<SrcChar>, DstChar>::Parse(const GenericStringView<SrcChar>& value, const FormatParseContext<DstChar>& parseContext)
	{
		uintMem start = 0;
		GenericStringView<DstChar> string = parseContext.GetString();
		start += this->ParseAlignment(GenericStringView<DstChar>(string.Ptr() + start, string.Count() - start), parseContext);
		start += this->ParseWidth(GenericStringView<DstChar>(string.Ptr() + start, string.Count() - start), parseContext);

		if constexpr (SameAs<SrcChar, DstChar>)
		{
			convertedLength = value.Count();
			return this->characterWidth > value.Count() ? this->characterWidth : value.Count();
		}
		else
		{
			uintMem count = 0;
			uintMem i = 0;
			while (i < value.Count())
			{
				UnicodeChar ch;
				i += ch.FromChars(value.Ptr() + i, value.Count() - i);
				count += ch.ToChars<DstChar>().count;
			}

			convertedLength = count;
			return this->characterWidth > count ? this->characterWidth : count;
		}
	}
	template<typename SrcChar, typename DstChar>
	inline void Formatter<GenericStringView<SrcChar>, DstChar>::Format(const GenericStringView<SrcChar>& string, FormatContext<DstChar>& context)
	{
		uintMem preFillCount = 0, postFillCount = 0;

		if (this->characterWidth != 0)
		{
			if (convertedLength >= this->characterWidth)
			{
				preFillCount = 0;
				postFillCount = 0;
			}
			else
			{
				if (this->align == 0)
				{
					preFillCount = 0;
					postFillCount = this->characterWidth - convertedLength;
				}
				else if (this->align == 1)
				{
					preFillCount = this->characterWidth - convertedLength;
					postFillCount = 0;
				}
				else if (this->align == 2)
				{
					preFillCount = (this->characterWidth - convertedLength) / 2;
					postFillCount = this->characterWidth - convertedLength - preFillCount;
				}
			}
		}

		for (uintMem i = 0; i < preFillCount; ++i)
			context.Write(UnicodeChar(this->fill).ToChars<DstChar>().AsString());

		if constexpr (SameAs<SrcChar, DstChar>)
			context.Write(string);
		else
		{
			uintMem i = 0;
			while (i < string.Count())
			{
				UnicodeChar ch;
				i += ch.FromChars(string.Ptr() + i, string.Count() - i);
				context.Write(ch.ToChars<DstChar>().AsString());
			}
		}

		for (uintMem i = 0; i < postFillCount; ++i)
			context.Write(UnicodeChar(this->fill).ToChars<DstChar>().AsString());
	}

	template<typename SrcChar, typename DstChar> requires Formattable<GenericStringView<SrcChar>, DstChar>
	inline uintMem Formatter<GenericString<SrcChar>, DstChar>::Parse(const GenericString<SrcChar>& string, const FormatParseContext<DstChar>& parseContext)
	{
		return Formatter<GenericStringView<SrcChar>, DstChar>::Parse(GenericStringView<SrcChar>(string), parseContext);
	}
	template<typename SrcChar, typename DstChar> requires Formattable<GenericStringView<SrcChar>, DstChar>
	inline void Formatter<GenericString<SrcChar>, DstChar>::Format(const GenericString<SrcChar>& string, FormatContext<DstChar>& context)
	{
		Formatter<GenericStringView<SrcChar>, DstChar>::Format(GenericStringView<SrcChar>(string), context);
	}

	template<typename Char>
	inline uintMem Formatter<bool, Char>::Parse(const bool& value, const FormatParseContext<Char>& parseContext)
	{
		uintMem start = 0;
		GenericStringView<Char> string = parseContext.GetString();
		start += this->ParseAlignment(GenericStringView<Char>(string.Ptr() + start, string.Count() - start), parseContext);
		start += this->ParseWidth(GenericStringView<Char>(string.Ptr() + start, string.Count() - start), parseContext);
		
		uintMem convertedLength = value ? 4 : 5;
		this->characterWidth = (convertedLength > this->characterWidth ? convertedLength : this->characterWidth);
		return this->characterWidth;
	}
	template<typename Char>
	inline void Formatter<bool, Char>::Format(const bool& value, FormatContext<Char>& context)
	{
		uintMem preFillCount = 0, postFillCount = 0;
		
		uintMem convertedLength = value ? 4 : 5;

		if (this->align == 0)
		{
			preFillCount = 0;
			postFillCount = this->characterWidth - convertedLength;
		}
		else if (this->align == 1)
		{
			preFillCount = this->characterWidth - convertedLength;
			postFillCount = 0;
		}
		else if (this->align == 2)
		{
			preFillCount = (this->characterWidth - convertedLength) / 2;
			postFillCount = this->characterWidth - convertedLength - preFillCount;
		}

		for (uintMem i = 0; i < preFillCount; ++i)
			context.Write(UnicodeChar(this->fill).ToChars<Char>().AsString());

		if (value)
			context.Write("true");
		else
			context.Write("false");

		for (uintMem i = 0; i < postFillCount; ++i)
			context.Write(UnicodeChar(this->fill).ToChars<Char>().AsString());
	}

	template<typename Char> requires OneOf<Char, char, char8_t, char16_t, char32_t>
	inline uintMem BaseIntegerFormatter<Char>::ParseFormatSpecifiers(const FormatParseContext<Char>& parseContext)
	{
		uintMem start = 0;
		GenericStringView<Char> string = parseContext.GetString();

		start += this->ParseAlignment       ({ string.Ptr() + start, string.Ptr() + string.Count() }, parseContext);
		start += ParseSign            ({ string.Ptr() + start, string.Ptr() + string.Count() }, parseContext);
		start += ParseAlternateValue  ({ string.Ptr() + start, string.Ptr() + string.Count() }, parseContext);
		start += ParseSignAwarePadding({ string.Ptr() + start, string.Ptr() + string.Count() }, parseContext);
		start += this->ParseWidth           ({ string.Ptr() + start, string.Ptr() + string.Count() }, parseContext);
		start += ParseType            ({ string.Ptr() + start, string.Ptr() + string.Count() }, parseContext);
		return start;
	}
	template<typename Char> requires OneOf<Char, char, char8_t, char16_t, char32_t>
	template<IntegerType T>
	inline uintMem BaseIntegerFormatter<Char>::ParseValue(T value)
	{
		if constexpr (SignedIntegerType<T>)
			if (alternateValue == 1)
				return ParseValue(static_cast<MakeUnsigned<T>>(value));

		if (type == 1)
		{
			*reinterpret_cast<char32_t*>(symbols) = static_cast<char32_t>(value);
			return UnicodeChar(static_cast<char32_t>(value)).ToCharsSize<Char>();
		}

		char* dst = symbols;

		if (alternateValue == 0) //the alternate form wont hande signs
		{
			if (sign == 1) //sign for both negative and positive integers
			{
				if (value >= 0)
				{
					*dst = '+';
					++dst;
				}
			}
			else if (sign == 2) //space for positive integers
			{
				if (value >= 0)
				{
					*dst = ' ';
					++dst;
				}
			}
		}

		if (auto [ptr, ec] = std::to_chars(dst, symbols + _countof(symbols) - 1, value, base); ec == std::errc())
		{
			if (alternateValue == 1) //Add prefix 0x or 0b
			{
				if (base == 16)
				{
					uintMem zeroDigitCount = sizeof(T) * 2 - (ptr - dst);

					//Make room for 0x and 0 digits
					memmove(dst + 2 + zeroDigitCount, dst, ptr - dst);
					ptr += 2 + zeroDigitCount;

					*dst = '0'; ++dst;
					*dst = 'x'; ++dst; //uppercase will be handled later

					for (uintMem i = 0; i < zeroDigitCount; ++i, ++dst)
						*dst = '0';
				}
				else if (base == 2)
				{
					uintMem zeroDigitCount = sizeof(T) * 8 - (ptr - dst);

					//Make room for 0b and 0 digits
					memmove(dst + 2 + zeroDigitCount, dst, ptr - dst);
					ptr += 2 + zeroDigitCount;

					*dst = '0'; ++dst;
					*dst = 'b'; ++dst; //uppercase will be handled later

					for (uintMem i = 0; i < zeroDigitCount; ++i, ++dst)
						*dst = '0';
				}
			}

			uintMem symbolsCount = ptr - symbols;

			*ptr = '\0';

			if (uppercase)
				for (char* it = symbols; it != ptr; ++it)
					if (*it >= 'a' && *it <= 'z')
						*it = *it - ('a' - 'A');

			if (this->characterWidth != 0)
				if (symbolsCount >= this->characterWidth)
					return symbolsCount;
				else
					return (this->characterWidth - symbolsCount) * UnicodeChar(this->fill).ToCharsSize<Char>() + symbolsCount;

			return symbolsCount;
		}
		else
		{
			BLAZE_LOG_WARNING_BASIC("std::to_chars failed to convert integer to string");
			symbols[0] = '0';
			symbols[1] = '\0';
			return 1;
		}
	}
	template<typename Char> requires OneOf<Char, char, char8_t, char16_t, char32_t>
	inline void BaseIntegerFormatter<Char>::CopyFormated(FormatContext<Char>& context)
	{
		uintMem preFillCount = 0, postFillCount = 0;

		if (this->characterWidth != 0)
		{
			uintMem formattedWidth = 0;
			char* it = symbols;
			while (*it != '\0' && formattedWidth < _countof(symbols))
			{
				++formattedWidth;
				++it;
			}

			if (formattedWidth >= this->characterWidth)
			{
				preFillCount = 0;
				postFillCount = 0;
			}
			else
			{
				if (this->align == 0)
				{
					preFillCount = 0;
					postFillCount = this->characterWidth - formattedWidth;
				}
				else if (this->align == 1)
				{
					preFillCount = this->characterWidth - formattedWidth;
					postFillCount = 0;
				}
				else if (this->align == 2)
				{
					preFillCount = (this->characterWidth - formattedWidth) / 2;
					postFillCount = this->characterWidth - formattedWidth - preFillCount;
				}
			}
		}

		bool signFirst = type == 0 && signAwarePadding == 1 && alternateValue == 0 && (symbols[0] == '-' || symbols[0] == '+');

		if (signFirst)
			context.Write(UnicodeChar(symbols[0]).ToChars<Char>().AsString());

		for (uintMem i = 0; i < preFillCount; ++i)
			context.Write(UnicodeChar(this->fill).ToChars<Char>().AsString());

		if (type == 0)
			for (uintMem i = signFirst ? 1 : 0; symbols[i] != '\0' && i < _countof(symbols); ++i)
				context.Write(UnicodeChar(symbols[i]).ToChars<Char>().AsString());
		else
			context.Write(UnicodeChar(*reinterpret_cast<char32_t*>(symbols)).ToChars<Char>().AsString());

		for (uintMem i = 0; i < postFillCount; ++i)
			context.Write(UnicodeChar(this->fill).ToChars<Char>().AsString());
	}
	template<typename Char> requires OneOf<Char, char, char8_t, char16_t, char32_t>
	inline uintMem BaseIntegerFormatter<Char>::ParseSign(GenericStringView<Char> string, const FormatParseContext<Char>& parseContext)
	{
		sign = 0;

		if (string.Empty())
			return 0;

		switch (string[0])
		{
		case static_cast<Char>('-'): sign = 0; break;
		case static_cast<Char>('+'): sign = 1; break;
		case static_cast<Char>(' '): sign = 2; break;
		default: return 0;
		}

		return 1;
	}
	template<typename Char> requires OneOf<Char, char, char8_t, char16_t, char32_t>
	inline uintMem BaseIntegerFormatter<Char>::ParseAlternateValue(GenericStringView<Char> string, const FormatParseContext<Char>& parseContext)
	{
		alternateValue = 0;

		if (string.Empty())
			return 0;

		if (string[0] == static_cast<Char>('#'))
		{
			alternateValue = 1;
			return 1;
		}

		return 0;
	}
	template<typename Char> requires OneOf<Char, char, char8_t, char16_t, char32_t>
	inline uintMem BaseIntegerFormatter<Char>::ParseSignAwarePadding(GenericStringView<Char> string, const FormatParseContext<Char>& parseContext)
	{
		signAwarePadding = 0;

		if (string.Empty())
			return 0;

		if (string[0] == static_cast<Char>('0'))
		{
			signAwarePadding = 1;
			return 1;
		}

		return 0;
	}
	template<typename Char> requires OneOf<Char, char, char8_t, char16_t, char32_t>
	inline uintMem BaseIntegerFormatter<Char>::ParseType(GenericStringView<Char> string, const FormatParseContext<Char>& parseContext)
	{
		type = 0;
		base = 10;
		uppercase = false;

		if (string.Empty())
			return 0;

		switch (string[0])
		{
		case static_cast<Char>('b'): type = 0; base = 2; uppercase = false; break;
		case static_cast<Char>('B'): type = 0; base = 2; uppercase = true; break;
		case static_cast<Char>('c'): type = 1; base = 0; uppercase = false; break;
		case static_cast<Char>('d'): type = 0; base = 10; uppercase = false; break;
		case static_cast<Char>('o'): type = 0; base = 8; uppercase = false; break;
		case static_cast<Char>('x'): type = 0; base = 16; uppercase = false; break;
		case static_cast<Char>('X'): type = 0; base = 16; uppercase = true; break;
		default: return 0;
		}

		return 1;
	}

	template<typename Char> requires OneOf<Char, char, char8_t, char16_t, char32_t>
	inline uintMem BaseDecimalFormatter<Char>::ParseFormatSpecifiers(const FormatParseContext<Char>& parseContext)
	{
		uintMem start = 0;
		GenericStringView<Char> string = parseContext.GetString();

		errorFlag = 0;
		start += this->ParseAlignment({ string.Ptr() + start, string.Ptr() + string.Count() }, parseContext);
		start += ParseSign({ string.Ptr() + start, string.Ptr() + string.Count() }, parseContext);
		start += ParseAlternateValue({ string.Ptr() + start, string.Ptr() + string.Count() }, parseContext);
		start += ParseSignAwarePadding({ string.Ptr() + start, string.Ptr() + string.Count() }, parseContext);
		start += this->ParseWidth({ string.Ptr() + start, string.Ptr() + string.Count() }, parseContext);
		start += ParsePrecision({ string.Ptr() + start, string.Ptr() + string.Count() }, parseContext);
		start += ParseType({ string.Ptr() + start, string.Ptr() + string.Count() }, parseContext);
		return start;
	}
	template<typename Char> requires OneOf<Char, char, char8_t, char16_t, char32_t>
	template<DecimalType T>
	inline uintMem BaseDecimalFormatter<Char>::ParseValue(T value)
	{
		if constexpr (SignedIntegerType<T>)
			if (alternateValue == 1)
				return ParseValue(static_cast<MakeUnsigned<T>>(value));

		char* dst = symbols;

		if (sign == 1) //sign for both negative and positive integers
		{
			if (value >= T(0))
			{
				*dst = '+';
				++dst;
			}
		}
		else if (sign == 2) //space for positive integers
		{
			if (value >= T(0))
			{
				*dst = ' ';
				++dst;
			}
		}

		if (value < T(0))
		{
			*dst = '-';
			++dst;
			value = -value;
		}

		std::chars_format _format;

		switch (type)
		{
		case 0:
			_format = std::chars_format::hex;
			break;
		case 1:
			_format = std::chars_format::scientific;
			break;
		case 2:
			_format = std::chars_format::fixed;
			break;
		case 3:
			_format = std::chars_format::general;
			break;
		default:
			break;
		}

		if (auto [end, ec] = std::to_chars(dst, symbols + _countof(symbols) - 1, value, _format); ec == std::errc())
		{
			if (type == 0)
			{
				memmove(dst + 2, dst, end - dst);
				*dst = '0'; ++dst;
				*dst = 'x'; ++dst; //uppercase will be handled later
				end += 2;
			}
			else
			{
				DecimalParts parts = DivideSymbols(end);

				if (alternateValue == 1 || precision > 0)
					AssertDotIntoNumber(parts, end);

				if (precision > 0)
					AssertPrecisionIntoNumber(parts, end);
			}

			uintMem symbolsCount = end - symbols;
			*end = '\0';

			if (uppercase)
				for (char* it = symbols; it != end; ++it)
					if (*it >= 'a' && *it <= 'z')
						*it = *it - ('a' - 'A');

			if (this->characterWidth != 0)
				if (symbolsCount >= this->characterWidth)
					return symbolsCount;
				else
					return (this->characterWidth - symbolsCount) * UnicodeChar(this->fill).ToCharsSize<Char>() + symbolsCount;

			return symbolsCount;
		}
		else
		{
			BLAZE_LOG_WARNING_BASIC("std::to_chars failed to convert integer to string");
			symbols[0] = '0';
			symbols[1] = '\0';
			return 1;
		}
	}
	template<typename Char> requires OneOf<Char, char, char8_t, char16_t, char32_t>
	inline void BaseDecimalFormatter<Char>::CopyFormated(FormatContext<Char>& context)
	{
		if (errorFlag == 1)
		{
			for (char* it = symbols; it < symbols + _countof(symbols) && *it != '\0'; ++it)
				context.Write(UnicodeChar(*it).ToChars<Char>().AsString());
			return;
		}

		uintMem preFillCount = 0, postFillCount = 0;

		if (this->characterWidth != 0)
		{
			uintMem formattedWidth = 0;
			char* it = symbols;
			while (*it != '\0' && formattedWidth < _countof(symbols))
			{
				++formattedWidth;
				++it;
			}

			if (formattedWidth >= this->characterWidth)
			{
				preFillCount = 0;
				postFillCount = 0;
			}
			else
			{
				if (this->align == 0)
				{
					preFillCount = 0;
					postFillCount = this->characterWidth - formattedWidth;
				}
				else if (this->align == 1)
				{
					preFillCount = this->characterWidth - formattedWidth;
					postFillCount = 0;
				}
				else if (this->align == 2)
				{
					preFillCount = (this->characterWidth - formattedWidth) / 2;
					postFillCount = this->characterWidth - formattedWidth - preFillCount;
				}
			}
		}

		bool signFirst = signAwarePadding == 1 && (symbols[0] == '-' || symbols[0] == '+');

		if (signFirst)
			context.Write(UnicodeChar(symbols[0]).ToChars<Char>().AsString());

		for (uintMem i = 0; i < preFillCount; ++i)
			context.Write(UnicodeChar(this->fill).ToChars<Char>().AsString());

		for (uintMem i = signFirst ? 1 : 0; symbols[i] != '\0' && i < _countof(symbols); ++i)
			context.Write(UnicodeChar(symbols[i]).ToChars<Char>().AsString());

		for (uintMem i = 0; i < postFillCount; ++i)
			context.Write(UnicodeChar(this->fill).ToChars<Char>().AsString());
	}
	template<typename Char> requires OneOf<Char, char, char8_t, char16_t, char32_t>
	inline uintMem BaseDecimalFormatter<Char>::ParseSign(GenericStringView<Char> string, const FormatParseContext<Char>& parseContext)
	{
		sign = 0;

		if (string.Empty())
			return 0;

		switch (string[0])
		{
		case static_cast<Char>('-'): sign = 0; break;
		case static_cast<Char>('+'): sign = 1; break;
		case static_cast<Char>(' '): sign = 2; break;
		default: return 0;
		}

		return 1;
	}
	template<typename Char> requires OneOf<Char, char, char8_t, char16_t, char32_t>
	inline uintMem BaseDecimalFormatter<Char>::ParseAlternateValue(GenericStringView<Char> string, const FormatParseContext<Char>& parseContext)
	{
		alternateValue = 0;

		if (string.Empty())
			return 0;

		if (string[0] == static_cast<Char>('#'))
		{
			alternateValue = 1;
			return 1;
		}

		return 0;
	}
	template<typename Char> requires OneOf<Char, char, char8_t, char16_t, char32_t>
	inline uintMem BaseDecimalFormatter<Char>::ParseSignAwarePadding(GenericStringView<Char> string, const FormatParseContext<Char>& parseContext)
	{
		signAwarePadding = 0;

		if (string.Empty())
			return 0;

		if (string[0] == static_cast<Char>('0'))
		{
			signAwarePadding = 1;
			return 1;
		}

		return 0;
	}
	template<typename Char> requires OneOf<Char, char, char8_t, char16_t, char32_t>
	inline uintMem BaseDecimalFormatter<Char>::ParsePrecision(GenericStringView<Char> string, const FormatParseContext<Char>& parseContext)
	{
		precision = 0;

		if (string.Empty())
			return 0;

		if (string[0] == static_cast<Char>('.'))
		{
			uintMem i = 1;
			uintMem parsedPrecision = 0;

			while (i < string.Count())
			{
				UnicodeChar ch;
				uintMem charWidth = ch.FromChars(string.Ptr() + i, string.Count() - i);

				if (charWidth == 0)
				{
					BLAZE_LOG_WARNING_BASIC("Invalid integer format specifier. Failed to convert to a unicode character");
					break;
				}

				uintMem digit = 0;
				switch (ch.Value())
				{
				case static_cast<char32_t>('0'): digit = 0; break;
				case static_cast<char32_t>('1'): digit = 1; break;
				case static_cast<char32_t>('2'): digit = 2; break;
				case static_cast<char32_t>('3'): digit = 3; break;
				case static_cast<char32_t>('4'): digit = 4; break;
				case static_cast<char32_t>('5'): digit = 5; break;
				case static_cast<char32_t>('6'): digit = 6; break;
				case static_cast<char32_t>('7'): digit = 7; break;
				case static_cast<char32_t>('8'): digit = 8; break;
				case static_cast<char32_t>('9'): digit = 9; break;
				default: digit = 10; break;
				}

				if (digit > 9)
					break;

				parsedPrecision = parsedPrecision * 10 + digit;

				i += charWidth;
			}

			precision = parsedPrecision;

			return i;
		}

		return 0;
	}
	template<typename Char> requires OneOf<Char, char, char8_t, char16_t, char32_t>
	inline uintMem BaseDecimalFormatter<Char>::ParseType(GenericStringView<Char> string, const FormatParseContext<Char>& parseContext)
	{
		type = 3;
		uppercase = false;

		if (string.Empty())
			return 0;

		switch (string[0])
		{
		case static_cast<Char>('a'): type = 0; uppercase = false; break;
		case static_cast<Char>('A'): type = 0; uppercase = true; break;
		case static_cast<Char>('e'): type = 1; uppercase = false; break;
		case static_cast<Char>('E'): type = 1; uppercase = true; break;
		case static_cast<Char>('f'): type = 2; uppercase = false; break;
		case static_cast<Char>('F'): type = 2; uppercase = true; break;
		case static_cast<Char>('g'): type = 3; uppercase = false; break;
		case static_cast<Char>('G'): type = 3; uppercase = true; break;
		default: return 0;
		}

		return 1;
	}
	template<typename Char> requires OneOf<Char, char, char8_t, char16_t, char32_t>
	inline uintMem BaseDecimalFormatter<Char>::ParseError(StringView message)
	{
		errorFlag = 1;
		BLAZE_LOG_ERROR_BASIC(message);
		strcpy_s(symbols, "ERROR_FORMATTING");
		return strnlen_s(symbols, _countof(symbols));
	}
	template<typename Char> requires OneOf<Char, char, char8_t, char16_t, char32_t>
	inline auto BaseDecimalFormatter<Char>::DivideSymbols(char* end) -> DecimalParts
	{
		DecimalParts parts;
		char* it = symbols;

		parts.signLength = it != end && (*it == '-' || *it == '+' || *it == ' ') ? 1 : 0;
		it += parts.signLength;

		if (it == end || !(*it >= '0' && *it <= '9'))
		{
			ParseError("Expected digit when formatting decimal number");
			return {  };
		}

		char* numberStart = it;

		while (it != end && (*it >= '0' && *it <= '9'))
			++it;

		parts.numberLength = it - numberStart;

		parts.dotLength = it != end && *it == '.' ? 1 : 0;
		it += parts.dotLength;
		char* decimalStart = it;

		if (parts.dotLength != 0)
			while (it != end && (*it >= '0' && *it <= '9'))
				++it;

		parts.decimalLength = it - decimalStart;

		parts.exponentCharacterLength = it != end && (*it == 'e' || *it == 'E') ? 1 : 0;
		it += parts.exponentCharacterLength;

		char* exponentStart = it;
		if (parts.exponentCharacterLength != 0)
		{
			parts.exponentSignLength = it != end && (*it == '-' || *it == '+') ? 1 : 0;
			it += parts.exponentSignLength;

			if (it == end || !(*it >= '0' && *it <= '9'))
			{
				ParseError("Expected digit when formatting decimal number");
				return { };
			}

			while (it != end && (*it >= '0' && *it <= '9'))
				++it;
		}

		parts.exponentNumberLength = it - exponentStart;

		return parts;
	}
	template<typename Char> requires OneOf<Char, char, char8_t, char16_t, char32_t>
	inline void BaseDecimalFormatter<Char>::AssertDotIntoNumber(DecimalParts& parts, char*& end)
	{
		if (parts.dotLength == 0 && end + 1 < symbols + _countof(symbols))
		{
			InsertIntoSymbols(parts.signLength + parts.numberLength, ".", end);
			parts.dotLength = 1;
		}
	}
	template<typename Char> requires OneOf<Char, char, char8_t, char16_t, char32_t>
	inline void BaseDecimalFormatter<Char>::AssertPrecisionIntoNumber(DecimalParts& parts, char*& end)
	{
		if (parts.exponentCharacterLength != 0)
		{
			if (parts.decimalLength + parts.numberLength > precision)
			{
				uintMem digitsToBeRemoved = parts.decimalLength + parts.numberLength - precision;
				EraseFromSymbols(parts.signLength + parts.numberLength + parts.dotLength + precision, digitsToBeRemoved, end);
				parts.decimalLength = precision - parts.numberLength;
			}
			else while (parts.decimalLength + parts.numberLength < precision)
			{
				InsertIntoSymbols(parts.signLength + parts.numberLength + parts.dotLength + parts.decimalLength, "0", end);
				++parts.decimalLength;
			}
		}
		else
		{
			if (parts.decimalLength > precision)
			{
				uintMem digitsToBeRemoved = parts.decimalLength - precision;
				EraseFromSymbols(parts.signLength + parts.numberLength + parts.dotLength + precision, digitsToBeRemoved, end);
			}
			else while (parts.decimalLength < precision)
			{
				InsertIntoSymbols(parts.signLength + parts.numberLength + parts.dotLength + parts.decimalLength, "0", end);
				++parts.decimalLength;
			}

			parts.decimalLength = precision;
		}
	}
	template<typename Char> requires OneOf<Char, char, char8_t, char16_t, char32_t>
	inline void BaseDecimalFormatter<Char>::InsertIntoSymbols(uintMem index, StringView str, char*& end)
	{
		if (symbols + index > end)
			return;

		uintMem moveCount = end - symbols - index;
		if (index + str.Count() + moveCount > _countof(symbols) - 1)
			if (index + str.Count() > _countof(symbols) - 1)
				moveCount = 0;
			else
				moveCount = _countof(symbols) - 1 - str.Count() - index;

		for (uintMem i = 0; i < moveCount; ++i)
			symbols[index + moveCount - i - 1 + str.Count()] = symbols[index + moveCount - i - 1];

		uintMem copyCount = str.Count();
		if (index + copyCount > _countof(symbols) - 1)
			if (index > _countof(symbols) - 1)
				copyCount = 0;
			else
				copyCount = _countof(symbols) - 1 - index;

		for (uintMem i = 0; i < copyCount; ++i)
			symbols[index + i] = str[i];

		end += copyCount;
	}
	template<typename Char> requires OneOf<Char, char, char8_t, char16_t, char32_t>
	inline void BaseDecimalFormatter<Char>::EraseFromSymbols(uintMem index, uintMem count, char*& end)
	{
		if (index + count > _countof(symbols) - 1)
			count = _countof(symbols) - 1 - index;

		memmove(symbols + index, symbols + index + count, end - (symbols + index + count));
		end -= count;
	}
}