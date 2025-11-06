#include "pch.h"
#include "BlazeEngine/UI/Text/TextShaping/WrappedTextShaping.h"
#include "BlazeEngine/Core/Container/ArrayView.h"
#include <stdexcept>
#include <utf8proc.h>
#include <harfbuzz/hb.h>
#include <linebreak.h>
#include <wordbreak.h>

namespace Blaze::UI::TextShaping
{
	Array<char> get_text_linebreaks(const u32StringView& string, const char* language)
	{
		static struct Initializer {
			Initializer() {
				init_linebreak();
			}
		} initializer;

		if (string.Empty())
			return { };

		Array<char> breaks(string.Count());
		set_linebreaks_utf32(reinterpret_cast<const utf32_t*>(string.Ptr()), string.Count(), language, breaks.Ptr());
		return breaks;
	}
	Array<char> get_text_wordbreaks(const u32StringView& string, const char* language)
	{
		static struct Initializer {
			Initializer() {
				init_wordbreak();
			}
		} initializer;

		if (string.Empty())
			return { };

		Array<char> breaks(string.Count());
		set_wordbreaks_utf32(reinterpret_cast<const utf32_t*>(string.Ptr()), string.Count(), language, breaks.Ptr());
		return breaks;
	}

	WrappedTextShapingContext::WrappedTextShapingContext(u32StringView text, const FontFace& fontFace, uint32 maxWidth, TextUnitSeparationLevel smallestSeparationLevel, TextUnitSeparationLevel expectedSeparationLevel, ShouldIncludeAtEndFunctionType shouldIncludeAtEndFunction)
		: ShapingContext(text, fontFace), maxWidth(maxWidth), smallestSeparationLevel(smallestSeparationLevel), expectedSeparationLevel(expectedSeparationLevel), shouldIncludeAtEndFunction(shouldIncludeAtEndFunction)
	{
	}

	TextUnitSeparationLevel TextShaping::WrappedTextShapingContext::GetTextUnitSeparationAt(u32StringViewIterator iterator) const
	{
		return textUnitSeparations[iterator - GetString().FirstIterator()];
	}

	bool WrappedTextShapingContext::ShouldIncludeAtEndDefault(UnicodeChar character, const WrappedTextShapingContext& shapingContext)
	{
		switch (character.Value())
		{
		case ' ':
			//case '.':
			//case ',':
			//case '!':
			//case '?':
			//case ':':
			//case ';':
		case '\t':
			return true;
		default:
			return false;
		}
	}

	static const GlyphInfo* next_cluster(const GlyphInfo* glyph, const ArrayView<GlyphInfo>& glyphs)
	{
		if (glyph < glyphs.Begin() || glyph >= glyphs.End())
			throw std::exception("Invalid glyph iterator");

		const auto originalCluster = glyph->textBegin;
		while (glyph != glyphs.End() && glyph->textBegin == originalCluster)
			++glyph;

		return glyph;
	}
	static const GlyphInfo* previous_cluster(const GlyphInfo* glyph, const ArrayView<GlyphInfo>& glyphs)
	{
		if (glyph <= glyphs.Begin() || glyph > glyphs.End())
			throw std::exception("Invalid glyph iterator");

		const auto originalCluster = (glyph - 1)->textBegin;
		while (glyph != glyphs.Begin() && (glyph - 1)->textBegin == originalCluster)
			--glyph;

		return glyph;
	}

	static const GlyphInfo* cluster_ceil(const GlyphInfo* glyph, const ArrayView<GlyphInfo>& glyphs)
	{
		if (glyph == glyphs.Begin())
			return glyph;

		while (glyph != glyphs.End() && glyph->textBegin == (glyph - 1)->textBegin)
			++glyph;

		return glyph;
	}
	static const GlyphInfo* cluster_floor(const GlyphInfo* glyph, const ArrayView<GlyphInfo>& glyphs)
	{
		if (glyph >= glyphs.End())
			throw std::invalid_argument("Invalid glyph iterator");

		while (glyph != glyphs.Begin() && glyph->textBegin == (glyph - 1)->textBegin)
			--glyph;

		return glyph;
	}
	static int32 width_between_glyphs(const GlyphInfo* begin, const GlyphInfo* end)
	{
		if (begin > end)
			throw std::invalid_argument("Invalid glyph iterator");
		int32 sumWidth = 0;
		while (begin != end)
		{
			sumWidth += begin->advance.x;
			++begin;
		}

		return sumWidth;
	}
	static int32 cluster_width(const GlyphInfo* glyph, const ArrayView<GlyphInfo>& glyphs)
	{
		if (glyph < glyphs.Begin() || glyph >= glyphs.End())
			throw std::exception("Invalid glyph iterator");

		int32 width = 0;
		const auto originalCluster = glyph->textBegin;

		while (glyph != glyphs.End() && glyph->textBegin == originalCluster)
		{
			width += glyph->advance.x;
			++glyph;
		}

		return width;
	}

	static bool is_cluster_unsafe_to_break(const GlyphInfo* glyph, const ArrayView<GlyphInfo> glyphs)
	{
		if (glyph < glyphs.Begin() || glyph >= glyphs.End())
			throw std::exception("Invalid glyph iterator");

		const auto originalCluster = glyph->textBegin;

		while (glyph != glyphs.End() && glyph->textBegin == originalCluster)
		{
			if (glyph->unsafeToBreak)
				return true;
			++glyph;
		}

		return false;
	}
	static bool is_cluster_unsafe_to_concat_reverse(const GlyphInfo* glyph, const ArrayView<GlyphInfo> glyphs)
	{
		if (glyph <= glyphs.Begin() || glyph > glyphs.End())
			throw std::exception("Invalid glyph iterator");

		--glyph;
		const auto originalCluster = glyph->textBegin;

		while (glyph != glyphs.Begin() && glyph->textBegin == originalCluster)
		{
			if (glyph->unsafeToConcat)
				return true;
			--glyph;
		}

		return false;
	}
	static bool is_cluster_unsafe_to_concat(const GlyphInfo* glyph, const ArrayView<GlyphInfo> glyphs)
	{
		if (glyph < glyphs.Begin() || glyph >= glyphs.End())
			throw std::exception("Invalid glyph iterator");

		const auto originalCluster = glyph->textBegin;

		bool unsafeToConcat = false;

		while (glyph != glyphs.End() && glyph->textBegin == originalCluster)
		{
			unsafeToConcat |= glyph->unsafeToConcat;
			++glyph;
		}

		return unsafeToConcat;
	}

	static void find_next_cluster_safe_to_concat(const GlyphInfo*& glyph, const ArrayView<GlyphInfo> glyphs)
	{
		if (glyph < glyphs.Begin() || glyph >= glyphs.End())
			throw std::exception("Invalid glyph iterator");

		while (glyph < glyphs.End())
		{
			if (!is_cluster_unsafe_to_concat(glyph, glyphs))
				break;

			glyph = next_cluster(glyph, glyphs);
		}
	}
	static void find_previous_cluster_safe_to_concat(const GlyphInfo*& glyph, const ArrayView<GlyphInfo> glyphs)
	{
		while (glyph > glyphs.Begin())
		{
			if (!is_cluster_unsafe_to_concat(glyph, glyphs))
				break;

			glyph = previous_cluster(glyph, glyphs);
		}
	}

	//static bool include_special_characters_at_end(u32StringViewIterator& it, const WrappedTextShapingContext& shapingContext)
	//{
	//	const u32StringViewIterator original = it;
	//	while (it != shapingContext.GetString().BehindIterator() && shapingContext.ShouldIncludeAtEnd({ it, it + 1 }))
	//		++it;
	//
	//	return original != it;
	//}
	static u32StringViewIterator find_end_rounded_to_text_unit(u32StringViewIterator textBegin, u32StringViewIterator textEnd, TextUnitSeparationLevel level, const WrappedTextShapingContext& shapingContext)
	{
		u32StringViewIterator it = textEnd;

		while (it != textBegin)
		{
			if (shapingContext.GetTextUnitSeparationAt(it) >= level)
				break;

			--it;
		}

		return it;
	}
	static u32StringViewIterator find_end_rounded_to_text_unit(u32StringViewIterator textBegin, u32StringViewIterator textEnd, const WrappedTextShapingContext& shapingContext)
	{
		u32StringViewIterator end = textEnd;

		TextUnitSeparationLevel separationLevel = shapingContext.GetExpectedSeparationLevel();
		while (separationLevel != shapingContext.GetSmallestSeparationLevel() - 1)
		{
			end = find_end_rounded_to_text_unit(textBegin, textEnd, separationLevel, shapingContext);

			if (end != textBegin)
				break;

			separationLevel = TextUnitSeparationLevel(separationLevel - 1);
		}

		return end;
	}

	struct FirstGlyphToExceedWidthData
	{
		const GlyphInfo* glyph;
		uint32 widthUpToGlyph;
	};
	static FirstGlyphToExceedWidthData first_glyph_to_exceed_width(const ArrayView<GlyphInfo>& lineGlyphs, uint32 maxWidth, const WrappedTextShapingContext& shapingContext)
	{
		if (lineGlyphs.Empty())
			return { lineGlyphs.Begin(), 0 };

		uint32 width = 0;
		const GlyphInfo* it = lineGlyphs.Begin();
		for (; it != lineGlyphs.End(); ++it)
		{
			uint32 newWidth = width + it->advance.x;

			if (newWidth > maxWidth)
				return { it, width };

			width = newWidth;
		}

		return { lineGlyphs.End(), width };
	}
	static u32StringViewIterator first_character_to_pass_minimum_separation_level(u32StringViewIterator textBegin, u32StringViewIterator textEnd, const WrappedTextShapingContext& shapingContext)
	{
		for (auto it = textBegin; it != textEnd; ++it)
			if (shapingContext.GetTextUnitSeparationAt(it) >= shapingContext.GetSmallestSeparationLevel())
				return it;

		return textEnd;
	}
	static const GlyphInfo* find_cluster_containing_character(const ArrayView<GlyphInfo>& glyphs, u32StringViewIterator character)
	{
		auto it = std::lower_bound(glyphs.Begin(), glyphs.End(), character, [](const GlyphInfo& glyph, u32StringViewIterator character) { return glyph.textBegin < character; });

		if (it != glyphs.End())
			it = cluster_floor(it, glyphs);

		return it;
	}

	struct ShapedTailResult
	{
		ShapedText shapedText;

		//This glyph is either nullptr or the start of a cluster that is safe to concat.
		//The shaped text is shaped with text starting at startGlyph->textBegin.
		const GlyphInfo* startGlyph;
		//This glyph is either nullptr or the start of a cluster that contains the end character of
		//the shaped text. If the end charecter of the <shapedText> is the end character of <text>
		//then this glyph is the end of <glyphs>.
		const GlyphInfo* clusterContainingEndCharacter;
	};
	static ShapedTailResult shape_line_tail(const ArrayView<GlyphInfo>& glyphs, u32StringViewIterator textBegin, u32StringViewIterator textEnd, uint32 maxWidth, const WrappedTextShapingContext& shapingContext, bool mustBeConcatableAtBeginning)
	{
		auto [tailEndGlyph, widthUpToTailEnd] = first_glyph_to_exceed_width(glyphs, maxWidth, shapingContext);

		if (tailEndGlyph == glyphs.Begin() || tailEndGlyph == glyphs.End())
			return {
				.shapedText = {
					.textBegin = textBegin,
					.textEnd = tailEndGlyph == glyphs.End() ? textEnd : textBegin,
					.glyphs = { glyphs.Begin(), tailEndGlyph },
					.extent = widthUpToTailEnd
				},
				.startGlyph = glyphs.Begin(),
				.clusterContainingEndCharacter = tailEndGlyph
		};

		auto tailEndCharacter = find_end_rounded_to_text_unit(textBegin, tailEndGlyph->textBegin, shapingContext);
		auto tailStartCluster = find_cluster_containing_character(glyphs, tailEndCharacter);
		uint32 widthUpToTailStart = widthUpToTailEnd - width_between_glyphs(tailStartCluster, tailEndGlyph);

		//Find next previous cluster safe to concat and try to form tail end
		while (tailStartCluster > glyphs.Begin())
		{
			if (!is_cluster_unsafe_to_concat(tailStartCluster, glyphs))
			{
				auto shapedTail = ShapeText(tailStartCluster->textBegin, tailEndCharacter, shapingContext);

				//The first glyph might not be concatable so it wouldn't satisfy conditions even if the shaped text
				//doesn't need to be concatable at the begging, so we need to add that check

				if (tailStartCluster == glyphs.Begin() && !mustBeConcatableAtBeginning || shapedTail.glyphs.Empty() || !is_cluster_unsafe_to_concat(shapedTail.glyphs.Ptr(), shapedTail.glyphs))
				{
					auto clusterContainingEndCharacter = find_cluster_containing_character(glyphs, shapedTail.textEnd);

					return {
						.shapedText = std::move(shapedTail),
						.startGlyph = tailStartCluster,
						.clusterContainingEndCharacter = clusterContainingEndCharacter
					};
				}
			}

			tailStartCluster = previous_cluster(tailStartCluster, glyphs);
			widthUpToTailStart -= cluster_width(tailStartCluster, glyphs);
		}

		return {
			.shapedText = { },
			.startGlyph = nullptr,
			.clusterContainingEndCharacter = nullptr
		};
	}

	struct ShapedHeadResult
	{
		ShapedText shapedText;
		//This glyph is either nullptr, the end of remainingGlyphs or the start of a cluster.
		//If the glyph is the start of a cluster then the preceeding cluster (if it exists) is safe to concat or the cluster is safe to break at.
		//The shaped text is shaped with text up until endGlyph->textBegin.
		const GlyphInfo* endGlyph;
	};
	static ShapedHeadResult shape_line_head(const ArrayView<GlyphInfo>& glyphs, u32StringViewIterator textBegin, u32StringViewIterator textEnd, const WrappedTextShapingContext& shapingContext)
	{
		//If the first cluster is safe to break we dont need to shape the line head and can return an empty result
		if (glyphs.Empty() || textBegin == textEnd || textBegin == glyphs.First().textBegin && !is_cluster_unsafe_to_break(glyphs.Begin(), glyphs))
			return {
				.shapedText = {.textBegin = textBegin, .textEnd = textBegin, .glyphs = {}, .extent = 0 },
				.endGlyph = glyphs.Begin()
		};

		auto nextConcatableClusterGlyph = glyphs.Begin();

		while (true)
		{
			find_next_cluster_safe_to_concat(nextConcatableClusterGlyph, glyphs);

			if (nextConcatableClusterGlyph == glyphs.End())
				break;

			//Include the concatable cluster
			auto endCluster = next_cluster(nextConcatableClusterGlyph, glyphs);
			auto endCharacter = endCluster == glyphs.End() ? textEnd : endCluster->textBegin;

			ShapedText shapedText = ShapeText(textBegin, endCharacter, shapingContext);

			if (shapedText.extent > shapingContext.GetMaxWidth())
				break;

			//If the line head end glyph isn't safe to concat we search for the next concatable cluster
			if (!shapedText.glyphs.Empty() && !is_cluster_unsafe_to_concat_reverse(shapedText.glyphs.Ptr() + shapedText.glyphs.Count(), shapedText.glyphs))
				return {
					.shapedText = std::move(shapedText),
					.endGlyph = endCluster
			};

			nextConcatableClusterGlyph = endCluster;
		}

		return {
			.shapedText = {.textBegin = textBegin, .textEnd = textBegin, .glyphs = {}, .extent = 0},
			.endGlyph = nullptr
		};
	}

	struct WrappedLineResult
	{
		ShapedText shapedText;
		//This glyph is either start of a cluster that contains the end character of the shaped text.
		//If the end charecter of the shaped text is the end character of remainingText
		//then this glyph is the end of the remainingGlyphs.
		const GlyphInfo* clusterContainingEndCharacter = nullptr;
	};
	static bool TryReuseGlyphs(WrappedLineResult& result, const ArrayView<GlyphInfo>& glyphs, u32StringViewIterator textBegin, u32StringViewIterator textEnd, const WrappedTextShapingContext& shapingContext)
	{
		ShapedHeadResult lineHeadResult = shape_line_head(glyphs, textBegin, textEnd, shapingContext);

		if (lineHeadResult.endGlyph == nullptr)
			return false;

		ShapedTailResult lineTailResult = shape_line_tail({ lineHeadResult.endGlyph, glyphs.End() }, lineHeadResult.shapedText.textEnd, textEnd, shapingContext.GetMaxWidth() - lineHeadResult.shapedText.extent, shapingContext, !lineHeadResult.shapedText.glyphs.Empty());

		if (lineTailResult.startGlyph == nullptr)
			return false;

		if (lineTailResult.shapedText.textBegin == lineTailResult.shapedText.textEnd &&
			lineHeadResult.shapedText.textBegin != lineHeadResult.shapedText.textEnd)
			if (shapingContext.GetTextUnitSeparationAt(lineHeadResult.shapedText.textEnd) != shapingContext.GetExpectedSeparationLevel())
				return false;

		result.shapedText.textBegin = lineHeadResult.shapedText.textBegin;
		result.shapedText.textEnd = lineTailResult.shapedText.textEnd;
		result.shapedText.extent = lineHeadResult.shapedText.extent + lineTailResult.shapedText.extent + width_between_glyphs(lineHeadResult.endGlyph, lineTailResult.startGlyph);
		result.clusterContainingEndCharacter = lineTailResult.clusterContainingEndCharacter;
		result.shapedText.glyphs.ReserveExactly(lineHeadResult.shapedText.glyphs.Count() + (lineTailResult.startGlyph - lineHeadResult.endGlyph) + lineTailResult.shapedText.glyphs.Count());
		result.shapedText.glyphs.Append(lineHeadResult.shapedText.glyphs);
		result.shapedText.glyphs.Append({ lineHeadResult.endGlyph, lineTailResult.startGlyph });
		result.shapedText.glyphs.Append(lineTailResult.shapedText.glyphs);
		return true;
	}

	/**/
	static WrappedLineResult WrapLineRaw(const ArrayView<GlyphInfo>& glyphs, u32StringViewIterator textBegin, u32StringViewIterator textEnd, const WrappedTextShapingContext& shapingContext)
	{
		WrappedLineResult result;
		if (!TryReuseGlyphs(result, glyphs, textBegin, textEnd, shapingContext))
		{
			auto [firstGlyphToExceedWidth, width] = first_glyph_to_exceed_width(glyphs, shapingContext.GetMaxWidth(), shapingContext);
			auto firstCharacterToExceedWidth = firstGlyphToExceedWidth == glyphs.End() ? textEnd : firstGlyphToExceedWidth->textBegin;
			firstCharacterToExceedWidth = find_end_rounded_to_text_unit(textBegin, firstCharacterToExceedWidth, shapingContext);

			auto shapedText = ShapeText(textBegin, firstCharacterToExceedWidth, shapingContext);

			auto clusterContainingEndCharacter = find_cluster_containing_character(glyphs, shapedText.textEnd);

			result = { std::move(shapedText), clusterContainingEndCharacter };
		}

		return result;
	}
	/*
		Wraps the line with the desired width.
	*/
	static void HandleEmptyLine(WrappedLineResult& wrappedLineResult, u32StringViewIterator textEnd, const ArrayView<GlyphInfo>& glyphs, const WrappedTextShapingContext& shapingContext)
	{
		if (wrappedLineResult.shapedText.textBegin == wrappedLineResult.shapedText.textEnd)
		{
			auto roundedEnd = first_character_to_pass_minimum_separation_level(wrappedLineResult.shapedText.textBegin, textEnd, shapingContext);

			if (roundedEnd != textEnd)
				++roundedEnd;

			wrappedLineResult.shapedText = ShapeText(wrappedLineResult.shapedText.textBegin, roundedEnd, shapingContext);
			wrappedLineResult.clusterContainingEndCharacter = find_cluster_containing_character(glyphs, roundedEnd);
		}
	}
	static void IncludeCharactersAtEnd(WrappedLineResult& result, u32StringViewIterator textEnd, const WrappedTextShapingContext& shapingContext)
	{
		if (result.shapedText.textBegin == result.shapedText.textEnd)
			return;

		u32StringViewIterator shouldIncludeAtEndBegin = result.shapedText.textEnd;
		for (; shouldIncludeAtEndBegin != result.shapedText.textBegin; --shouldIncludeAtEndBegin)
		{
			if (!shapingContext.ShouldIncludeAtEnd(*(shouldIncludeAtEndBegin - 1)))
				break;
		}

		u32StringViewIterator shouldIncludeAtEndEnd = result.shapedText.textEnd;
		for (; shouldIncludeAtEndEnd != textEnd; ++shouldIncludeAtEndEnd)
		{
			if (!shapingContext.ShouldIncludeAtEnd(*shouldIncludeAtEndEnd))
				break;
		}

		auto glyphBegin = find_cluster_containing_character(result.shapedText.glyphs, shouldIncludeAtEndBegin);

		result.shapedText.extent -= width_between_glyphs(glyphBegin, result.shapedText.glyphs.End());
		result.shapedText.textEnd = shouldIncludeAtEndEnd;
	}
	/*
		Wraps the line with the desired width. If the line is empty some characters are added. Includes empty characters at end
	*/
	static WrappedLineResult WrapLine(const ArrayView<GlyphInfo>& glyphs, u32StringViewIterator textBegin, u32StringViewIterator textEnd, const WrappedTextShapingContext& shapingContext)
	{
		WrappedLineResult result = WrapLineRaw(glyphs, textBegin, textEnd, shapingContext);

		HandleEmptyLine(result, textEnd, glyphs, shapingContext);

		IncludeCharactersAtEnd(result, textEnd, shapingContext);

		return result;
	}

	static void DivideLineIntoWrappedLines(Array<ShapedText>& wrappedLines, u32StringViewIterator textBegin, u32StringViewIterator textEnd, const WrappedTextShapingContext& lineShapingContext)
	{
		ShapedText shapedLine = ShapeText(textBegin, textEnd, lineShapingContext);

		if (shapedLine.glyphs.Empty())
			return;

		//First character in next line
		auto lineStart = shapedLine.textBegin;
		//First glyph in the first WHOLE cluster in the next line
		const auto* clusterContainingStartCharacter = shapedLine.glyphs.Ptr();


		while (lineStart != shapedLine.textEnd)
		{
			const WrappedLineResult result = WrapLine({ clusterContainingStartCharacter, shapedLine.glyphs.Ptr() + shapedLine.glyphs.Count() }, lineStart, textEnd, lineShapingContext);

			wrappedLines.AddBack(result.shapedText);

			lineStart = result.shapedText.textEnd;
			clusterContainingStartCharacter = result.clusterContainingEndCharacter;
		}
	}

	struct LineSpan
	{
		u32StringViewIterator begin;
		u32StringViewIterator end;
	};
	static LineSpan next_line(u32StringViewIterator begin, const Array<char>& lineBreaks, const u32StringView& text)
	{
		if (begin < text.FirstIterator() || begin > text.BehindIterator() || lineBreaks.Count() != text.Count())
			throw std::out_of_range("iterator out of range");

		uintMem endIndex = begin - text.FirstIterator();

		while (endIndex != lineBreaks.Count() && lineBreaks[endIndex] != LINEBREAK_MUSTBREAK)
			++endIndex;

		if (endIndex != lineBreaks.Count())
			++endIndex;

		return { begin, text.FirstIterator() + endIndex };
	}
	static LineSpan first_line(const Array<char>& lineBreaks, const u32StringView& text)
	{
		return next_line(text.FirstIterator(), lineBreaks, text);
	}

	static bool IsWordSuffixCharacter(char32_t character)
	{
		switch (character)
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
	static Array<TextUnitSeparationLevel> get_text_separations(const u32StringView& text)
	{
		if (text.Empty())
			return { };

		auto wordBreaks = get_text_wordbreaks(text, hb_language_to_string(hb_language_get_default()));

		Array<TextUnitSeparationLevel> out{ text.Count() };

		utf8proc_int32_t state = 0;
		for (uintMem i = 0; i < text.Count() - 1; ++i)
		{
			if (utf8proc_grapheme_break_stateful(text[i], text[i + 1], &state))
			{
				if (wordBreaks[i] == WORDBREAK_BREAK)
					out[i] = TextUnitSeparationLevel::Word;
				else
				{
					out[i] = TextUnitSeparationLevel::Grapheme;
					state = 0;
				}
			}
			else
				out[i] = TextUnitSeparationLevel::Codepoint;

			if (i != 0 && IsWordSuffixCharacter(text[i]) && !IsWordSuffixCharacter(text[i - 1]) && out[i - 1] == TextUnitSeparationLevel::Word)
			{
				out[i - 1] = TextUnitSeparationLevel::Grapheme;
				out[i] = TextUnitSeparationLevel::Word;
			}

			if (text[i] == U'\n')
				out[i] = TextUnitSeparationLevel::NewLine;

		}

		out.Last() = TextUnitSeparationLevel::Word;

		return out;
	}

	Array<ShapedText> ShapeTextWrapped(u32StringViewIterator textBegin, u32StringViewIterator textEnd, const WrappedTextShapingContext& shapingContext)
	{
		if (textBegin == textEnd)
			return {};

		const auto lineBreaks = get_text_linebreaks({ textBegin, textEnd }, hb_language_to_string(hb_language_get_default()));
		auto textUnitSeparations = get_text_separations({ textBegin, textEnd });

		Array<ShapedText> wrappedLines;
		for (auto lineText = first_line(lineBreaks, { textBegin, textEnd }); lineText.begin != lineText.end; lineText = next_line(lineText.end, lineBreaks, { textBegin, textEnd }))
			DivideLineIntoWrappedLines(wrappedLines, lineText.begin, lineText.end, shapingContext);

		return wrappedLines;
	}
}