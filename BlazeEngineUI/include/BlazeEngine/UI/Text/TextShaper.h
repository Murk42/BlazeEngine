#pragma once
#include "BlazeEngine/Core/Math/Vector.h"
#include "BlazeEngine/Core/String/String.h"
#include "BlazeEngine/Core/Container/Array.h"

namespace Blaze::UI::Text
{
	struct ShapedGlyph
	{
		uintMem textBegin;
		uintMem textEnd;
		uint32 glyphIndex;
		Vec2f advance;
		Vec2f offset;

		bool unsafeToBreak : 1;
		bool unsafeToConcat : 1;
	};

	struct ShapedString
	{
		Array<ShapedGlyph> glyphs;
		float extent;
	};

	class TextShaper
	{
	public:
		virtual ~TextShaper() { }
		virtual Array<ShapedString> Shape(u8StringView text) = 0;
	private:
	};
}