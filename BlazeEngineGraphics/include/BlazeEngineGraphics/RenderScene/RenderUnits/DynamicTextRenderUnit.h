#pragma once
#include "BlazeEngineCore/DataStructures/Align.h"
#include "BlazeEngineCore/DataStructures/RawArray.h"
#include "BlazeEngineCore/DataStructures/Array.h"
#include "BlazeEngineCore/DataStructures/Map.h"
#include "BlazeEngineCore/Resource/ResourceRef.h"
#include "BlazeEngineGraphics/RenderScene/RenderUnit.h"
#include "BlazeEngineGraphics/Text/Core/TextStyle.h"
#include "BlazeEngineGraphics/Text/Core/TextFormatterCache.h"

//TODO after changing renderunit API remove this
#include "BlazeEngineGraphics/Renderers/TexturedRectRenderer.h"

namespace Blaze
{
	class FontFace;
	class FontAtlas;	
}

namespace Blaze::Graphics
{
	class BLAZE_GRAPHICS_API DynamicTextRenderUnit :
		public Graphics::RenderUnit
	{
	public:
		struct AtlasKey
		{
			const FontFace& fontFace;
			float fontSize;

			AtlasKey(const TextStyle& style);

			bool operator==(const AtlasKey& other) const { return &fontFace == &other.fontFace && fontSize == other.fontSize; }
			uint32 Hash() const
			{
				return Blaze::Hash<const FontFace*>().Compute(&fontFace) + Blaze::Hash<float>().Compute(fontSize);
			}
		};
		struct CacheUpdatedEvent
		{
			DynamicTextRenderUnit& renderUnit;
		};
		EventDispatcher<CacheUpdatedEvent> cacheUpdatedEventDispatcher;

		DynamicTextRenderUnit();

		void BeginStream() override;
		void* Advance() override;

		void SetTransform(Vec2f position, Vec2f right);
		void SetText(TextFormatterCache cache, Map<AtlasKey, ResourceRef<FontAtlas>> atlases);		

		const TextFormatterCache& GetCache() const { return cache; }
	private:
		uintMem clusterIndex;
		uintMem glyphDataIndex;		
		float baselineOffset;
		Graphics::TexturedRectRenderData renderData;		

		TextFormatterCache cache;		
		Map<AtlasKey, ResourceRef<FontAtlas>> atlases;		
		Vec2f position;
		Vec2f right;

		void NextGlyph();
	};
}