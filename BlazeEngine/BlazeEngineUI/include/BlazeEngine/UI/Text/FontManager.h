#pragma once
#include "BlazeEngine/Core/String/String.h"
#include "BlazeEngine/Core/Container/ArrayView.h"
#include "BlazeEngine/Core/Container/Map.h"
#include "BlazeEngine/Core/Resource/ResourceRef.h"
#include "BlazeEngine/Core/Resource/ResourceManager.h"
#include "BlazeEngine/Graphics/Core/GraphicsContextBase.h"
#include "BlazeEngine/UI/Graphics/Renderers/TextRendererBase.h"
#include "BlazeEngine/UI/Text/FontAtlas.h"
#include "BlazeEngine/UI/Text/TextStyle.h"

namespace Blaze::UI
{
	class FontFace;
	class Screen;

	class FontManager
	{
	public:
		struct FontAtlasData
		{
			const FontAtlas* atlas;
			const FontFace* fontFace;
			Graphics::RendererTypeID rendererTypeID;
		};

		FontManager(ResourceManager& resourceManager);
		~FontManager();
		
		bool AddFontFace(StringView name, ResourceRef<FontFace> fontFace);
		bool CreateFontAtlas(StringView name, ArrayView<float> fontAtlasPixelSizes, const FontGlyphRasterizer& glyphRasterizer, Graphics::RendererTypeID rendererTypeID, Graphics::GraphicsContextBase& graphicsContext);
		bool CreateFontAtlas(StringView name, ArrayView<float> fontAtlasPixelSizes, const Graphics::TextRendererBase& textRenderer);

		bool GetFontAtlas(const TextStyle& style, FontAtlasData& fontAtlasData) const;

		static void AddScreenFontManager(Screen& screen, ResourceRef<FontManager> fontManager);
		static FontManager* GetScreenFontManager(Screen& screen);
	private:
		struct AtlasData
		{
			FontAtlas atlas;
			Graphics::RendererTypeID rendererTypeID;
		};
		struct FontFaceData
		{
			ResourceRef<FontFace> fontFace;
			Array<AtlasData> atlasData;
		};
		ResourceManager& resourceManager;
		Map<StringView, FontFaceData> entries;
	};
}