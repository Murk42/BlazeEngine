#pragma once
#include "BlazeEngine/Core/String/UnicodeChar.h"
#include "BlazeEngine/Common/Bitmap.h"

namespace Blaze::UI
{
	class FontFace;

	class BLAZE_API FontGlyphRasterizer
	{
	public:
		virtual ~FontGlyphRasterizer() {}

		virtual Bitmap Render(const FontFace& fontFace, float rasterFontHeight, uint32 glyphIndex) const = 0;
	};

	namespace FontGlyphRasterizers
	{
		class BLAZE_API MonochromeFontGlyphRasterizer : public FontGlyphRasterizer
		{
		public:
			Bitmap Render(const FontFace& fontFace, float rasterFontHeight, uint32 glyphIndex) const override;
		};

		class BLAZE_API AntialiasedFontGlyphRasterizer : public FontGlyphRasterizer
		{
		public:
			Bitmap Render(const FontFace& fontFace, float rasterFontHeight, uint32 glyphIndex) const override;
		};

		class BLAZE_API HorizontalLCDFontGlyphRasterizer : public FontGlyphRasterizer
		{
		public:
			Bitmap Render(const FontFace& fontFace, float rasterFontHeight, uint32 glyphIndex) const override;
		};

		class BLAZE_API VerticalLCDFontGlyphRasterizer : public FontGlyphRasterizer
		{
		public:
			Bitmap Render(const FontFace& fontFace, float rasterFontHeight, uint32 glyphIndex) const override;
		};

		class BLAZE_API SDFFontGlyphRasterizer : public FontGlyphRasterizer
		{
		public:
			Bitmap Render(const FontFace& fontFace, float rasterFontHeight, uint32 glyphIndex) const override;
		};
	}
}