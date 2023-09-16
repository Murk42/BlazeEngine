#pragma once
#include "BlazeEngine/DataStructures/StringViewUTF8.h"
#include "BlazeEngine/DataStructures/Buffer.h"
#include "BlazeEngine/DataStructures/Rect.h"
#include "BlazeEngine/DataStructures/VirtualMap.h"
#include "BlazeEngine/Resources/Bitmap/Bitmap.h"
#include "BlazeEngine/Application/ResourceSystem/Resource.h"
#include "BlazeEngine/File/Stream/Stream.h"

namespace Blaze
{
	class Font;
	class FontResolution;

	struct FontResolutionGlyphData
	{
		float horizontalAdvance;
		float verticalAdvance;
		Vec2f offset;
		Vec2f size;
	};

	class FontGlyphRenderer
	{
	public:
		using CopyBitmapCallback = std::function<void(UnicodeChar, BitmapView)>;

		virtual ~FontGlyphRenderer() { }

		virtual Bitmap Render(const Font& font, uint fontHeight, UnicodeChar character) = 0;
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

	class BLAZE_API CharacterSet
	{
	public:
		struct CharacterSpan
		{
			UnicodeChar first;
			UnicodeChar last;
		};

		Array<CharacterSpan> spans;

		CharacterSet();
		CharacterSet(CharacterSet&& other) noexcept;
		CharacterSet(Array<CharacterSpan> spans);

		static CharacterSet ASCIICharacterSet();

		CharacterSet& operator=(CharacterSet&& other) noexcept;
	};

	class BLAZE_API FontGlyphBitmaps
	{
	public:
		using CreateAtlasCallback = std::function<void(Vec2i)>;
		using CopyBitmapCallback = std::function<void(UnicodeChar, Vec2i, BitmapView)>;

		struct GlyphData
		{
			Bitmap bitmap;
			UnicodeChar character;
		};

		FontGlyphBitmaps();
		FontGlyphBitmaps(const Font& font, const FontResolution& fontResolution, FontGlyphRenderer& fontGlyphRenderer);
		FontGlyphBitmaps(FontGlyphBitmaps&& other) noexcept;
		~FontGlyphBitmaps();

		void CreateAtlas(const CreateAtlasCallback& createAtlas, const CopyBitmapCallback& copyBitmap);

		void Clear();

		inline ArrayView<GlyphData> GetGlyphsData() { return glyphsData; }

		FontGlyphBitmaps& operator=(FontGlyphBitmaps&& other) noexcept;
	private:

		Array<GlyphData> glyphsData;

		friend class FontResolution;
	};

	class BLAZE_API FontResolution
	{
	public:		
		mutable VirtualMap<String> dataMap;

		FontResolution();
		FontResolution(const Font& font, uint fontHeight, const CharacterSet& characterSet);
		FontResolution(FontResolution&& other) noexcept;
		~FontResolution();

		bool GetGlyphData(UnicodeChar ch, FontResolutionGlyphData& data) const;

		void Clear();
		
		inline uint GetFontHeight() const { return fontHeight; }
		inline uint GetBaselineDistance() const { return baselineDistance; }
		inline uint GetNoneEmptyGlyphCount() const { return nonEmptyCount; }
		inline const Map<UnicodeChar, FontResolutionGlyphData>& GetGlyphsData() const { return glyphsData; }

		FontResolution& operator=(FontResolution && other) noexcept;		
	private:		
		Map<UnicodeChar, FontResolutionGlyphData> glyphsData;
		uint nonEmptyCount;

		uint fontHeight;
		uint baselineDistance;
	};

	class BLAZE_API Font : public ResourceSystem::Resource
	{
	public:
		using CreateAtlasCallback = std::function<void(Vec2i)>;
		using CopyBitmapCallback = std::function<void(UnicodeChar, Vec2i, BitmapView)>;

		Font();
		Font(Font&& other) noexcept;		
		Font(Path path);
		~Font();

		/// <summary>					Load a font file. </summary>
		/// <param name='path'>			- Path to the file </param>
		/// <param name='characters'>	- All characters to be loaded. If empty loads all characters from the file </param>		
		Result Load(Path path);
		Result Load(ReadStream& readStream);
		Vec2f GetKerning(const FontResolution& fontResolution, UnicodeChar left, UnicodeChar right) const;

		void Clear();		

		inline void* GetHandle() const { return ptr; }

		Font& operator=(Font&& other) noexcept;

		friend class FontResolution;
		RESOURCE(Font)
	private:
		void* ptr;
		void* memory;

		List<FontResolution> resolutions;

		uint pixelsPerUnit; //How many pixels fit in one font unit
		float baselineDistance;		
	};
}	