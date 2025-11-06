#pragma once
#include "BlazeEngine/Core/String/UnicodeChar.h"
#include "BlazeEngine/Common/Bitmap.h"

namespace Blaze::UI
{
	class FontFace;

	class BLAZE_API FontGlyphRenderer
	{
	public:
		virtual ~FontGlyphRenderer() {}

		virtual Bitmap Render(const FontFace& fontFace, float fontPixelHeight, uint32 glyphIndex) const = 0;
	};

	namespace FontGlyphRenderers
	{
		class BLAZE_API MonochromeFontGlyphRenderer : public FontGlyphRenderer
		{
		public:
			Bitmap Render(const FontFace& fontFace, float fontPixelHeight, uint32 glyphIndex) const override;
		};

		class BLAZE_API AntialiasedFontGlyphRenderer : public FontGlyphRenderer
		{
		public:
			Bitmap Render(const FontFace& fontFace, float fontPixelHeight, uint32 glyphIndex) const override;
		};

		class BLAZE_API HorizontalLCDFontGlyphRenderer : public FontGlyphRenderer
		{
		public:
			Bitmap Render(const FontFace& fontFace, float fontPixelHeight, uint32 glyphIndex) const override;
		};

		class BLAZE_API VerticalLCDFontGlyphRenderer : public FontGlyphRenderer
		{
		public:
			Bitmap Render(const FontFace& fontFace, float fontPixelHeight, uint32 glyphIndex) const override;
		};

		class BLAZE_API SDFFontGlyphRenderer : public FontGlyphRenderer
		{
		public:
			Bitmap Render(const FontFace& fontFace, float fontPixelHeight, uint32 glyphIndex) const override;
		};
	}
}