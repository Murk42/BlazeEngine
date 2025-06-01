#pragma once
#include "BlazeEngineCore/BlazeEngineCoreDefines.h"
#include "BlazeEngineCore/DataStructures/UnicodeChar.h"
#include "BlazeEngine/Resources/Bitmap/Bitmap.h"
#include <functional>

namespace Blaze
{
	class Font;

	class BLAZE_API FontGlyphRenderer
	{
	public:
		using CopyBitmapCallback = std::function<void(UnicodeChar, BitmapView)>;

		virtual ~FontGlyphRenderer() {}

		virtual Bitmap Render(const Font& font, uint fontPixelHeight, UnicodeChar character) = 0;
	};

	namespace FontGlyphRenderers
	{
		class BLAZE_API MonochromeFontGlyphRenderer : public FontGlyphRenderer
		{
		public:
			Bitmap Render(const Font& font, uint fontHeight, UnicodeChar character) override;
		};

		class BLAZE_API AntialiasedFontGlyphRenderer : public FontGlyphRenderer
		{
		public:
			Bitmap Render(const Font& font, uint fontHeight, UnicodeChar character) override;
		};

		class BLAZE_API HorizontalLCDFontGlyphRenderer : public FontGlyphRenderer
		{
		public:
			Bitmap Render(const Font& font, uint fontHeight, UnicodeChar character) override;
		};

		class BLAZE_API VerticalLCDFontGlyphRenderer : public FontGlyphRenderer
		{
		public:
			Bitmap Render(const Font& font, uint fontHeight, UnicodeChar character) override;
		};

		class BLAZE_API SDFFontGlyphRenderer : public FontGlyphRenderer
		{
		public:
			Bitmap Render(const Font& font, uint fontHeight, UnicodeChar character) override;
		};

		/*
		class BLAZE_API MSDFFontGlyphRenderer : public FontGlyphRenderer
		{
		public:
			Bitmap Render(const Font& font, uint fontHeight, UnicodeChar character) override;
		};
		*/
	}
}