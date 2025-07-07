#include "pch.h"
#include "BlazeEngineGraphics/RenderScene/RenderUnits/StaticTextRenderUnit.h"
#include "BlazeEngineGraphics/Text/Core/FontAtlas.h"
#include "BlazeEngineCore/Resource/Resource.h"
#include "BlazeEngineGraphics/Core/OpenGL/OpenGLWrapper/Textures/OpenGLTexture2D.h"
#include "BlazeEngineGraphics/Text/TextLayoutCache/DefaultTextLayoutCache.h"

namespace Blaze::Graphics
{
	StaticTextRenderUnit::StaticTextRenderUnit()
		: RenderUnit("ColoredCharacterRenderer"), glyphDataIndex(0), atlasDataIndex(0)
	{
	}
	void StaticTextRenderUnit::BeginStream()
	{
		glyphDataIndex = 0;
		atlasDataIndex = 0;
	}
	void* StaticTextRenderUnit::Advance()
	{
		if (atlasDataIndex == atlasData.Count())
			return nullptr;		

		const auto& atlasData = this->atlasData[atlasDataIndex];
		const auto& glyphRenderData = this->glyphRenderData[glyphDataIndex];

		Vec2f up = Vec2f(-right.y, right.x);

		renderData.blend = 0.0f;
		renderData.alpha = 1.0f;
		renderData.pos = glyphRenderData.pos + position;
		renderData.right = right * glyphRenderData.size.x;
		renderData.up = up * glyphRenderData.size.y;
		renderData.color = ColorRGBAf(glyphRenderData.color);
		renderData.uv1 = Vec2f(glyphRenderData.uv1) / atlasData.atlas.GetSize();
		renderData.uv2 = Vec2f(glyphRenderData.uv2) / atlasData.atlas.GetSize();
		renderData.texture = atlasData.atlas.GetTexture();

		++glyphDataIndex;

		if (atlasData.lastGlyphDataIndex == glyphDataIndex)
			++atlasDataIndex;

		return &renderData;
	}
	void StaticTextRenderUnit::SetTransform(Vec2f position, Vec2f right)
	{
		this->position = position;
		this->right = right;
	}
	Rectf StaticTextRenderUnit::SetText(StringViewUTF8 text, ArrayView<TextStyleSpan> styleSpans, float lineWrapLength, ResourceRef<TextStyle> defaultTextStyle, const Map<FontAtlasIdentifier, const FontAtlas&>& atlases)
	{
		DefaultTextLayoutCache layoutCache;
		layoutCache.SetString(text);
		layoutCache.SetStyleSpans(styleSpans);
		layoutCache.SetLineWrapLength(lineWrapLength);
		layoutCache.SetDefaultStyle(defaultTextStyle);		

		layoutCache.CleanCache();

		return SetText(layoutCache, atlases);
	}

	Rectf StaticTextRenderUnit::SetText(const DefaultTextLayoutCache& cache, const Map<FontAtlasIdentifier, const FontAtlas&>& atlases)
	{
		uintMem glyphRenderDataCount = 0;
		Map<const FontAtlas&, Array<GlyphRenderData>> tempGlyphRenderDataMap;
				
		auto& textShapingData = cache.GetTextShapingData();
		auto& textLayoutData = cache.GetTextLayoutData();

		for (uintMem i = 0; i < textShapingData.clusters.Count(); ++i)
		{			
			auto& style = textShapingData.clusters[i].style.Get();

			auto atlasIt = atlases.Find({style.fontFace.Get(), style.fontSize});

			if (atlasIt.IsNull())
				continue;

			auto& atlas = atlasIt->value;

			auto [it, inserted] = tempGlyphRenderDataMap.Insert(atlas);

			for (uintMem j = textShapingData.clusters[i].glyphStartIndex; j != textShapingData.clusters[i].glyphEndIndex; ++j)
			{
				GlyphRenderData glyphRenderData;

				glyphRenderData.color = textShapingData.clusters[i].style.Get().color;
				glyphRenderData.pos = Vec2f(textShapingData.glyphs[j].clusterOffset) + textLayoutData.clusters[i].lineOffset + textLayoutData.lines[textLayoutData.clusters[i].lineIndex].position;
				glyphRenderData.size = Vec2f(textShapingData.glyphs[j].size);
				atlas.GetGlyphUV(textShapingData.glyphs[j].glyphIndex, glyphRenderData.uv1, glyphRenderData.uv2);

				it->value.AddBack(glyphRenderData);
				++glyphRenderDataCount;
			}
		}

		atlasData.ReserveExactly(tempGlyphRenderDataMap.Count());
		glyphRenderData.Reallocate(glyphRenderDataCount);

		uintMem offset = 0;
		for (auto& glyphRenderDataMapPair : tempGlyphRenderDataMap)
		{
			for (uintMem i = 0; i < glyphRenderDataMapPair.value.Count(); ++i)
				glyphRenderData[i] = glyphRenderDataMapPair.value[i + offset];

			offset += glyphRenderDataMapPair.value.Count();

			atlasData.AddBack(AtlasData{ glyphRenderDataMapPair.key, offset });
		}

		Vec2f p1, p2;

		for (auto& line : cache.GetTextLayoutData().lines)
		{
			p1.x = std::min(p1.x, line.position.x);
			p1.y = std::min(p1.y, line.position.y);
			p2.x = std::max(p2.x, line.position.x + line.size.x);
			p2.y = std::max(p2.y, line.position.y + line.size.y);
		}

		return Rectf(p1, p2 - p1);
	}

}