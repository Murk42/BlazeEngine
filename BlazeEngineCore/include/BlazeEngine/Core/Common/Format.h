#pragma once
#include "BlazeEngine/Core/BlazeEngineCoreDefines.h"
#include "BlazeEngine/Core/String/String.h"
#include "BlazeEngine/Core/String/UnicodeChar.h"
#include "BlazeEngine/Core/Common/Tuple.h"
#include "BlazeEngine/Core/Type/IntegerTraits.h"

namespace Blaze
{
	template<typename Char>
	class FormatParseContext
	{
	public:
		FormatParseContext(const GenericStringView<Char>& str);
		FormatParseContext(const FormatParseContext&) = delete;

		GenericStringView<Char> GetString() const;

		FormatParseContext& operator=(const FormatParseContext&) = delete;
	private:
		const GenericStringView<Char>& formatString;
	};
	template<typename Char>
	class FormatContext
	{
	public:
		FormatContext(Char* buffer, uintMem startPosition, uintMem endPosition);
		FormatContext(const FormatContext&) = delete;

		uintMem Write(const GenericStringView<Char>& string);
		uintMem Write(const Char* str, uintMem len);
		uintMem Write(Char c);

		FormatContext& operator=(const FormatContext&) = delete;
	private:
		Char* buffer;
		uintMem position;
		const uintMem endPosition;
	};

	template<typename T, typename Char>
	class Formatter;

	template<typename T, typename Char>
	concept DirectlyFormattable = requires(Formatter<RemoveCVRef<T>, Char> f, const RemoveCVRef<T>& value, FormatParseContext<Char> parseContext, FormatContext<Char> context) {
		{ f.Parse(value, parseContext) } -> SameAs<uintMem>;
		{ f.Format(value, context) };
	};

	template<typename T, typename Char>
	concept Formattable = DirectlyFormattable<T, Char>/* || OneOf<Char, char8_t, char16_t, char32_t> && (DirectlyFormattable<T, char> || DirectlyFormattable<T, char8_t> || DirectlyFormattable<T, char16_t> || DirectlyFormattable<T, char32_t>)*/;

	struct FormatArgumentData
	{
		uintMem formatStringParseBegin = 0;
		uintMem formatStringParseEnd = 0;
		uintMem formatedSize = 0;
		uintMem outputStringPosition;
	};

	template<typename Char, Formattable<Char> ...Args>
	uintMem FormatSize(const GenericStringView<Char>& formatString, const Tuple<const Args& ...>& args, FormatArgumentData(&formatArgumentData)[sizeof...(Args)], Tuple<Formatter<Args, Char>...>& formatters);
	template<typename Char, Formattable<Char> ...Args>
	uintMem FormatSize(const GenericStringView<Char>& formatString, const Args & ... args);

	template<typename Char, Formattable<Char> ...Args>
	uintMem FormatInto(Char* ptr, uintMem size, const GenericStringView<Char>& formatString, const Tuple<const Args& ...>& args, FormatArgumentData(&formatArgumentData)[sizeof...(Args)], Tuple<Formatter<Args, Char>...>& formatters);
	template<typename Char, Formattable<Char> ...Args>
	uintMem FormatInto(Char* ptr, uintMem size, const GenericStringView<Char>& formatString, const Args & ... args);

	template<typename Char, AllocatorType Allocator = DefaultAllocator, Formattable<Char> ...Args>
	GenericString<Char, Allocator> Format(const GenericStringView<Char>& formatString, const Args & ... args);
	template<typename Char, AllocatorType Allocator0, AllocatorType Allocator = Allocator0, Formattable<Char> ...Args >
	GenericString<Char, Allocator> Format(const GenericString<Char, Allocator0>& string, const Args & ... args);
	template<typename Char, uintMem Size, AllocatorType Allocator = DefaultAllocator, Formattable<Char> ...Args>
	GenericString<Char, Allocator> Format(const Char(&string)[Size], const Args & ... args);


	template<typename T>
	concept HasFormatFunctionWithSpecifiers =
		requires(const T& value, StringView specifiers) { { value.Format(specifiers) } -> IsConvertibleTo<StringView>; } ||
		requires(const T& value, u8StringView specifiers) { { value.Format(specifiers) } -> IsConvertibleTo<u8StringView>; } ||
		requires(const T& value, u16StringView specifiers) { { value.Format(specifiers) } -> IsConvertibleTo<u16StringView>; } ||
		requires(const T& value, u32StringView specifiers) { { value.Format(specifiers) } -> IsConvertibleTo<u32StringView>; };
	template<typename T>
	concept HasFormatFunction =
		requires(const T& value) { { value.Format() } -> IsConvertibleTo<StringView>; } ||
		requires(const T& value) { { value.Format() } -> IsConvertibleTo<u8StringView>; } ||
		requires(const T& value) { { value.Format() } -> IsConvertibleTo<u16StringView>; } ||
		requires(const T& value) { { value.Format() } -> IsConvertibleTo<u32StringView>; } ||
		HasFormatFunctionWithSpecifiers<T>;

	template<HasFormatFunction T, typename Char>
	class Formatter<T, Char>
	{
	public:
		uintMem Parse(const T& value, const FormatParseContext<Char>& parseContext)
		{
			if constexpr (HasFormatFunctionWithSpecifiers<T>)
				formattedString = GenericStringView(value.Format(parseContext.GetString())).ConvertToStringType<Char>();
			else
				formattedString = GenericStringView(value.Format()).ConvertToStringType<Char>();

			return formattedString.Count();
		}
		void Format(const T& value, FormatContext<Char>& context)
		{
			context.Write(formattedString);
			formattedString.Clear();
		}
	private:
		GenericString<Char> formattedString;
	};

	template<typename Char>
	class BaseFormatter;

	template<typename Char> requires OneOf<Char, char, char8_t, char16_t, char32_t>
	class BaseFormatter<Char>
	{
	public:
		uintMem characterWidth;
		char32_t fill;
		uint8 align : 2;

		uintMem ParseAlignment(GenericStringView<Char> string, const FormatParseContext<Char>& parseContext);
		uintMem ParseWidth(GenericStringView<Char> string, const FormatParseContext<Char>& parseContext);
	};

	template<typename SrcChar, typename DstChar> requires OneOf<SrcChar, char, char8_t, char16_t, char32_t>
	class Formatter<SrcChar, DstChar> : Formatter<GenericStringView<SrcChar>, DstChar>
	{
	public:
		uintMem Parse(const SrcChar& ch, const FormatParseContext<DstChar>& parseContext)
		{
			return Formatter<GenericStringView<SrcChar>, DstChar>::Parse(GenericStringView<SrcChar>(&ch, 1), parseContext);
		}
		void Format(const SrcChar& ch, FormatContext<DstChar>& context)
		{
			Formatter<GenericStringView<SrcChar>, DstChar>::Format(GenericStringView<SrcChar>(&ch, 1), context);
		}
	};

	template<typename SrcArray, typename DstChar> requires IsArrayType<SrcArray> && OneOf<ArrayValueType<SrcArray>, char, char8_t, char16_t, char32_t>
	class Formatter<SrcArray, DstChar> : Formatter<GenericStringView<RemoveConst<ArrayValueType<SrcArray>>>, DstChar>
	{
	public:
		uintMem Parse(const SrcArray& array, const FormatParseContext<DstChar>& parseContext)
		{
			return Formatter<GenericStringView<RemoveConst<ArrayValueType<SrcArray>>>, DstChar>::Parse(GenericStringView<RemoveConst<ArrayValueType<SrcArray>>>(array), parseContext);
		}
		void Format(const SrcArray& array, FormatContext<DstChar>& context)
		{
			Formatter<GenericStringView<RemoveConst<ArrayValueType<SrcArray>>>, DstChar>::Format(GenericStringView<RemoveConst<ArrayValueType<SrcArray>>>(array), context);
		}
	};

	template<typename SrcChar, typename DstChar>
	class Formatter<GenericStringView<SrcChar>, DstChar> : BaseFormatter<DstChar>
	{
	public:
		inline uintMem Parse(const GenericStringView<SrcChar>& value, const FormatParseContext<DstChar>& parseContext);
		inline void Format(const GenericStringView<SrcChar>& string, FormatContext<DstChar>& context);
	private:
		uintMem convertedLength;
	};
	template<typename SrcChar, typename DstChar> requires Formattable<GenericStringView<SrcChar>, DstChar>
	class Formatter<GenericString<SrcChar>, DstChar> : Formatter<GenericStringView<SrcChar>, DstChar>
	{
	public:
		uintMem Parse(const GenericString<SrcChar>& string, const FormatParseContext<DstChar>& parseContext);
		void Format(const GenericString<SrcChar>& string, FormatContext<DstChar>& context);
	};

	template<typename Char>
	class BaseIntegerFormatter;

	template<typename Char> requires OneOf<Char, char, char8_t, char16_t, char32_t>
	class BaseIntegerFormatter<Char> : public BaseFormatter<Char>
	{
	public:
		uint8 sign : 2;
		uint8 type : 2;
		uint8 uppercase : 1;
		uint8 alternateValue : 1;
		uint8 base : 6;
		uint8 signAwarePadding : 1;
		char symbols[80];

		uintMem ParseFormatSpecifiers(const FormatParseContext<Char>& parseContext);
		template<IntegerType T>
		uintMem ParseValue(T value);
		void CopyFormated(FormatContext<Char>& context);

		uintMem ParseSign(GenericStringView<Char> string, const FormatParseContext<Char>& parseContext);
		uintMem ParseAlternateValue(GenericStringView<Char> string, const FormatParseContext<Char>& parseContext);
		uintMem ParseSignAwarePadding(GenericStringView<Char> string, const FormatParseContext<Char>& parseContext);
		uintMem ParseType(GenericStringView<Char> string, const FormatParseContext<Char>& parseContext);
	};

	template<IntegerType T, typename Char>
	class Formatter<T, Char> : BaseIntegerFormatter<Char>
	{
	public:
		uintMem Parse(const T& value, const FormatParseContext<Char>& parseContext)
		{
			this->ParseFormatSpecifiers(parseContext);
			return this->ParseValue(value);
		}
		void Format(const T& value, FormatContext<Char>& context)
		{
			this->CopyFormated(context);
		}
	};

	template<typename Char>
	class BaseDecimalFormatter;

	template<typename Char> requires OneOf<Char, char, char8_t, char16_t, char32_t>
	class BaseDecimalFormatter<Char> : public BaseFormatter<Char>
	{
	public:
		uint8 sign : 2;
		uint8 type : 2;
		uint8 uppercase : 1;
		uint8 alternateValue : 1;
		uint8 signAwarePadding : 1;
		uint8 precision : 4;
		uint8 errorFlag : 1;
		char symbols[80];

		uintMem ParseFormatSpecifiers(const FormatParseContext<Char>& parseContext);
		template<DecimalType T>
		uintMem ParseValue(T value);
		void CopyFormated(FormatContext<Char>& context);

		uintMem ParseSign(GenericStringView<Char> string, const FormatParseContext<Char>& parseContext);
		uintMem ParseAlternateValue(GenericStringView<Char> string, const FormatParseContext<Char>& parseContext);
		uintMem ParseSignAwarePadding(GenericStringView<Char> string, const FormatParseContext<Char>& parseContext);
		uintMem ParsePrecision(GenericStringView<Char> string, const FormatParseContext<Char>& parseContext);
		uintMem ParseType(GenericStringView<Char> string, const FormatParseContext<Char>& parseContext);
		uintMem ParseError(StringView message);

		struct DecimalParts
		{
			uintMem signLength = 0;
			uintMem numberLength = 0;
			uintMem dotLength = 0;
			uintMem decimalLength = 0;
			uintMem exponentSignLength = 0;
			uintMem exponentCharacterLength = 0;
			uintMem exponentNumberLength = 0;
		};

		DecimalParts DivideSymbols(char* end);

		void AssertDotIntoNumber(DecimalParts& parts, char*& end);
		void AssertPrecisionIntoNumber(DecimalParts& parts, char*& end);

		void InsertIntoSymbols(uintMem index, StringView str, char*& end);
		void EraseFromSymbols(uintMem index, uintMem count, char*& end);
	};

	template<DecimalType T, typename Char>
	class Formatter<T, Char> : BaseDecimalFormatter<Char>
	{
	public:
		uintMem Parse(const T& value, const FormatParseContext<Char>& parseContext)

		{
			this->ParseFormatSpecifiers(parseContext);
			return this->ParseValue(value);
		}
		void Format(const T& value, FormatContext<Char>& context)
		{
			this->CopyFormated(context);
		}
	};

	template<typename T, typename Char>
	class Formatter<T*, Char> : BaseIntegerFormatter<Char>
	{
	public:
		uintMem Parse(T* const & value, const FormatParseContext<Char>& parseContext)
		{
			this->ParseFormatSpecifiers(parseContext);
			this->base = 16;
			this->sign = 0;
			this->type = 0;
			this->signAwarePadding = 0;
			return this->ParseValue(reinterpret_cast<uintMem>(value));
		}
		void Format(T* const & value, FormatContext<Char>& context)
		{
			this->CopyFormated(context);
		}
	};
}

#include "BlazeEngine/Core/Common/FormatImpl.h"