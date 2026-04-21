#include "pch.h"
#include "BlazeEngine/Core/File/File.h"
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
	FontManager::FontManager(ResourceManager& resourceManager, Screen& screen)
		: resourceManager(resourceManager)
	{
		AddToScreen(screen);
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
	bool FontManager::CreateFontAtlas(StringView name, ArrayView<uint32> fontAtlasPixelSizes, const FontGlyphRasterizer& glyphRasterizer, Graphics::RendererTypeID rendererTypeID, Graphics::GraphicsContext& graphicsContext, bool useCache)
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
			
			bool createAtlas = true;

			newAtlasData->rendererTypeID = rendererTypeID;

			if (useCache)
			{
				Blaze::File file{ Path(Format("cache/{}_{}.bzatlas", name, fontAtlasPixelSize)), FileAccessPermission::Read};
				
				if (file.IsOpen())
				{
					createAtlas = false;
					newAtlasData->atlas.Load(file, graphicsContext, resourceManager);
				}
			}
			
			if (createAtlas)
			{
				newAtlasData->atlas.Create(descriptor, graphicsContext, resourceManager);

				if (useCache)
				{
					Blaze::File file{ Path(Format("cache/{}_{}.bzatlas", name, fontAtlasPixelSize)), FileAccessPermission::Write };
					newAtlasData->atlas.Save(file);
				}
			}
		}

		return true;
	}
	bool FontManager::CreateFontAtlas(StringView name, ArrayView<uint32> fontAtlasPixelSizes, const Graphics::TextRendererBase& textRenderer, bool useCache)
	{
		return CreateFontAtlas(name, fontAtlasPixelSizes, textRenderer.GetFontGlyphRasterizer(), textRenderer.GetTypeID(), textRenderer.GetGraphicsContext(), useCache);
	}
	bool FontManager::GetFontAtlas(StringView name, uint32 fontHeight, FontAtlasData& fontAtlasData) const
	{
		if (fontHeight == 0.0f || name.Empty())
		{
			fontAtlasData.atlas = nullptr;
			fontAtlasData.fontFace = nullptr;
			fontAtlasData.rendererTypeID = 0;
			return false;
		}

		auto it = entries.Find(name);

		if (it.IsNull() || it->value.atlasData.Empty())
		{
			fontAtlasData.atlas = nullptr;
			fontAtlasData.fontFace = nullptr;
			fontAtlasData.rendererTypeID = 0;
			return false;
		}

		uintMem i = 0;

		for (; i < it->value.atlasData.Count(); ++i)
			if (it->value.atlasData[i].atlas.GetRasterFontHeight() >= fontHeight)
				break;

		if (i == it->value.atlasData.Count())
			--i;

		fontAtlasData.fontFace = it->value.fontFace.GetValue();
		fontAtlasData.atlas = &it->value.atlasData[i].atlas;
		fontAtlasData.rendererTypeID = it->value.atlasData[i].rendererTypeID;
		return true;
	}
	void FontManager::AddToScreen(Screen& screen)
	{
		screen.dataMap.map.Insert<FontManager*>("font manager", this);
	}
	FontManager* FontManager::GetScreenFontManager(Screen& screen)
	{
		auto it = screen.dataMap.map.Find("font manager");

		if (it.IsNull())
			return nullptr;

		auto ptr = it.GetValue<FontManager*>();

		if (ptr == nullptr)
			return nullptr;

		return *ptr;
	}
}