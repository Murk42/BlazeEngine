#include "pch.h"
#include "BlazeEngine/UI/Text/FontManager.h"
#include "BlazeEngine/UI/Text/FontAtlas.h"
#include "BlazeEngine/UI/Text/FontFace.h"
#include "BlazeEngine/UI/Core/Screen.h"

namespace Blaze::UI
{
	FontManager::FontManager(ResourceManager& resourceManager)
		: resourceManager(resourceManager)
	{
	}
	FontManager::~FontManager()
	{
	}
	bool FontManager::AddFontFace(StringView name, ResourceRef<FontFace> fontFace)
	{
		if (fontFace.IsNull())
		{
			entries.Erase(name);
			return true;
		}

		auto [it, inserted] = entries.Insert(name);
		
		if (inserted)
		{
			it->value.fontFace = fontFace;
			return true;
		}
		else
		{
			if (it->value.fontFace != fontFace)
			{
				it->value.atlasData.Clear();
				it->value.fontFace = fontFace;

				return true;
			}

			return false;
		}
	}
	bool FontManager::CreateFontAtlas(StringView name, ArrayView<float> fontAtlasPixelSizes, const FontGlyphRasterizer& glyphRasterizer, Graphics::RendererTypeID rendererTypeID, Graphics::GraphicsContextBase& graphicsContext)
	{
		auto it = entries.Find(name);

		if (it.IsNull())
			return false;

		for (auto& fontAtlasPixelSize : fontAtlasPixelSizes)
		{
			AtlasData* newAtlasData = nullptr;
			for (uintMem i = 0; i < it->value.atlasData.Count(); ++i)
			{
				AtlasData& atlasData = it->value.atlasData[i];
				if (atlasData.atlas.GetRasterFontHeight() == fontAtlasPixelSize)
				{
					newAtlasData = &it->value.atlasData[i];
					break;
				}
				else if (atlasData.atlas.GetRasterFontHeight() > fontAtlasPixelSize)
				{
					newAtlasData = it->value.atlasData.AddAt(i);
					break;
				}
			}

			if (newAtlasData == nullptr)
				newAtlasData = it->value.atlasData.AddBack();

			auto glyphIndices = it->value.fontFace->GetAllGlyphsInidices();
			FontAtlasDescriptor descriptor{
				.fontFace = *it->value.fontFace.GetValue(),
				.rasterFontHeight = fontAtlasPixelSize,
				.glyphIndices = glyphIndices,
				.glyphRasterizer = glyphRasterizer,
			};

			newAtlasData->rendererTypeID = rendererTypeID;
			newAtlasData->atlas.Create(descriptor, graphicsContext, resourceManager);
		}

		return true;
	}
	bool FontManager::CreateFontAtlas(StringView name, ArrayView<float> fontAtlasPixelSizes, const Graphics::TextRendererBase& textRenderer)
	{
		return CreateFontAtlas(name, fontAtlasPixelSizes, textRenderer.GetFontGlyphRasterizer(), textRenderer.GetTypeID(), textRenderer.GetGraphicsContext());
	}
	bool FontManager::GetFontAtlas(const TextStyle& style, FontAtlasData& fontAtlasData) const
	{
		if (style.fontHeight == 0.0f || style.fontName.Empty())
		{
			fontAtlasData.atlas = nullptr;
			fontAtlasData.fontFace = nullptr;
			fontAtlasData.rendererTypeID = 0;
			return false;
		}

		auto it = entries.Find(style.fontName);

		if (it.IsNull() || it->value.atlasData.Empty())
		{
			fontAtlasData.atlas = nullptr;
			fontAtlasData.fontFace = nullptr;
			fontAtlasData.rendererTypeID = 0;
			return false;
		}

		uintMem i = 0;

		for (; i < it->value.atlasData.Count(); ++i)
			if (it->value.atlasData[i].atlas.GetRasterFontHeight() > style.fontHeight)
				break;

		if (i == it->value.atlasData.Count())
			--i;

		fontAtlasData.fontFace = it->value.fontFace.GetValue();
		fontAtlasData.atlas = &it->value.atlasData[i].atlas;
		fontAtlasData.rendererTypeID = it->value.atlasData[i].rendererTypeID;
		return true;
	}
	void FontManager::AddScreenFontManager(Screen& screen, ResourceRef<FontManager> fontManager)
	{
		if (fontManager.IsNull())
		{
			BLAZE_LOG_WARNING("Trying to add a null reference FontManager to a screen");
			return;
		}

		screen.dataMap.map.Insert<ResourceRef<FontManager>>("FontManager", std::move(fontManager));
	}
	FontManager* FontManager::GetScreenFontManager(Screen& screen)
	{
		auto it = screen.dataMap.map.Find("FontManager");

		if (it.IsNull())
			return nullptr;

		return it.GetValue<ResourceRef<FontManager>>()->GetValue();
	}
}