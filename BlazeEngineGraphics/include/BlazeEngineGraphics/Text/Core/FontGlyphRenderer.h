#pragma once
#include "BlazeEngineCore/BlazeEngineCoreDefines.h"
#include "BlazeEngineCore/DataStructures/UnicodeChar.h"
#include "BlazeEngine/Resources/Bitmap/Bitmap.h"
#include <functional>

namespace Blaze
{
	class FontFace;	

	class BLAZE_API FontGlyphRenderer
	{
	public:		
		virtual ~FontGlyphRenderer() {}

		virtual Bitmap Render(const FontFace& fontFace, float fontPixelHeight, uint32 glyphIndex) = 0;
	};

	namespace FontGlyphRenderers
	{
		class BLAZE_API MonochromeFontGlyphRenderer : public FontGlyphRenderer
		{
		public:
			Bitmap Render(const FontFace& fontFace, float fontPixelHeight, uint32 glyphIndex) override;
		};

		class BLAZE_API AntialiasedFontGlyphRenderer : public FontGlyphRenderer
		{
		public:
			Bitmap Render(const FontFace& fontFace, float fontPixelHeight, uint32 glyphIndex) override;
		};

		class BLAZE_API HorizontalLCDFontGlyphRenderer : public FontGlyphRenderer
		{
		public:
			Bitmap Render(const FontFace& fontFace, float fontPixelHeight, uint32 glyphIndex) override;
		};

		class BLAZE_API VerticalLCDFontGlyphRenderer : public FontGlyphRenderer
		{
		public:
			Bitmap Render(const FontFace& fontFace, float fontPixelHeight, uint32 glyphIndex) override;
		};

		class BLAZE_API SDFFontGlyphRenderer : public FontGlyphRenderer
		{
		public:
			Bitmap Render(const FontFace& fontFace, float fontPixelHeight, uint32 glyphIndex) override;
		};
	}
}