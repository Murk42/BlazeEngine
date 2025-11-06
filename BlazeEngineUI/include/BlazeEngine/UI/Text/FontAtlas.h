#pragma once
#include "BlazeEngine/Core/Math/Vector.h"
#include "BlazeEngine/Core/Resource/ResourceManager.h"
#include "BlazeEngine/Core/Container/Map.h"
#include "BlazeEngine/UI/Text/TextStyle.h"

namespace Blaze::UI
{
	class FontFace;
	class FontGlyphRenderer;

	class BLAZE_API FontAtlasIdentifier
	{
	public:
		FontAtlasIdentifier(const FontAtlasIdentifier&);
		FontAtlasIdentifier(const FontFace& fontFace, float fontSize);

		uint64 Hash() const;

		inline const FontFace& GetFontFace() const { return *fontFace; }
		inline float GetFontSize() const { return fontSize; }

		bool operator==(const FontAtlasIdentifier& other) const;

		FontAtlasIdentifier& operator=(const FontAtlasIdentifier&);
	private:
		const FontFace* fontFace;
		float fontSize;
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
		FontAtlas(const FontFace& fontFace, float fontPixelHeight, ArrayView<uint32> glyphIndices, const FontGlyphRenderer& glyphRenderer, ResourceManager& resourceManager);

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
		inline const FontFace* GetFontFace() const { return fontFace; }
		inline float GetFontPixelHeight() const { return fontPixelHeight; }

		FontAtlas& operator=(FontAtlas&& other) noexcept;
	private:
		const FontFace* fontFace;
		float fontPixelHeight;
		ResourceBaseRef texture;
		Vec2u16 size;
		Map<uint32, UVRect> uvs;
	};
}