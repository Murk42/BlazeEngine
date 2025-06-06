#pragma once
#include "BlazeEngineCore/DataStructures/Buffer.h"
#include "BlazeEngineCore/DataStructures/Map.h"
#include "BlazeEngineCore/File/Path.h"
#include "BlazeEngine/Resources/Bitmap/Bitmap.h"
#include <functional>

namespace Blaze
{
	class FontGlyphRenderer;
	class CharacterSet;

	class BLAZE_API Font
	{
	public:		
		using CreateAtlasCallback = std::function<void(Vec2u)>;
		using CopyBitmapCallback = std::function<void(UnicodeChar, Vec2u, BitmapView)>;

		//Glyph metric data expressed in EMs (relative unit)
		//Multiple each number by the point size of the font to get its final point size (absolute unit)
		//Each point is equal to 0.352778mm so use the display parameters to find the wanted pixel size.
		struct GlyphMetrics
		{
			float horizontalAdvance;
			float verticalAdvance;
			Vec2f offset;
			Vec2f size;
		};		

		Font();
		Font(Font&& other) noexcept;		
		Font(const Path& path);
		~Font();

		void Clear();		

		void Load(const Path& path);
		void Load(ReadStream& readStream);

		Vec2f GetGlyphKerning(uint fontPixelHeight, UnicodeChar left, UnicodeChar right) const;
		bool GetGlyphMetrics(uint fontPixelHeight, UnicodeChar character, GlyphMetrics& data) const;

		Map<UnicodeChar, Bitmap> CreateGlyphBitmaps(uint fontPixelHeight, const CharacterSet& characterSet, FontGlyphRenderer& glyphRenderer) const;
		bool CreateAtlas(uint fontPixelHeight, const CharacterSet& characterSet, FontGlyphRenderer& glyphRenderer, const CreateAtlasCallback& createAtlasCallback, const CopyBitmapCallback& copyBitmapCallback) const;
		
		inline void* GetHandle() const { return ptr; }

		Font& operator=(Font&& other) noexcept;		

		static Font LoadDefault();
	private:
		Buffer memory;
		void* ptr;
	};
}	