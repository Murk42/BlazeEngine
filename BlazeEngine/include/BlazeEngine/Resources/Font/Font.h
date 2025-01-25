#pragma once
#include "BlazeEngine/Resources/Bitmap/Bitmap.h"
#include "BlazeEngine/Application/ResourceSystem/Resource.h"

namespace Blaze
{		
	class CharacterSet;
	class Font;

	//Glyph metric data expressed in EMs (relative unit)
	//Multiple each number by the point size of the font to get its final point size (absolute unit)
	//Each point is equal to 0.352778mm so use the display parameters to find the wanted pixel size.
	struct FontGlyphMetrics
	{
		float horizontalAdvance;
		float verticalAdvance;
		Vec2f offset;
		Vec2f size;
	};	

	struct CharacterSpan
	{
		UnicodeChar first;
		UnicodeChar last;
	};


	class BLAZE_API CharacterSetIterator
	{
	public:
		CharacterSetIterator();
		CharacterSetIterator(const CharacterSetIterator& other);
		~CharacterSetIterator();

		bool IsNull() const;

		CharacterSetIterator& operator++();
		CharacterSetIterator operator++(int);
		CharacterSetIterator& operator--();
		CharacterSetIterator operator--(int);

		UnicodeChar operator*() const;		

		bool operator==(CharacterSetIterator&) const;
		bool operator!=(CharacterSetIterator&) const;

		CharacterSetIterator& operator=(const CharacterSetIterator& other);
	private:
		CharacterSetIterator(const CharacterSet* characterSet, uintMem spanIndex, uintMem characterIndex);

		const CharacterSet* characterSet;
		uintMem spanIndex;
		uintMem characterIndex;

		friend class CharacterSet;
	};	

	class BLAZE_API CharacterSet
	{
	public:		
		using Iterator = CharacterSetIterator;

		CharacterSet();
		CharacterSet(CharacterSet&& other) noexcept;
		CharacterSet(Array<CharacterSpan> spans);

		static CharacterSet ASCIICharacterSet();

		uintMem Count() const;

		CharacterSetIterator FirstIterator() const;
		CharacterSetIterator BehindIterator() const;

		CharacterSet& operator=(CharacterSet&& other) noexcept;
	private:
		Array<CharacterSpan> spans;
		uintMem count;

		friend class CharacterSetIterator;
	};	

	CharacterSetIterator begin(const CharacterSet& characterSet);
	CharacterSetIterator end(const CharacterSet& characterSet);

	class BLAZE_API FontGlyphRenderer
	{
	public:
		using CopyBitmapCallback = std::function<void(UnicodeChar, BitmapView)>;

		virtual ~FontGlyphRenderer() { }

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

	class BLAZE_API FontMetrics
	{
	public:
		using CreateAtlasCallback = std::function<void(Vec2u)>;
		using CopyBitmapCallback = std::function<void(UnicodeChar, Vec2u, BitmapView)>;

		Vec2f GetGlyphKerning(UnicodeChar left, UnicodeChar right) const;
		bool GetGlyphMetrics(UnicodeChar character, FontGlyphMetrics& data) const;

		inline uint GetPixelFontHeight() const { return pixelFontHeight; }
		inline Font* GetFont() const { return font; }

		Map<UnicodeChar, Bitmap> GetGlyphBitmaps(const CharacterSet& characterSet, FontGlyphRenderer& glyphRenderer) const;
		void CreateAtlas(const CharacterSet& characterSet, FontGlyphRenderer& glyphRenderer, const CreateAtlasCallback& createAtlasCallback, const CopyBitmapCallback& copyBitmapCallback) const;

		FontMetrics(Font* font, uint pixelFontHeight);
	private:
		
		uint pixelFontHeight;
		mutable Map<UnicodeChar, FontGlyphMetrics> glyphMetrics;
		Font* font;

		friend class Font;
	};

	class BLAZE_API Font
	{
	public:		
		mutable VirtualMap<String> dataMap;

		Font();
		Font(Font&& other) noexcept;		
		Font(Path path);
		~Font();

		void Clear();		

		Result Load(Path path);
		Result Load(ReadStream& readStream);
		
		//The metrics are in font units
		FontMetrics* GetUnscalledMetrics() const;
		FontMetrics* GetMetrics(uint pixelFontHeight) const;		

		//How maany font units fit in one EM
		inline uint GetFontUnitScale() const;
		inline void* GetHandle() const { return ptr; }		
		//inline float GetBaselineDistance() const;

		Font& operator=(Font&& other) noexcept;		

		static Font LoadDefault();
	private:
		void* ptr;
		void* memory;
		
		mutable Map<uint, FontMetrics> metrics;		
		
		friend class FontMetrics;
	};
}	