#include "pch.h"
#include "BlazeEngine/UI/Text/TextShaping/WrappedTextShaping.h"
#include "BlazeEngine/Core/Container/ArrayView.h"
#include "BlazeEngine/Core/String/StringCharIterator.h"
#include <stdexcept>

namespace Blaze::UI::TextShaping
{
	using StringView = u8StringView;
	using StringViewIterator = u8StringViewIterator;

	WrappedTextShapingContext::WrappedTextShapingContext(StringView text, const FontFace& fontFace, uint32 fontHeight, float maxWidth, const TextSeparationDataBase& textSeparationData, ShouldIncludeAtEndFunctionType shouldIncludeAtEndFunction) :
		ShapingContext(text, fontFace, fontHeight), 
		maxWidth(maxWidth), 
		textSeparationData(textSeparationData),
		shouldIncludeAtEndFunction(shouldIncludeAtEndFunction)
	{
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
	static float width_between_glyphs(const GlyphInfo* begin, const GlyphInfo* end)
	{
		if (begin > end)
			throw std::invalid_argument("Invalid glyph iterator");

		float sumWidth = 0;
		while (begin != end)
		{
			sumWidth += begin->advance.x;
			++begin;
		}

		return sumWidth;
	}
	static float cluster_width(const GlyphInfo* glyph, const ArrayView<GlyphInfo>& glyphs)
	{
		if (glyph < glyphs.Begin() || glyph >= glyphs.End())
			throw std::exception("Invalid glyph iterator");

		float width = 0;
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

	static ShapingResult combine_shaped_texts(ShapingResult& a, const GlyphInfo* beginGlyph, const GlyphInfo* endGlyph, ShapingResult& b)
	{
		ShapingResult result;
		result.extent = a.extent + width_between_glyphs(beginGlyph, endGlyph) + b.extent;

		result.glyphs.ReserveExactly(a.glyphs.Count() + (endGlyph - beginGlyph) +b.glyphs.Count());
		result.glyphs.Append(a.glyphs);
		result.glyphs.Append({ endGlyph, beginGlyph});
		result.glyphs.Append(b.glyphs);

		return result;
	}

	
	static StringViewIterator find_end_rounded_to_text_unit(StringView text, uint32 level, const WrappedTextShapingContext& shapingContext)
	{
		StringViewIterator it = text.BehindIterator();

		while (it != text.FirstIterator())
		{
			UnicodeChar ch;
			uintMem count = ch.FromLastCodePoints(u8StringView{ text.FirstIterator(), it });

			if (count == 0)
			{
				BLAZE_LOG_FATAL("Invalid UTF-8 sequence");
				return it;
			}
			
			it -= count;

			if (shapingContext.GetTextSeparationData().GetSeparationValue(it - shapingContext.GetText().FirstIterator()) >= level)
				return it + count;
		}

		return text.FirstIterator();
	}
	static StringViewIterator find_end_rounded_to_text_unit(StringView text, const WrappedTextShapingContext& shapingContext)
	{
		StringViewIterator end = text.BehindIterator();

		uint32 separationLevel = shapingContext.GetTextSeparationData().GetMaximumWrappingSeparationValue();
		while (separationLevel != 0)
		{
			end = find_end_rounded_to_text_unit(text, separationLevel, shapingContext);

			if (end != text.FirstIterator())
				break;

			separationLevel = separationLevel - 1;
		}

		return end;
	}

	struct FirstGlyphToExceedWidthData
	{
		const GlyphInfo* glyph;
		float widthUpToGlyph;
	};
	static FirstGlyphToExceedWidthData first_glyph_to_exceed_width(const ArrayView<GlyphInfo>& lineGlyphs, float maxWidth, const WrappedTextShapingContext& shapingContext)
	{
		if (lineGlyphs.Empty())
			return { lineGlyphs.Begin(), 0 };

		float width = 0;
		const GlyphInfo* it = lineGlyphs.Begin();
		for (; it != lineGlyphs.End(); ++it)
		{
			float newWidth = width + it->advance.x;

			if (newWidth > maxWidth)
				return { it, width };

			width = newWidth;
		}

		return { lineGlyphs.End(), width };
	}
	static StringViewIterator first_character_to_pass_minimum_separation_level(StringView text, const WrappedTextShapingContext& shapingContext)
	{
		for (auto it = u8StringCharIterator::FirstIterator(text); it < u8StringCharIterator::BehindIterator(text); ++it)
		{
			uintMem globalTextIndex = it - shapingContext.GetText().FirstIterator();
			if (shapingContext.GetTextSeparationData().GetSeparationValue(globalTextIndex) > 0)
				return it;			
		}

		return text.BehindIterator();
	}
	static const GlyphInfo* find_cluster_containing_character(const ArrayView<GlyphInfo>& glyphs, StringViewIterator character)
	{
		auto it = std::lower_bound(glyphs.Begin(), glyphs.End(), character, [](const GlyphInfo& glyph, StringViewIterator character) { return glyph.textBegin < character; });

		if (it != glyphs.End())
			it = cluster_floor(it, glyphs);

		return it;
	}

	struct ShapedTailResult
	{
		ShapingResult shapingResult;

		StringView tailText;

		//This glyph is either nullptr or the start of a cluster that is safe to concat.
		//The shaped text is shaped with text starting at startGlyph->textBegin.
		const GlyphInfo* startGlyph;
		//This glyph is either nullptr or the start of a cluster that contains the end character of
		//the shaped text. If the end charecter of the <shapedText> is the end character of <text>
		//then this glyph is the end of <glyphs>.
		const GlyphInfo* clusterContainingLineEnd;
	};
	static ShapedTailResult shape_line_tail(const ArrayView<GlyphInfo>& glyphs, StringView text, float maxWidth, const WrappedTextShapingContext& shapingContext, bool mustBeConcatableAtBeginning)
	{
		auto [tailEndGlyph, widthUpToTailEnd] = first_glyph_to_exceed_width(glyphs, maxWidth, shapingContext);

		if (tailEndGlyph == glyphs.Begin() || tailEndGlyph == glyphs.End())
			return {
				.shapingResult{ .glyphs = { glyphs.Begin(), tailEndGlyph }, .extent = widthUpToTailEnd },
				.tailText = { text.FirstIterator(),  tailEndGlyph == glyphs.Begin() ? text.FirstIterator() : text.BehindIterator() },
				.startGlyph = glyphs.Begin(),
				.clusterContainingLineEnd = tailEndGlyph
		};

		auto tailEndCharacter = find_end_rounded_to_text_unit({ text.FirstIterator(), tailEndGlyph->textBegin}, shapingContext);
		auto tailStartCluster = find_cluster_containing_character(glyphs, tailEndCharacter);
		float widthUpToTailStart = widthUpToTailEnd - width_between_glyphs(tailStartCluster, tailEndGlyph);

		//Find next previous cluster safe to concat and try to form tail end
		while (tailStartCluster > glyphs.Begin())
		{
			if (!is_cluster_unsafe_to_concat(tailStartCluster, glyphs))
			{
				auto shapedTail = ShapeText({ tailStartCluster->textBegin, tailEndCharacter }, shapingContext);

				//The first glyph might not be concatable so it wouldn't satisfy conditions even if the shaped text
				//doesn't need to be concatable at the begging, so we need to add that check

				if (tailStartCluster == glyphs.Begin() && !mustBeConcatableAtBeginning || shapedTail.glyphs.Empty() || !is_cluster_unsafe_to_concat(shapedTail.glyphs.Ptr(), shapedTail.glyphs))
				{
					auto clusterContainingLineEnd = find_cluster_containing_character(glyphs, tailEndCharacter);

					return {
						.shapingResult = std::move(shapedTail),
						.startGlyph = tailStartCluster,
						.clusterContainingLineEnd = clusterContainingLineEnd
					};
				}
			}

			tailStartCluster = previous_cluster(tailStartCluster, glyphs);
			widthUpToTailStart -= cluster_width(tailStartCluster, glyphs);
		}

		return {
			.shapingResult = { },
			.startGlyph = nullptr,
			.clusterContainingLineEnd = nullptr
		};
	}

	struct ShapedHeadResult
	{
		ShapingResult shapingResult;
		StringViewIterator headEnd;

		//This glyph is either nullptr, the end of remainingGlyphs or the start of a cluster.
		//If the glyph is the start of a cluster then the preceeding cluster (if it exists) is safe to concat or the cluster is safe to break at.
		//The shaped text is shaped with text up until endGlyph->textBegin.
		const GlyphInfo* endGlyph;
	};
	static ShapedHeadResult shape_line_head(const ArrayView<GlyphInfo>& glyphs, StringView text, const WrappedTextShapingContext& shapingContext)
	{
		//If the first cluster is safe to break we dont need to shape the line head and can return an empty result
		if (glyphs.Empty() || text.Empty() || text.FirstIterator() == glyphs.First().textBegin && !is_cluster_unsafe_to_break(glyphs.Begin(), glyphs))
			return {
				
				.shapingResult = {  },
				.headEnd = text.FirstIterator(),
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
			auto endCharacter = endCluster == glyphs.End() ? text.BehindIterator() : endCluster->textBegin;

			ShapingResult shapingResult = ShapeText({ text.FirstIterator(), endCharacter }, shapingContext);

			if (shapingResult.extent > shapingContext.GetMaxWidth())
				break;

			//If the line head end glyph isn't safe to concat we search for the next concatable cluster
			if (!shapingResult.glyphs.Empty() && !is_cluster_unsafe_to_concat_reverse(shapingResult.glyphs.End(), shapingResult.glyphs))
				return {
					.shapingResult = std::move(shapingResult),
					.endGlyph = endCluster
			};

			nextConcatableClusterGlyph = endCluster;
		}

		return {
			.shapingResult = { },
			.headEnd = text.FirstIterator(),
			.endGlyph = nullptr
		};
	}

	struct WrappedLineResult
	{
		ShapingResult shapingResult;
		StringViewIterator lineEnd;

		//This glyph is either start of a cluster that contains the end character of the shaped text.
		//If the end charecter of the shaped text is the end character of remainingText
		//then this glyph is the end of the remainingGlyphs.
		const GlyphInfo* clusterContainingLineEnd = nullptr;
	};
	static bool TryReuseGlyphs(WrappedLineResult& result, const ArrayView<GlyphInfo>& glyphs, StringView text, const WrappedTextShapingContext& shapingContext)
	{
		ShapedHeadResult lineHeadResult = shape_line_head(glyphs, text, shapingContext);

		if (lineHeadResult.endGlyph == nullptr)
			return false;

		ShapedTailResult lineTailResult = shape_line_tail({ lineHeadResult.endGlyph, glyphs.End() }, { lineHeadResult.headEnd, text.BehindIterator() }, shapingContext.GetMaxWidth() - lineHeadResult.shapingResult.extent, shapingContext, !lineHeadResult.shapingResult.glyphs.Empty());

		if (lineTailResult.startGlyph == nullptr)
			return false;
				
		if (lineHeadResult.headEnd == text.FirstIterator() && lineTailResult.tailText.Empty())
			return false;

		if (lineHeadResult.headEnd == text.FirstIterator())
		{
			const bool lineTailCleanStart = shapingContext.GetTextSeparationData().GetSeparationValue(lineHeadResult.headEnd - shapingContext.GetText().FirstIterator()) == shapingContext.GetTextSeparationData().GetMaximumWrappingSeparationValue();

			if (lineTailCleanStart)
				return false;
		}

		//Combine text shaping results
		result.lineEnd = lineTailResult.tailText.BehindIterator();
		result.shapingResult = combine_shaped_texts(lineHeadResult.shapingResult, lineHeadResult.endGlyph, lineTailResult.startGlyph, lineTailResult.shapingResult);
		result.clusterContainingLineEnd = lineTailResult.clusterContainingLineEnd;
		return true;
	}

	/**/
	static WrappedLineResult WrapLineRaw(const ArrayView<GlyphInfo>& glyphs, StringView text, const WrappedTextShapingContext& shapingContext)
	{
		WrappedLineResult result;
		if (!TryReuseGlyphs(result, glyphs, text, shapingContext))
		{
			auto [firstGlyphToExceedWidth, width] = first_glyph_to_exceed_width(glyphs, shapingContext.GetMaxWidth(), shapingContext);
			auto firstCharacterToExceedWidth = firstGlyphToExceedWidth == glyphs.End() ? text.BehindIterator()  : firstGlyphToExceedWidth->textBegin;
			firstCharacterToExceedWidth = find_end_rounded_to_text_unit({ text.FirstIterator(), firstCharacterToExceedWidth }, shapingContext);

			auto shapingResult = ShapeText({ text.FirstIterator(), firstCharacterToExceedWidth } , shapingContext);

			auto clusterContainingLineEnd = find_cluster_containing_character(glyphs, firstCharacterToExceedWidth);

			result = { 
				std::move(shapingResult), 
				firstCharacterToExceedWidth,
				clusterContainingLineEnd 
			};
		}

		return result;
	}
	/*
		Wraps the line with the desired width.
	*/
	static void HandleEmptyLine(WrappedLineResult& wrappedLineResult, StringView text, const ArrayView<GlyphInfo>& glyphs, const WrappedTextShapingContext& shapingContext)
	{
		if (text.FirstIterator() == wrappedLineResult.lineEnd)
		{
			auto roundedEnd = first_character_to_pass_minimum_separation_level(text, shapingContext);

			if (roundedEnd != text.BehindIterator())
				++roundedEnd;

			wrappedLineResult.shapingResult = ShapeText({ text.FirstIterator(), roundedEnd }, shapingContext);
			wrappedLineResult.lineEnd = roundedEnd;
			wrappedLineResult.clusterContainingLineEnd = find_cluster_containing_character(glyphs, roundedEnd);
		}
	}
	static void IncludeCharactersAtEnd(WrappedLineResult& result, StringView text, const ArrayView<GlyphInfo>& glyphs, const WrappedTextShapingContext& shapingContext)
	{ 
		//Find all characters at the end of the line that can be ignored (for example white spaces)
		u8StringCharIterator ignorableCharactersBegin = { result.lineEnd, text };
		while (true)
		{
			if (ignorableCharactersBegin <= u8StringCharIterator::FirstIterator(text))
				break;

			u8StringCharIterator prev = ignorableCharactersBegin;
			--prev;

			if (!shapingContext.ShouldIncludeAtEnd(*prev))
				break;

			ignorableCharactersBegin = prev;
		}

		//Find all characters that weren't included in the line because of its extent but can be added to it and ignored
		u8StringCharIterator ignorableCharacterEnd = { result.lineEnd, text };
		while (true)
		{	
			if (ignorableCharacterEnd >= u8StringCharIterator::BehindIterator(text))
				break;

			if (!shapingContext.ShouldIncludeAtEnd(*ignorableCharacterEnd))
				break;		

			++ignorableCharacterEnd;
		}

		if (ignorableCharacterEnd == ignorableCharactersBegin)
			return;

		auto ignorableGlyphBegin = find_cluster_containing_character(result.shapingResult.glyphs, ignorableCharactersBegin);		
		auto newClusterContainingLineEnd = find_cluster_containing_character(glyphs, ignorableCharacterEnd);

		result.shapingResult.extent -= width_between_glyphs(ignorableGlyphBegin, result.shapingResult.glyphs.End());
		result.shapingResult.glyphs.Append({ result.clusterContainingLineEnd, newClusterContainingLineEnd });
		result.lineEnd = ignorableCharacterEnd;
		result.clusterContainingLineEnd = newClusterContainingLineEnd;
	}
	/*
		Wraps the line with the desired width. If the line is empty some characters are added. Includes empty characters at end
	*/
	static WrappedLineResult WrapLine(const ArrayView<GlyphInfo>& glyphs, StringView text, const WrappedTextShapingContext& shapingContext)
	{
		WrappedLineResult result = WrapLineRaw(glyphs, text, shapingContext);

		HandleEmptyLine(result, text, glyphs, shapingContext);

		IncludeCharactersAtEnd(result, text, glyphs, shapingContext);

		return result;
	}

	static void DivideLineIntoWrappedLines(Array<ShapingResult>& wrappedLines, StringView text, const WrappedTextShapingContext& lineShapingContext)
	{
		ShapingResult shapedLine = ShapeText(text, lineShapingContext);

		//First character in next line
		StringViewIterator lineBegin = text.FirstIterator();
		//First glyph in the first WHOLE cluster in the next line
		const auto* clusterContainingStartCharacter = shapedLine.glyphs.Ptr();


		while (lineBegin != text.BehindIterator())
		{
			const WrappedLineResult result = WrapLine({ clusterContainingStartCharacter, shapedLine.glyphs.End() }, { lineBegin, text.BehindIterator() }, lineShapingContext);

			wrappedLines.AddBack(result.shapingResult);

			lineBegin = result.lineEnd;
			clusterContainingStartCharacter = result.clusterContainingLineEnd;
		}
	}

	static StringView next_line(StringViewIterator it, const StringView& text, const WrappedTextShapingContext& context)
	{
		if (it < text.FirstIterator() || it > text.BehindIterator())
			throw std::out_of_range("iterator out of range");		

		auto begin = it;

		while (it < text.BehindIterator() && !context.GetTextSeparationData().IsNewLine(it - context.GetText().FirstIterator()))
			++it;

		if (it != text.BehindIterator())
			++it;

		return { begin, it };
	}
	static StringView first_line(const StringView& text, const WrappedTextShapingContext& context)
	{
		return next_line(text.FirstIterator(), text, context);
	}

	Array<ShapingResult> ShapeTextWrapped(StringView text, const WrappedTextShapingContext& shapingContext)
	{
		if (text.Empty())
			return {};

		Array<ShapingResult> wrappedLines;
		for (auto lineText = first_line(text, shapingContext); !lineText.Empty(); lineText = next_line(lineText.BehindIterator(), text, shapingContext))
			DivideLineIntoWrappedLines(wrappedLines, lineText, shapingContext);

		if (text.Last() == '\n')
			wrappedLines.AddBack();

		return wrappedLines;
	}
}