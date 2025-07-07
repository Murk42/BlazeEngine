#pragma once
#include "BlazeEngineCore/DataStructures/Align.h"
#include "BlazeEngineCore/DataStructures/RawArray.h"
#include "BlazeEngineCore/DataStructures/Array.h"
#include "BlazeEngineCore/DataStructures/Map.h"
#include "BlazeEngineCore/DataStructures/Rect.h"
#include "BlazeEngineCore/Resource/ResourceRef.h"
#include "BlazeEngineGraphics/RenderScene/RenderUnit.h"
#include "BlazeEngineGraphics/Text/Core/TextStyle.h"
#include "BlazeEngineGraphics/Text/Core/FontAtlas.h"

//TODO after changing renderunit API remove this
#include "BlazeEngineGraphics/Renderers/TexturedRectRenderer.h"

namespace Blaze
{
	class FontFace;
	class FontAtlas;	
	class DefaultTextLayoutCache;
}

namespace Blaze::Graphics
{
	class BLAZE_GRAPHICS_API StaticTextRenderUnit :
		public Graphics::RenderUnit
	{
	public:
		struct GlyphRenderData
		{
			//Screen-space position (in pixels)
			Vec2f pos;
			Vec2f size;

			ColorRGBA color;

			//UV coordinates (in pixels)
			Vec2<uint16> uv1, uv2;
		};

		StaticTextRenderUnit();

		void BeginStream() override;
		void* Advance() override;		

		void SetTransform(Vec2f position, Vec2f right);
		Rectf SetText(StringViewUTF8 text, ArrayView<TextStyleSpan> styleSpans, float maxLineLength, ResourceRef<TextStyle> defaultTextStyle, const Map<FontAtlasIdentifier, const FontAtlas&>& atlases);
		Rectf SetText(const DefaultTextLayoutCache& cache, const Map<FontAtlasIdentifier, const FontAtlas&>& atlases);
	private:		
		struct AtlasData
		{
			const FontAtlas& atlas;
			uintMem lastGlyphDataIndex;
		};
		
		uintMem glyphDataIndex;
		uintMem atlasDataIndex;
		Graphics::TexturedRectRenderData renderData;

		Vec2f position;
		Vec2f right;		
		Array<AtlasData> atlasData; //TODO this doesn't need to be a dynamic Array, it doesn't need the reserved member
		RawArray<GlyphRenderData> glyphRenderData;
	};
}