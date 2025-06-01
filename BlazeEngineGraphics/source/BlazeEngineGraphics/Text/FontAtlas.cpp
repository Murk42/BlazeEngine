#include "pch.h"
#include "BlazeEngine/Resources/Font/FontGlyphRenderer.h"
#include "BlazeEngine/Resources/Font/CharacterSet.h"
#include "BlazeEngine/Resources/Font/Font.h"
#include "BlazeEngineGraphics/Text/FontAtlas.h"

namespace Blaze::Graphics
{
	FontAtlas::FontAtlas()
	{
	}
	FontAtlas::FontAtlas(FontAtlas&& other) noexcept
		: texture(std::move(other.texture)), uvs(std::move(other.uvs))
	{
	}
	FontAtlas::FontAtlas(Font& font, uint fontPixelHeight, const CharacterSet& characterSet)
	{
#ifdef HORIZONTAL_LCD
		FontGlyphRenderers::HorizontalLCDFontGlyphRenderer glyphRenderer;
#else
		FontGlyphRenderers::AntialiasedFontGlyphRenderer glyphRenderer;
#endif

		Vec2f atlasSize;
		auto CreateAtlas = [&](Vec2u size) {
			using namespace OpenGLWrapper;
#ifdef HORIZONTAL_LCD
			texture.Create(size, TextureInternalPixelFormat::RGB8);
			data.atlasData.atlas.SetSwizzle(TextureSwizzle::R, TextureSwizzle::G, TextureSwizzle::B, TextureSwizzle::O);
#else
			texture.Create(size, TextureInternalPixelFormat::R8, { .min = TextureSampling::Linear, .mag = TextureSampling::Linear, .mip = TextureSampling::Nearest, .textureLevelCount = 1 });
			texture.SetSwizzle(TextureSwizzle::One, TextureSwizzle::One, TextureSwizzle::One, TextureSwizzle::R);
#endif
			atlasSize = Vec2f(size);
			};
		auto CopyBitmap = [&](UnicodeChar character, Vec2u offset, BitmapView bm) {
			Vec2f uv1 = Vec2f(offset) / atlasSize;
			Vec2f uv2 = Vec2f(offset + bm.GetSize()) / atlasSize;
			
			std::swap(uv1.y, uv2.y);

			texture.SetPixels(offset, bm, 0);
			uvs.Insert(character, FontAtlas::UVRect({ uv1, uv2 }));
			};
		
		if (!font.CreateAtlas(fontPixelHeight, characterSet, glyphRenderer, CreateAtlas, CopyBitmap))		
			BLAZE_ENGINE_GRAPHICS_WARNING("Failed to create font atlas");
	}
	bool FontAtlas::GetCharacterUV(UnicodeChar character, Vec2f& uv1, Vec2f& uv2) const
	{		
		auto it = uvs.Find(character);
		if (it.IsNull())
			return false;

		uv1 = it->value.uv1;
		uv2 = it->value.uv2;

		return true;
	}

	FontAtlas& FontAtlas::operator=(FontAtlas&& other) noexcept
	{
		texture = std::move(other.texture);
		uvs = std::move(other.uvs);

		return *this;
	}
}