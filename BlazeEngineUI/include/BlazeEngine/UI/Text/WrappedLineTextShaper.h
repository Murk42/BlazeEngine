#pragma once
#include <BlazeEngine/UI/Text/SingleLineTextShaper.h>
#include <BlazeEngine/UI/Text/TextSeparations.h>

namespace Blaze::UI::Text
{
	class WrappedLineTextShaper : public SingleLineTextShaper
	{
	public:
		WrappedLineTextShaper(u8StringView textContext, const FontFace& fontFace, uint32 fontHeight, const TextSeparationDataBase& textSeparationData, float maxWidth);
		~WrappedLineTextShaper() override;

		virtual Array<ShapedString> Shape(u8StringView text) override;
	protected:
		virtual bool ShouldIncludeAtEnd(UnicodeChar character);
	private:
		struct FirstGlyphToExceedWidthData
		{
			const ShapedGlyph* glyph;
			float widthUpToGlyph;
		};
		struct ShapedTailResult
		{
			ShapedString shapingResult;

			u8StringView tailText;

			//This glyph is either nullptr or the start of a cluster that is safe to concat.
			//The shaped text is shaped with text starting at startGlyph->textBegin.
			const ShapedGlyph* startGlyph;
			//This glyph is either nullptr or the start of a cluster that contains the end character of
			//the shaped text. If the end charecter of the <shapedText> is the end character of <text>
			//then this glyph is the end of <glyphs>.
			const ShapedGlyph* clusterContainingLineEnd;
		};
		struct ShapedHeadResult
		{
			ShapedString shapingResult;
			u8StringViewIterator headEnd;

			//This glyph is either nullptr, the end of remainingGlyphs or the start of a cluster.
			//If the glyph is the start of a cluster then the preceeding cluster (if it exists) is safe to concat or the cluster is safe to break at.
			//The shaped text is shaped with text up until endGlyph->textBegin.
			const ShapedGlyph* endGlyph;
		};
		struct WrappedLineResult
		{
			ShapedString shapingResult;
			u8StringViewIterator lineEnd;

			//This glyph is either start of a cluster that contains the end character of the shaped text.
			//If the end charecter of the shaped text is the end character of remainingText
			//then this glyph is the end of the remainingGlyphs.
			const ShapedGlyph* clusterContainingLineEnd = nullptr;
		};

		const TextSeparationDataBase& textSeparationData;
		float maxWidth;

		const ShapedGlyph* FindClusterContainingCharacter(const ArrayView<ShapedGlyph>& glyphs, u8StringViewIterator character);

		u8StringViewIterator FindEndRoundedToTextUnit(u8StringView text, uint32 level);
		u8StringViewIterator FindEndRoundedToTextUnit(u8StringView text);

		FirstGlyphToExceedWidthData FirstGlyphToExceedWidth(const ArrayView<ShapedGlyph>& lineGlyphs, float maxWidth);
		u8StringViewIterator FirstCharacterToPassMinimumSeparationLevel(u8StringView text);

		ShapedTailResult ShapeLineTail(const ArrayView<ShapedGlyph>& glyphs, u8StringView text, float maxWidth, bool mustBeConcatableAtBeginning);
		ShapedHeadResult ShapeLineHead(const ArrayView<ShapedGlyph>& glyphs, u8StringView text);

		bool TryReuseGlyphs(WrappedLineResult& result, const ArrayView<ShapedGlyph>& glyphs, u8StringView text);
		WrappedLineResult WrapLineRaw(const ArrayView<ShapedGlyph>& glyphs, u8StringView text);

		void HandleEmptyLine(WrappedLineResult& wrappedLineResult, u8StringView text, const ArrayView<ShapedGlyph>& glyphs);
		void IncludeCharactersAtEnd(WrappedLineResult& result, u8StringView text, const ArrayView<ShapedGlyph>& glyphs);

		WrappedLineResult WrapLine(const ArrayView<ShapedGlyph>& glyphs, u8StringView text);

		void DivideLineIntoWrappedLines(Array<ShapedString>& wrappedLines, u8StringView text);

		u8StringView NextLine(u8StringViewIterator it, const u8StringView& text);
		u8StringView FirstLine(const u8StringView& text);
	};
}