#include "pch.h"
#include <BlazeEngine/Core/String/StringCharIterator.h>
#include <BlazeEngine/UI/Text/WrappedLineTextShaper.h>

namespace Blaze::UI::Text
{
	static const ShapedGlyph* next_cluster(const ShapedGlyph* glyph, const ArrayView<ShapedGlyph>& glyphs)
	{
		if (glyph < glyphs.Begin() || glyph >= glyphs.End())
			throw std::exception("Invalid glyph iterator");

		const auto originalCluster = glyph->textBegin;
		while (glyph != glyphs.End() && glyph->textBegin == originalCluster)
			++glyph;

		return glyph;
	}
	static const ShapedGlyph* previous_cluster(const ShapedGlyph* glyph, const ArrayView<ShapedGlyph>& glyphs)
	{
		if (glyph <= glyphs.Begin() || glyph > glyphs.End())
			throw std::exception("Invalid glyph iterator");

		const auto originalCluster = (glyph - 1)->textBegin;
		while (glyph != glyphs.Begin() && (glyph - 1)->textBegin == originalCluster)
			--glyph;

		return glyph;
	}

	static const ShapedGlyph* cluster_ceil(const ShapedGlyph* glyph, const ArrayView<ShapedGlyph>& glyphs)
	{
		if (glyph == glyphs.Begin())
			return glyph;

		while (glyph != glyphs.End() && glyph->textBegin == (glyph - 1)->textBegin)
			++glyph;

		return glyph;
	}
	static const ShapedGlyph* cluster_floor(const ShapedGlyph* glyph, const ArrayView<ShapedGlyph>& glyphs)
	{
		if (glyph >= glyphs.End())
			throw std::invalid_argument("Invalid glyph iterator");

		while (glyph != glyphs.Begin() && glyph->textBegin == (glyph - 1)->textBegin)
			--glyph;

		return glyph;
	}
	static float width_between_glyphs(const ShapedGlyph* begin, const ShapedGlyph* end)
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
	static float cluster_width(const ShapedGlyph* glyph, const ArrayView<ShapedGlyph>& glyphs)
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

	static bool is_cluster_unsafe_to_break(const ShapedGlyph* glyph, const ArrayView<ShapedGlyph> glyphs)
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
	static bool is_cluster_unsafe_to_concat_reverse(const ShapedGlyph* glyph, const ArrayView<ShapedGlyph> glyphs)
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
	static bool is_cluster_unsafe_to_concat(const ShapedGlyph* glyph, const ArrayView<ShapedGlyph> glyphs)
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

	static void find_next_cluster_safe_to_concat(const ShapedGlyph*& glyph, const ArrayView<ShapedGlyph> glyphs)
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
	static void find_previous_cluster_safe_to_concat(const ShapedGlyph*& glyph, const ArrayView<ShapedGlyph> glyphs)
	{
		while (glyph > glyphs.Begin())
		{
			if (!is_cluster_unsafe_to_concat(glyph, glyphs))
				break;

			glyph = previous_cluster(glyph, glyphs);
		}
	}
	const ShapedGlyph* WrappedLineTextShaper::FindClusterContainingCharacter(const ArrayView<ShapedGlyph>& glyphs, u8StringViewIterator character)
	{
		auto it = std::lower_bound(glyphs.Begin(), glyphs.End(), character - GetTextContext().FirstIterator(), [](const ShapedGlyph& glyph, uintMem index) { return glyph.textBegin < index; });

		if (it != glyphs.End())
			it = cluster_floor(it, glyphs);

		return it;
	}

	static ShapedString combine_shaped_texts(ShapedString& a, const ShapedGlyph* beginGlyph, const ShapedGlyph* endGlyph, ShapedString& b)
	{
		ShapedString result;
		result.extent = a.extent + width_between_glyphs(beginGlyph, endGlyph) + b.extent;

		result.glyphs.ReserveExactly(a.glyphs.Count() + (endGlyph - beginGlyph) + b.glyphs.Count());
		result.glyphs.Append(a.glyphs);
		result.glyphs.Append({ beginGlyph, endGlyph });
		result.glyphs.Append(b.glyphs);

		return result;
	}	

	u8StringViewIterator WrappedLineTextShaper::FindEndRoundedToTextUnit(u8StringView text, uint32 level)
	{
		u8StringViewIterator it = text.BehindIterator();

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

			if (textSeparationData.GetSeparationValue(it - GetTextContext().FirstIterator()) >= level)
				return it + count;
		}

		return text.FirstIterator();
	}
	u8StringViewIterator WrappedLineTextShaper::FindEndRoundedToTextUnit(u8StringView text)
	{
		u8StringViewIterator end = text.BehindIterator();

		uint32 separationLevel = textSeparationData.GetMaximumWrappingSeparationValue();
		while (separationLevel != 0)
		{
			end = FindEndRoundedToTextUnit(text, separationLevel);

			if (end != text.FirstIterator())
				break;

			separationLevel = separationLevel - 1;
		}

		return end;
	}

	WrappedLineTextShaper::FirstGlyphToExceedWidthData WrappedLineTextShaper::FirstGlyphToExceedWidth(const ArrayView<ShapedGlyph>& lineGlyphs, float maxWidth)
	{
		if (lineGlyphs.Empty())
			return { lineGlyphs.Begin(), 0 };

		float width = 0;
		const ShapedGlyph* it = lineGlyphs.Begin();
		for (; it != lineGlyphs.End(); ++it)
		{
			float newWidth = width + it->advance.x;

			if (newWidth > maxWidth)
				return { it, width };

			width = newWidth;
		}

		return { lineGlyphs.End(), width };
	}
	u8StringViewIterator WrappedLineTextShaper::FirstCharacterToPassMinimumSeparationLevel(u8StringView text)
	{
		for (auto it = u8StringCharIterator::FirstIterator(text); it < u8StringCharIterator::BehindIterator(text); ++it)
		{
			uintMem globalTextIndex = it - GetTextContext().FirstIterator();
			if (textSeparationData.GetSeparationValue(globalTextIndex) > 0)
				return it;
		}

		return text.BehindIterator();
	}

	WrappedLineTextShaper::ShapedTailResult WrappedLineTextShaper::ShapeLineTail(const ArrayView<ShapedGlyph>& glyphs, u8StringView text, float maxWidth, bool mustBeConcatableAtBeginning)
	{
		auto [tailEndGlyph, widthUpToTailEnd] = FirstGlyphToExceedWidth(glyphs, maxWidth);

		if (tailEndGlyph == glyphs.Begin() || tailEndGlyph == glyphs.End())
			return {
				.shapingResult{.glyphs = { glyphs.Begin(), tailEndGlyph }, .extent = widthUpToTailEnd },
				.tailText = { text.FirstIterator(),  tailEndGlyph == glyphs.Begin() ? text.FirstIterator() : text.BehindIterator() },
				.startGlyph = glyphs.Begin(),
				.clusterContainingLineEnd = tailEndGlyph
		};

		auto tailEndCharacter = FindEndRoundedToTextUnit({ text.FirstIterator(), GetTextContext().FirstIterator() + tailEndGlyph->textBegin });
		auto tailStartCluster = FindClusterContainingCharacter(glyphs, tailEndCharacter);
		float widthUpToTailStart = widthUpToTailEnd - width_between_glyphs(tailStartCluster, tailEndGlyph);

		//Find next previous cluster safe to concat and try to form tail end
		while (tailStartCluster > glyphs.Begin())
		{
			if (!is_cluster_unsafe_to_concat(tailStartCluster, glyphs))
			{
				auto shapedTail = ShapeSingleLine({ GetTextContext().FirstIterator() + tailStartCluster->textBegin, tailEndCharacter });

				//The first glyph might not be concatable so it wouldn't satisfy conditions even if the shaped text
				//doesn't need to be concatable at the begging, so we need to add that check

				if (tailStartCluster == glyphs.Begin() && !mustBeConcatableAtBeginning || shapedTail.glyphs.Empty() || !is_cluster_unsafe_to_concat(shapedTail.glyphs.Ptr(), shapedTail.glyphs))
				{
					auto clusterContainingLineEnd = FindClusterContainingCharacter(glyphs, tailEndCharacter);

					return {
						.shapingResult = std::move(shapedTail),
						.tailText = { GetTextContext().FirstIterator() + tailStartCluster->textBegin, tailEndCharacter},
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
			.tailText = { GetTextContext().BehindIterator(), GetTextContext().BehindIterator() },
			.startGlyph = nullptr,
			.clusterContainingLineEnd = nullptr
		};
	}

	WrappedLineTextShaper::ShapedHeadResult WrappedLineTextShaper::ShapeLineHead(const ArrayView<ShapedGlyph>& glyphs, u8StringView text)
	{
		//If the first cluster is safe to break we dont need to shape the line head and can return an empty result
		if (glyphs.Empty() || text.Empty() || glyphs.First().textBegin == 0 && !is_cluster_unsafe_to_break(glyphs.Begin(), glyphs))
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
			auto endCharacter = endCluster == glyphs.End() ? text.BehindIterator() : GetTextContext().FirstIterator() + endCluster->textBegin;

			ShapedString shapingResult = ShapeSingleLine({ text.FirstIterator(), endCharacter });

			if (shapingResult.extent > maxWidth)
				break;

			//If the line head end glyph isn't safe to concat we search for the next concatable cluster
			if (!shapingResult.glyphs.Empty() && !is_cluster_unsafe_to_concat_reverse(shapingResult.glyphs.End(), shapingResult.glyphs))
				return {
					.shapingResult = std::move(shapingResult),
					.headEnd = endCharacter,
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

	bool WrappedLineTextShaper::TryReuseGlyphs(WrappedLineResult& result, const ArrayView<ShapedGlyph>& glyphs, u8StringView text)
	{
		ShapedHeadResult lineHeadResult = ShapeLineHead(glyphs, text);

		if (lineHeadResult.endGlyph == nullptr)
			return false;

		ShapedTailResult lineTailResult = ShapeLineTail({ lineHeadResult.endGlyph, glyphs.End() }, { lineHeadResult.headEnd, text.BehindIterator() }, maxWidth - lineHeadResult.shapingResult.extent, !lineHeadResult.shapingResult.glyphs.Empty());

		if (lineTailResult.startGlyph == nullptr)
			return false;

		if (lineHeadResult.headEnd == text.FirstIterator() && lineTailResult.tailText.Empty())
			return false;

		if (lineHeadResult.headEnd == text.FirstIterator())
		{
			const bool lineTailCleanStart = textSeparationData.GetSeparationValue(lineHeadResult.headEnd - GetTextContext().FirstIterator()) == textSeparationData.GetMaximumWrappingSeparationValue();

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
	WrappedLineTextShaper::WrappedLineResult WrappedLineTextShaper::WrapLineRaw(const ArrayView<ShapedGlyph>& glyphs, u8StringView text)
	{
		WrappedLineResult result;
		if (!TryReuseGlyphs(result, glyphs, text))
		{
			auto [firstGlyphToExceedWidth, width] = FirstGlyphToExceedWidth(glyphs, maxWidth);
			auto firstCharacterToExceedWidth = firstGlyphToExceedWidth == glyphs.End() ? text.BehindIterator() : GetTextContext().FirstIterator() + firstGlyphToExceedWidth->textBegin;
			firstCharacterToExceedWidth = FindEndRoundedToTextUnit({ text.FirstIterator(), firstCharacterToExceedWidth });

			auto shapingResult = ShapeSingleLine({ text.FirstIterator(), firstCharacterToExceedWidth });

			auto clusterContainingLineEnd = FindClusterContainingCharacter(glyphs, firstCharacterToExceedWidth);

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
	void WrappedLineTextShaper::HandleEmptyLine(WrappedLineResult& wrappedLineResult, u8StringView text, const ArrayView<ShapedGlyph>& glyphs)
	{
		if (text.FirstIterator() == wrappedLineResult.lineEnd)
		{
			auto roundedEnd = FirstCharacterToPassMinimumSeparationLevel(text);

			if (roundedEnd != text.BehindIterator())
				++roundedEnd;

			wrappedLineResult.shapingResult = ShapeSingleLine({ text.FirstIterator(), roundedEnd });
			wrappedLineResult.lineEnd = roundedEnd;
			wrappedLineResult.clusterContainingLineEnd = FindClusterContainingCharacter(glyphs, roundedEnd);
		}
	}
	void WrappedLineTextShaper::IncludeCharactersAtEnd(WrappedLineResult& result, u8StringView text, const ArrayView<ShapedGlyph>& glyphs)
	{
		//Find all characters at the end of the line that can be ignored (for example white spaces)
		u8StringCharIterator ignorableCharactersBegin = { result.lineEnd, text };
		while (true)
		{
			if (ignorableCharactersBegin <= u8StringCharIterator::FirstIterator(text))
				break;

			u8StringCharIterator prev = ignorableCharactersBegin;
			--prev;

			if (!ShouldIncludeAtEnd(*prev))
				break;

			ignorableCharactersBegin = prev;
		}

		//Find all characters that weren't included in the line because of its extent but can be added to it and ignored
		u8StringCharIterator ignorableCharacterEnd = { result.lineEnd, text };
		while (true)
		{
			if (ignorableCharacterEnd >= u8StringCharIterator::BehindIterator(text))
				break;

			if (!ShouldIncludeAtEnd(*ignorableCharacterEnd))
				break;

			++ignorableCharacterEnd;
		}

		if (ignorableCharacterEnd == ignorableCharactersBegin)
			return;

		auto ignorableGlyphBegin = FindClusterContainingCharacter(result.shapingResult.glyphs, ignorableCharactersBegin);
		auto newClusterContainingLineEnd = FindClusterContainingCharacter(glyphs, ignorableCharacterEnd);

		result.shapingResult.extent -= width_between_glyphs(ignorableGlyphBegin, result.shapingResult.glyphs.End());
		result.shapingResult.glyphs.Append({ result.clusterContainingLineEnd, newClusterContainingLineEnd });
		result.lineEnd = ignorableCharacterEnd;
		result.clusterContainingLineEnd = newClusterContainingLineEnd;
	}
	/*
		Wraps the line with the desired width. If the line is empty some characters are added. Includes empty characters at end
	*/
	WrappedLineTextShaper::WrappedLineResult WrappedLineTextShaper::WrapLine(const ArrayView<ShapedGlyph>& glyphs, u8StringView text)
	{
		WrappedLineResult result = WrapLineRaw(glyphs, text);

		HandleEmptyLine(result, text, glyphs);

		IncludeCharactersAtEnd(result, text, glyphs);

		return result;
	}

	void WrappedLineTextShaper::DivideLineIntoWrappedLines(Array<ShapedString>& wrappedLines, u8StringView text)
	{
		ShapedString shapedLine = ShapeSingleLine(text);

		//First character in next line
		u8StringViewIterator lineBegin = text.FirstIterator();
		//First glyph in the first WHOLE cluster in the next line
		const auto* clusterContainingStartCharacter = shapedLine.glyphs.Ptr();


		while (lineBegin != text.BehindIterator())
		{
			const WrappedLineResult result = WrapLine({ clusterContainingStartCharacter, shapedLine.glyphs.End() }, { lineBegin, text.BehindIterator() });

			wrappedLines.AddBack(result.shapingResult);

			lineBegin = result.lineEnd;
			clusterContainingStartCharacter = result.clusterContainingLineEnd;
		}
	}

	u8StringView WrappedLineTextShaper::NextLine(u8StringViewIterator it, const u8StringView& text)
	{
		if (it < text.FirstIterator() || it > text.BehindIterator())
			throw std::out_of_range("iterator out of range");

		auto begin = it;

		while (it < text.BehindIterator() && !textSeparationData.IsNewLine(it - GetTextContext().FirstIterator()))
			++it;

		if (it != text.BehindIterator())
			++it;

		return { begin, it };
	}
	u8StringView WrappedLineTextShaper::FirstLine(const u8StringView& text)
	{
		return NextLine(text.FirstIterator(), text);
	}

	WrappedLineTextShaper::WrappedLineTextShaper(u8StringView textContext, const FontFace& fontFace, uint32 fontHeight, const TextSeparationDataBase& textSeparationData, float maxWidth)
		: SingleLineTextShaper(textContext, fontFace, fontHeight), textSeparationData(textSeparationData), maxWidth(maxWidth)
	{
	}
	WrappedLineTextShaper::~WrappedLineTextShaper()
	{
	}
	Array<ShapedString> WrappedLineTextShaper::Shape(u8StringView text)
	{
		if (text.Empty())
			return {};

		Array<ShapedString> wrappedLines;
		for (auto lineText = FirstLine(text); !lineText.Empty(); lineText = NextLine(lineText.BehindIterator(), text))
			DivideLineIntoWrappedLines(wrappedLines, lineText);

		if (text.Last() == '\n')
			wrappedLines.AddBack();

		return wrappedLines;
	}
	bool WrappedLineTextShaper::ShouldIncludeAtEnd(UnicodeChar character)
	{
		switch (character.Value())
		{
		case ' ':
		case '\t':
			return true;
		default:
			return false;
		}
	}
}