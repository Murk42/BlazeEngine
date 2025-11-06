#pragma once
#include "BlazeEngine/Core/String/String.h"
#include "BlazeEngine/Core/Math/Vector.h"
#include "BlazeEngine/Core/Container/Array.h"
#include "BlazeEngine/UI/Text/FontFace.h"

namespace Blaze::UI::TextShaping
{
	struct GlyphInfo {
		u32StringViewIterator textBegin;
		u32StringViewIterator textEnd;
		uint32 glyphIndex;
		Vec2i32 advance;
		Vec2i32 offset;

		bool unsafeToBreak : 1;
		bool unsafeToConcat : 1;
	};

	struct ShapedText
	{
		u32StringViewIterator textBegin;
		u32StringViewIterator textEnd;
		Array<GlyphInfo> glyphs;
		uint32 extent = 0;
	};

	class ShapingContext
	{
	public:
		ShapingContext(u32StringView text, const FontFace& fontFace);
		~ShapingContext();

		inline u32StringView GetString() const { return text; }
		inline const FontFace& GetFontFace() const { return fontFace; }
		inline void* GetFontHandle() const { return fontHandle; }
	private:
		u32StringView text;
		const FontFace& fontFace;
		void* fontHandle;
	};

	BLAZE_API ShapedText ShapeText(u32StringViewIterator begin, u32StringViewIterator end, const ShapingContext& shapingContext);
}