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
		struct LineData
		{
			uintMem firstCharacter;
			Vec2f lineStart;
		};
		struct CharacterData
		{
			uintMem lineIndex;
			Vec2f cursorPos;
			Vec2f topRight;
			Vec2f bottomLeft;
		};
		struct GlyphData
		{
			uintMem characterIndex;
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

		Vec2f GetCursorPosBeforeCharacter(uintMem characterIndex) const;
		Vec2f GetCursorPosAfterCharacter(uintMem characterIndex) const;
		
		uintMem GetCharacterLineIndex(uintMem characterIndex) const;
		uintMem GetLineFirstCharacter(uintMem lineIndex) const;

		inline uintMem GetLineCount() const { return lineData.Count(); }

		//inline ArrayView<GlyphData> GetGlyphData() const { return glyphData; }
		//inline ArrayView<CharacterData> GetCharacterData() const { return characterData; }
		//inline ArrayView<LineData> GetLineData() const { return lineData; }
	private:
		Array<GlyphData> glyphData;
		Array<CharacterData> characterData;
		Array<LineData> lineData;

		void UpdateLineLayout(ArrayView<ShapedString> shapedLines, const Nodes::TextStyle& style);
		void UpdateCharacterLayout(ArrayView<ShapedString> shapedLines, const Nodes::TextStyle& style);
		void UpdateGlyphLayout(ArrayView<ShapedString> shapedLines, const Nodes::TextStyle& style);
	};
}