#pragma once
#include "BlazeEngine/Core/String/String.h"
#include "BlazeEngine/Core/Math/Vector.h"
#include "BlazeEngine/Core/Container/Array.h"
#include "BlazeEngine/UI/Text/FontFace.h"

namespace Blaze::UI::TextShaping
{
	struct GlyphInfo 
	{
		u8StringViewIterator textBegin;
		u8StringViewIterator textEnd;		
		uint32 glyphIndex;
		Vec2f advance;
		Vec2f offset;

		bool unsafeToBreak : 1;
		bool unsafeToConcat : 1;
	};

	class ShapingContext
	{
	public:
		ShapingContext(u8StringView text, const FontFace& fontFace, uint32 fontHeight);
		~ShapingContext();

		inline u8StringView GetText() const { return text; }
		inline const FontFace& GetFontFace() const { return fontFace; }
		inline uint32 GetFontHeight() const { return fontHeight; }
		inline void* GetFontHandle() const { return fontHandle; }
	private:
		u8StringView text;
		const FontFace& fontFace;
		uint32 fontHeight;
		void* fontHandle;
	};
	
	struct ShapingResult
	{
		Array<GlyphInfo> glyphs;
		float extent = 0;
	};

	BLAZE_API ShapingResult ShapeText(u8StringView text, const ShapingContext& shapingContext);
}