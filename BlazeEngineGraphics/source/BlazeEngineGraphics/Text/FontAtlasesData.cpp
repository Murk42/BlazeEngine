#include "pch.h"
#include "BlazeEngineGraphics/Text/FontAtlasesData.h"

namespace Blaze::Graphics
{	
	FontAtlasesData::FontAtlasesData(Font* font)
		: font(font)
	{
	}
	AtlasData* FontAtlasesData::GetAtlasData(uint fontHeight)
	{
		AtlasData* bestBellow = nullptr;
		uint fontHeightBellow = 0;
		AtlasData* bestAbove = nullptr;
		uint fontHeightAbove = UINT_MAX;

		for (auto& data : atlasData)
		{
			if (data.fontHeight == fontHeight)
				return &data;

			if (data.fontHeight > fontHeight && data.fontHeight < fontHeightAbove)
			{
				bestAbove = &data;
				fontHeightAbove = data.fontHeight;
			}

			if (data.fontHeight < fontHeight && data.fontHeight > fontHeightBellow)
			{
				bestBellow = &data;
				fontHeightBellow = data.fontHeight;
			}

		}

		if (bestAbove == nullptr)
			return bestBellow;
		return bestAbove;
	}
	void FontAtlasesData::AddAtlasData(const Array<uint>& fontHeights, const CharacterSet& characterSet)
	{
		for (auto fontHeight : fontHeights)
		{
			auto it = atlasData.FirstIterator();
			for (; it != atlasData.BehindIterator(); ++it)
				if (it->fontHeight == fontHeight)
					break;

			if (it == atlasData.BehindIterator())
				it = atlasData.AddBack();

			auto& data = *it;
			data.fontHeight = fontHeight;

			// doesn't work because of alpha channel: #define HORIZONTAL_LCD

#ifdef HORIZONTAL_LCD
			FontGlyphRenderers::HorizontalLCDFontGlyphRenderer glyphRenderer;
#else
			FontGlyphRenderers::AntialiasedFontGlyphRenderer glyphRenderer;
#endif
			auto metrics = font->GetMetrics(fontHeight);

			Vec2f atlasSize;
			metrics.CreateAtlas(characterSet, glyphRenderer, [&](auto size) {
#ifdef HORIZONTAL_LCD
				data.atlasData.atlas.Create(size, OpenGLWrapper::TextureInternalPixelFormat::RGB8);
#else
				data.atlas.Create(size, OpenGLWrapper::TextureInternalPixelFormat::R8, {
					.min = OpenGLWrapper::TextureSampling::Linear,
					.mag = OpenGLWrapper::TextureSampling::Linear,
					.mip = OpenGLWrapper::TextureSampling::Linear,
					.textureLevelCount = 1
					});
#endif
				atlasSize = Vec2f(size);
				}, [&](UnicodeChar character, Vec2u offset, BitmapView bm) {
					Vec2f uv1 = Vec2f(offset);
					Vec2f uv2 = Vec2f(offset + bm.GetSize());
					uv1 /= atlasSize;
					uv2 /= atlasSize;

					std::swap(uv1.y, uv2.y);

					data.atlas.SetPixels(offset, bm, 0);
					data.uvs.Insert(character, AtlasData::UVRect({ uv1, uv2 }));
					});
#ifdef HORIZONTAL_LCD
				data.atlasData.atlas.SetSwizzle(
					OpenGLWrapper::TextureSwizzle::R,
					OpenGLWrapper::TextureSwizzle::G,
					OpenGLWrapper::TextureSwizzle::B,
					OpenGLWrapper::TextureSwizzle::O);
#else
				data.atlas.SetSwizzle(
					OpenGLWrapper::TextureSwizzle::One,
					OpenGLWrapper::TextureSwizzle::One,
					OpenGLWrapper::TextureSwizzle::One,
					OpenGLWrapper::TextureSwizzle::R);
#endif
		}
	}
	FontAtlasesData& FontAtlasesData::AddToFont(Font& font, const Array<uint>& fontHeights, const CharacterSet& characterSet)
	{
		auto it = font.dataMap.Find("FontAtlasesData");
		FontAtlasesData* ptr = it.GetValue<FontAtlasesData>();

		if (ptr == nullptr)
		{
			it = font.dataMap.Insert<FontAtlasesData, true>("FontAtlasesData", &font).iterator;
			ptr = it.GetValue<FontAtlasesData>();
		}

		ptr->AddAtlasData(fontHeights, characterSet);

		return *ptr;
	}

	FontAtlasesData* FontAtlasesData::RetrieveFromFont(Font& font)
	{
		auto it = font.dataMap.Find("FontAtlasesData");
		FontAtlasesData* ptr = it.GetValue<FontAtlasesData>();

		if (ptr == nullptr)
			return nullptr;				

		return ptr;
	}

}