#include "pch.h"
#include "BlazeEngineGraphics/RenderScene/RenderUnits/StaticTextRenderUnit.h"
#include "BlazeEngineGraphics/Text/Core/FontAtlas.h"
#include "BlazeEngineCore/Resource/Resource.h"
#include "BlazeEngineGraphics/Core/OpenGL/OpenGLWrapper/Textures/OpenGLTexture2D.h"

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
	Rectf StaticTextRenderUnit::SetText(u8StringView text, TextStyleData* textStyleData, float lineWrapLength, ResourceRef<TextStyle> defaultTextStyle, const Map<FontAtlasIdentifier, const FontAtlas&>& atlases)
	{
		//TextLayoutCacheBase layoutCache;
		//layoutCache.CreateCache(text, textStyleData, defaultTextStyle);
		//
		//return SetText(layoutCache, atlases);
		return {};
	}

	Rectf StaticTextRenderUnit::SetText(const TextLayoutCacheBase& cache, const Map<FontAtlasIdentifier, const FontAtlas&>& atlases)
	{
		//glyphRenderData.Clear();
		//atlasData.Clear();
		//
		//uintMem glyphRenderDataCount = 0;
		//Map<const FontAtlas&, Array<GlyphRenderData>> tempGlyphRenderDataMap;
		//		
		//auto glyphs = cache.GetGlyphData();		
		//
		//if (glyphs.Empty())
		//	return;
		//
		//ResourceRef<TextStyle> style;
		//Map<FontAtlasIdentifier, const FontAtlas&>::ConstIterator atlasIt;
		//const FontAtlas* atlas = nullptr;
		//
		//for (auto& glyph : glyphs)
		//{
		//	if (glyph.style != style)
		//	{
		//		auto newAtlasIt = atlases.Find({ style->GetFontFace().Get(), style->GetFontSize() });
		//
		//		if (newAtlasIt.IsNull())
		//			continue;
		//
		//		atlas = &newAtlasIt->value;
		//		atlasIt = newAtlasIt;
		//		style = glyph.style;
		//
		//		auto [it, inserted] = tempGlyphRenderDataMap.Insert(*atlas);
		//	}
		//
		//	GlyphRenderData glyphRenderData;
		//
		//	glyphRenderData.color = style->GetColor();
		//	glyphRenderData.pos = glyph.clusterOffset;
		//	glyphRenderData.size = Vec2f(textShapingData.glyphs[j].size);
		//	atlas.GetGlyphUV(textShapingData.glyphs[j].glyphIndex, glyphRenderData.uv1, glyphRenderData.uv2);
		//
		//	it->value.AddBack(glyphRenderData);
		//	++glyphRenderDataCount;
		//}
		//
		//atlasData.ReserveExactly(tempGlyphRenderDataMap.Count());
		//glyphRenderData.Reallocate(glyphRenderDataCount);
		//
		//uintMem offset = 0;
		//for (auto& glyphRenderDataMapPair : tempGlyphRenderDataMap)
		//{
		//	for (uintMem i = 0; i < glyphRenderDataMapPair.value.Count(); ++i)
		//		glyphRenderData[i] = glyphRenderDataMapPair.value[i + offset];
		//
		//	offset += glyphRenderDataMapPair.value.Count();
		//
		//	atlasData.AddBack(AtlasData{ glyphRenderDataMapPair.key, offset });
		//}
		//
		//Vec2f p1, p2;
		//
		//for (auto& line : cache.GetTextLayoutData().lines)
		//{
		//	p1.x = std::min(p1.x, line.position.x);
		//	p1.y = std::min(p1.y, line.position.y);
		//	p2.x = std::max(p2.x, line.position.x + line.size.x);
		//	p2.y = std::max(p2.y, line.position.y + line.size.y);
		//}
		//
		//return Rectf(p1, p2 - p1);
		return {};
	}

}