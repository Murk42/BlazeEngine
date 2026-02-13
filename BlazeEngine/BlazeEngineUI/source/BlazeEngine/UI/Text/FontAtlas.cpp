#include "pch.h"
#include "BlazeEngine/Core/Debug/Logger.h"
#include "BlazeEngine/UI/Text/FontFace.h"
#include "BlazeEngine/UI/Text/FontAtlas.h"
#include "BlazeEngine/UI/Text/FontGlyphRasterizer.h"
#include "BlazeEngine/Graphics/Core/OpenGL/OpenGLWrapper/Textures/OpenGLTexture2D.h"

#include "BlazeEngine/External/FreeType/FreeType.h"
#include "BlazeEngine/External/rectpack2D/rectpack2D.h"
#include <functional>


namespace Blaze::UI
{
	using CreateAtlasCallback = std::function<void(Vec2u)>;
	using CopyBitmapCallback = std::function<void(uint32, Vec2u, BitmapView)>;

	static bool CreateAtlas(const FontFace& face, float rasterFontHeight, ArrayView<uint32> glyphIndices, const FontGlyphRasterizer& glyphRasterizer, const CreateAtlasCallback& createAtlasCallback, const CopyBitmapCallback& copyBitmapCallback)
	{
		constexpr int glyphBitmapExtraPadding = 1;

		std::vector<rect_type> glyphAtlasRects;
		glyphAtlasRects.reserve(glyphIndices.Count());

		struct GlyphData
		{
			Bitmap bitmap;
			uint32 glyphIndex;
		};
		Array<GlyphData> glyphs;
		glyphs.ReserveExactly(glyphIndices.Count());

		auto glyphAtlasRectIt = glyphAtlasRects.begin();
		for (auto glyphIndex : glyphIndices)
		{
			Bitmap bitmap = glyphRasterizer.Render(face, rasterFontHeight, glyphIndex);

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

	FontAtlas::FontAtlas() : 
		rasterFontHeight(0.0f), 
		size({ })
	{
	}
	FontAtlas::FontAtlas(FontAtlas&& other) noexcept :
		texture(std::move(other.texture)),
		uvs(std::move(other.uvs)),
		size(std::exchange(other.size, { })),
		rasterFontHeight(std::exchange(other.rasterFontHeight, 0.0f))
	{
	}
	FontAtlas::FontAtlas(const FontAtlasDescriptor& descriptor, Graphics::GraphicsContextBase& graphicsContext, ResourceManager& resourceManager)
		: FontAtlas()
	{
		Create(descriptor, graphicsContext, resourceManager);
	}
	FontAtlas::~FontAtlas()
	{
	}
	void FontAtlas::Clear()
	{
		uvs.Clear();
	}
	void FontAtlas::Create(const FontAtlasDescriptor& descriptor, Graphics::GraphicsContextBase& graphicsContext, ResourceManager& resourceManager)
	{
		if (graphicsContext.GetImplementationName() == "OpenGL")
		{
			using namespace Graphics::OpenGL;

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

			if (!CreateAtlas(descriptor.fontFace, descriptor.rasterFontHeight, descriptor.glyphIndices, descriptor.glyphRasterizer, CreateAtlasCallback, CopyBitmapCallback))
				BLAZE_LOG_WARNING("Failed to create font atlas");

			rasterFontHeight = descriptor.rasterFontHeight;
			this->texture = texture;
		}	
		else
		{
			BLAZE_LOG_ERROR("Graphics context implementation not supported");
		}
	}
	bool FontAtlas::GetGlyphUV(uint32 glyphIndex, Vec2u16& uv1, Vec2u16& uv2) const
	{
		auto it = uvs.Find(glyphIndex);
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
		size = std::exchange(other.size, { });
		rasterFontHeight = std::exchange(other.rasterFontHeight, 0.0f);

		return *this;
	}
}