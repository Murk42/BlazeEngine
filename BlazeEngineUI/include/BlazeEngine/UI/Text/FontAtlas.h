#pragma once
#include "BlazeEngine/Core/Math/Vector.h"
#include "BlazeEngine/Core/Resource/ResourceManager.h"
#include "BlazeEngine/Core/Container/Map.h"
#include "BlazeEngine/Graphics/Core/GraphicsContextBase.h"
#include "BlazeEngine/UI/Text/TextStyle.h"

namespace Blaze::UI
{
	class FontFace;
	class FontGlyphRasterizer;

	struct FontAtlasDescriptor
	{
		const FontFace& fontFace;
		float rasterFontHeight;
		ArrayView<uint32> glyphIndices;
		const FontGlyphRasterizer& glyphRasterizer;
	};

	class BLAZE_API FontAtlas
	{
	public:
		struct UVRect
		{
			Vec2u16 uv1;
			Vec2u16 uv2;
		};

		FontAtlas();
		FontAtlas(FontAtlas&& other) noexcept;
		FontAtlas(const FontAtlasDescriptor& descriptor, Graphics::GraphicsContextBase& graphicsContext, ResourceManager& resourceManager);
		~FontAtlas();

		void Clear();
		void Create(const FontAtlasDescriptor& descriptor, Graphics::GraphicsContextBase& graphicsContext, ResourceManager& resourceManager);

		/*
			Retrieves glyphs UV coordinates in the font atlas

			\param glyphIndex   the glyphs index whose UV coordinates to return
			\param uv1         this parameter will be filled wit the lower left point of the character rectangle
			in the font atlas
			\param uv2         this parameter will be filled wit the top right point of the character rectangle
			in the font atlas
			\returns True if there is the requested character in the atlas, false otherwise
		*/
		bool GetGlyphUV(uint32 glyphIndex, Vec2u16& uv1, Vec2u16& uv2) const;
		inline Vec2u16 GetSize() const { return size; }
		inline ResourceBaseRef GetTexture() const { return texture; }
		inline float GetRasterFontHeight() const { return rasterFontHeight; }		

		FontAtlas& operator=(FontAtlas&& other) noexcept;
	private:
		Vec2u16 size;
		float rasterFontHeight;
		ResourceBaseRef texture;
		Map<uint32, UVRect> uvs;
	};
}