#pragma once
#include <BlazeEngine/Core/Math/Vector.h>
#include <BlazeEngine/Core/Container/Array.h>
#include <BlazeEngine/Core/Container/ArrayView.h>
#include <BlazeEngine/UI/Text/TextShaper.h>
#include <BlazeEngine/UI/Text/TextStyle.h>

namespace Blaze::UI::Text
{
	class TextLayoutMetadata
	{
	public:
		struct LineLayoutData
		{
			uintMem firstCharacter;
			Vec2f lineStart;
			float width;
		};
		struct CharacterLayoutData
		{
			uintMem lineIndex;
			Vec2f cursorPos;			
		};
		struct GlyphLayoutData
		{
			uintMem characterIndex;
			uint32 glyphIndex;
			Vec2f offset;
			Vec2f size;
		};

		void Clear();

		void UpdateLayoutData(ArrayView<ShapedString> shapedLines, const Nodes::TextStyle& style);

		/*
			\param pos     position in text space
			\returns       Index to the hit character
		*/
		uintMem HitTestCharacters(Vec2f pos) const;
		/*
			\param pos     position in text space
			\param line    line whose characters to hit test
			\returns       Index to the hit character
		*/
		uintMem HitTestLineCharacters(Vec2f pos, uintMem line) const;		

		LineLayoutData GetLineData(uintMem lineIndex) const;
		CharacterLayoutData GetCharacterData(uintMem characterIndex) const;
		GlyphLayoutData GetGlyphData(uintMem glyphIndex) const;

		Vec2f GetCursorPosBeforeCharacter(uintMem characterIndex) const;
		Vec2f GetCursorPosAfterCharacter(uintMem characterIndex) const;

		inline uintMem GetLineCount() const { return lineData.Count(); }
		inline uintMem GetGlyphCount() const { return glyphData.Count(); }
	private:
		Array<GlyphLayoutData> glyphData;
		Array<CharacterLayoutData> characterData;
		Array<LineLayoutData> lineData;

		void UpdateLineLayout(ArrayView<ShapedString> shapedLines, const Nodes::TextStyle& style);
		void UpdateCharacterLayout(ArrayView<ShapedString> shapedLines, const Nodes::TextStyle& style);
		void UpdateGlyphLayout(ArrayView<ShapedString> shapedLines, const Nodes::TextStyle& style);
	};
}