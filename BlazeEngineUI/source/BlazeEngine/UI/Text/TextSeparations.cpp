#include "pch.h"
#include "BlazeEngine/UI/Text/TextSeparations.h"
#include "BlazeEngine/Core/String/StringCharIterator.h"
#include <utf8proc.h>
#include <wordbreak.h>
#include <linebreak.h>
#include <harfbuzz/hb.h>

namespace Blaze::UI
{
	static Array<char> get_text_wordbreaks(const u8StringView& string, const char* language)
	{
		static struct Initializer
		{
			Initializer()
			{
				init_wordbreak();
			}
		} initializer;

		if (string.Empty())
			return { };

		Array<char> breaks(string.Count());
		set_wordbreaks_utf8(reinterpret_cast<const utf8_t*>(string.Ptr()), string.Count(), language, breaks.Ptr());
		return breaks;
	}
	static Array<char> get_text_linebreaks(const u8StringView& string, const char* language)
	{
		static struct Initializer
		{
			Initializer()
			{
				init_linebreak();
			}
		} initializer;

		if (string.Empty())
			return { };

		Array<char> breaks(string.Count());

		if constexpr (SameAs<RemoveCVRef<decltype(string)>, u8StringView>)
			set_linebreaks_utf8(reinterpret_cast<const utf8_t*>(string.Ptr()), string.Count(), language, breaks.Ptr());
		else if constexpr (SameAs<RemoveCVRef<decltype(string)>, u32StringView>)
			set_linebreaks_utf32(reinterpret_cast<const utf32_t*>(string.Ptr()), string.Count(), language, breaks.Ptr());
		else
		{
			BLAZE_LOG_FATAL("String type not implemented");
		}

		return breaks;
	}		
	DefaultTextSeparationData::DefaultTextSeparationData(u8StringView text)
	{
		SetText(text);
	}
	uint32 DefaultTextSeparationData::GetMaximumWrappingSeparationValue() const
	{
		return TextSeparation::Word;
	}
	void DefaultTextSeparationData::SetText(u8StringView text)
	{
		if (text.Empty())
			return;

		const char* language = hb_language_to_string(hb_language_get_default());
		auto wordBreaks = get_text_wordbreaks(text, language);
		auto lineBreaks = get_text_linebreaks(text, language);

		separations.Resize(text.Count());

		u8StringCharIterator it{ text.FirstIterator(), text };

		UnicodeChar prev = *it;

		utf8proc_int32_t state = 0;
		for (; it != text.BehindIterator(); ++it)
		{
			uintMem i = it.Ptr() - text.Ptr();
			UnicodeChar curr = *it;

			if (utf8proc_grapheme_break_stateful(prev.Value(), curr.Value(), &state))
			{
				if (lineBreaks[i] == LINEBREAK_MUSTBREAK)
					separations[i] = TextSeparation::NewLine;
				else if (wordBreaks[i] == WORDBREAK_BREAK || IsAdditionalWordCharacter(curr))
					separations[i] = TextSeparation::Word;
				else
				{
					separations[i] = TextSeparation::Grapheme;
					state = 0;

					if (IsWordSuffixCharacter(curr) && i != 0 && separations[i - 1] == TextSeparation::Word)
					{
						separations[i] = TextSeparation::Word;
						separations[i - 1] = TextSeparation::Grapheme;
					}
				}
			}
			else
				separations[i] = TextSeparation::Codepoint;

			//if (IsWordSuffixCharacter(curr))
			//	separations[i] = TextSeparation::Word;

			prev = curr;
		}
	}
	uint32 DefaultTextSeparationData::GetSeparationValue(uintMem index) const
	{
		return static_cast<uint32>(separations[index]);
	}
	bool DefaultTextSeparationData::IsNewLine(uintMem index) const
	{
		return separations[index] == NewLine;
	}
	uintMem DefaultTextSeparationData::Count() const
	{
		return separations.Count();
	}
	bool DefaultTextSeparationData::IsAdditionalWordCharacter(UnicodeChar character)
	{
		return false;
	}
	bool DefaultTextSeparationData::IsWordSuffixCharacter(UnicodeChar character)
	{
		switch (character.Value())
		{
		case U'.':
		case U',':
		case U'!':
		case U'?':
		case U':':
		case U';':
			return true;
		default:
			return false;
		}
	}
}