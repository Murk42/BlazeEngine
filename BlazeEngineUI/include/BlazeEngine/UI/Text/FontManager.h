#pragma once
#include "BlazeEngine/Core/String/String.h"
#include "BlazeEngine/Core/Container/ArrayView.h"
#include "BlazeEngine/Core/Container/Map.h"
#include "BlazeEngine/Core/Resource/ResourceRef.h"
#include "BlazeEngine/Core/Resource/ResourceManager.h"
#include "BlazeEngine/Graphics/Core/GraphicsContext.h"
#include "BlazeEngine/UI/Graphics/Renderers/TextRendererBase.h"
#include "BlazeEngine/UI/Text/FontAtlas.h"

namespace Blaze::UI
{
	class FontFace;
	class Screen;

	class FontManager
	{
	public:
		struct FontData
		{
			const FontAtlas* atlas;
			const FontFace* fontFace;
			Graphics::RendererTypeID rendererTypeID;
		};

		FontManager(ResourceManager& resourceManager);
		FontManager(ResourceManager& resourceManager, Screen& screen);
		~FontManager();
		
		bool AddFontFace(StringView name, ResourceRef<FontFace> fontFace);
		bool CreateFontAtlas(StringView name, ArrayView<uint32> fontAtlasPixelSizes, const FontGlyphRasterizer& glyphRasterizer, Graphics::RendererTypeID rendererTypeID, Graphics::GraphicsContext& graphicsContext, bool useCache = true);
		bool CreateFontAtlas(StringView name, ArrayView<uint32> fontAtlasPixelSizes, const Graphics::TextRendererBase& textRenderer, bool useCache = true);

		bool GetFontAtlas(StringView name, uint32 fontHeight, FontData& fontAtlasData) const;

		void AddToScreen(Screen& screen);
		static FontManager* GetScreenFontManager(Screen& screen);
	private:
		struct AtlasData
		{
			FontAtlas atlas;
			Graphics::RendererTypeID rendererTypeID{ };
		};
		struct FontFaceData
		{
			ResourceRef<FontFace> fontFace;
			Array<AtlasData> fontData;
		};
		ResourceManager& resourceManager;
		Map<StringView, FontFaceData> entries;
	};
}