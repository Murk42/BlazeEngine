#include "pch.h"
#include "BlazeEngineCore/Debug/Logger.h"
#include "BlazeEngine/Resources/Font/FontFace.h"
#include "BlazeEngineGraphics/Text/Core/FontAtlas.h"
#include "BlazeEngineGraphics/Text/Core/TextStyle.h"
#include "BlazeEngineGraphics/Text/Core/FontGlyphRenderer.h"
#include "BlazeEngineGraphics/Core/OpenGL/OpenGLWrapper/Textures/OpenGLTexture2D.h"

#include <freetype/freetype.h>
#include "rectpack2D.h"


namespace Blaze
{
	using CreateAtlasCallback = std::function<void(Vec2u)>;
	using CopyBitmapCallback = std::function<void(uint32, Vec2u, BitmapView)>;

	static bool CreateAtlas(const FontFace& face, float fontPixelHeight, ArrayView<uint32> glyphIndicies, FontGlyphRenderer& glyphRenderer, const CreateAtlasCallback& createAtlasCallback, const CopyBitmapCallback& copyBitmapCallback)
	{						
		constexpr int glyphBitmapExtraPadding = 1;
		
		std::vector<rect_type> glyphAtlasRects;
		glyphAtlasRects.reserve(glyphIndicies.Count());

		struct GlyphData
		{
			Bitmap bitmap;
			uint32 glyphIndex;
		};
		Array<GlyphData> glyphs;
		glyphs.ReserveExactly(glyphIndicies.Count());

		auto glyphAtlasRectIt = glyphAtlasRects.begin();
		for (auto glyphIndex : glyphIndicies)
		{
			Bitmap bitmap = glyphRenderer.Render(face, fontPixelHeight, glyphIndex);

			if (bitmap.Empty())
				continue;

			Vec2u size = bitmap.GetSize() + Vec2u(glyphBitmapExtraPadding * 2);

			glyphAtlasRects.push_back(rect_type(0, 0, size.x, size.y));
			glyphs.AddBack(GlyphData{ std::move(bitmap), glyphIndex });
		}

		const auto result_size = PackRects(glyphAtlasRects);

		createAtlasCallback(Vec2u(result_size.w, result_size.h));

		uintMem i = 0;
		for (auto& glyphs : glyphs)
		{
			Vec2u offset = Vec2u(glyphAtlasRects[i].x, glyphAtlasRects[i].y) + Vec2u(glyphBitmapExtraPadding);

			copyBitmapCallback(glyphs.glyphIndex, offset, glyphs.bitmap);

			++i;
		}

		return true;
	}

	FontAtlasIdentifier::FontAtlasIdentifier(const FontAtlasIdentifier& other)
		: fontFace(other.fontFace), fontSize(other.fontSize)
	{
	}
	FontAtlasIdentifier::FontAtlasIdentifier(const TextStyle& style)
		: fontFace(&style.fontFace.Get()), fontSize(style.fontSize)
	{
	}
	FontAtlasIdentifier::FontAtlasIdentifier(const FontFace& fontFace, float fontSize)
		: fontFace(&fontFace), fontSize(fontSize)
	{
	}
	uint32 FontAtlasIdentifier::Hash() const
	{
		return Blaze::Hash<float>().Compute(fontSize) ^ Blaze::Hash<const FontFace*>().Compute(fontFace);
	}
	bool FontAtlasIdentifier::operator==(const FontAtlasIdentifier& other) const
	{
		return fontSize == other.fontSize && fontFace == other.fontFace;
	}
	FontAtlasIdentifier& FontAtlasIdentifier::operator=(const FontAtlasIdentifier& other)
	{
		fontSize = other.fontSize;
		fontFace = other.fontFace;

		return *this;
	}

	FontAtlas::FontAtlas()
	{
	}
	FontAtlas::FontAtlas(FontAtlas&& other) noexcept
		: texture(std::move(other.texture)), uvs(std::move(other.uvs)), size(other.size)
	{
		other.size = {};
	}
	FontAtlas::FontAtlas(const FontFace& fontFace, float fontPixelHeight, ArrayView<uint32> glyphIndicies, FontGlyphRenderer& glyphRenderer, ResourceManager& resourceManager)
	{
		using namespace Graphics::OpenGLWrapper;		

		ResourceRef<Texture2D> texture;
		auto CreateAtlasCallback = [&](Vec2u size) {
			texture = resourceManager.LoadResource<Texture2D>("");
			texture->Create(size, TextureInternalPixelFormat::R8, { .min = TextureSampling::Linear, .mag = TextureSampling::Linear, .mip = TextureSampling::Nearest, .textureLevelCount = 1 });
			texture->SetSwizzle(TextureSwizzle::One, TextureSwizzle::One, TextureSwizzle::One, TextureSwizzle::R);			

			this->size = Vec2<uint16>(size);
			};
		
		auto CopyBitmapCallback = [&](uint32 glyphIndex, Vec2u offset, BitmapView bm) {
			Vec2u16 uv1 = Vec2u16(offset);
			Vec2u16 uv2 = Vec2u16(offset + bm.GetSize());

			texture->SetPixels(offset, bm, 0);
			uvs.Insert(glyphIndex, FontAtlas::UVRect({ uv1, uv2 }));
			};
				
		if (!CreateAtlas(fontFace, fontPixelHeight, glyphIndicies, glyphRenderer, CreateAtlasCallback, CopyBitmapCallback))
			BLAZE_ENGINE_GRAPHICS_WARNING("Failed to create font atlas");

		this->texture = texture;
	}
	FontAtlas::FontAtlas(const TextStyle& style, ArrayView<uint32> glyphIndicies, FontGlyphRenderer& glyphRenderer, ResourceManager& resourceManager)
		: FontAtlas(style.fontFace.Get(), style.fontSize, glyphIndicies, glyphRenderer, resourceManager)
	{
	}
	bool FontAtlas::GetGlyphUV(uint32 glypIndex, Vec2u16& uv1, Vec2u16& uv2) const
	{		
		auto it = uvs.Find(glypIndex);
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
		size = std::move(other.size);

		other.size = {};

		return *this;
	}	
}