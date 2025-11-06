#include "pch.h"
#include "BlazeEngineGraphics/RenderScene/RenderUnits/DynamicTextRenderUnit.h"
#include "BlazeEngineGraphics/Text/Core/FontAtlas.h"
#include "BlazeEngineCore/Resource/Resource.h"
#include "BlazeEngineGraphics/Core/OpenGL/OpenGLWrapper/Textures/OpenGLTexture2D.h"
#include "BlazeEngineGraphics/Text/TextFormatter/DefaultTextFormatter.h"

namespace Blaze::Graphics
{
	DynamicTextRenderUnit::DynamicTextRenderUnit()
		: RenderUnit("ColoredCharacterRenderer"), clusterIndex(0), glyphDataIndex(0)
	{
	}
	void DynamicTextRenderUnit::BeginStream()
	{
		clusterIndex = 0;
		glyphDataIndex = 0;
		baselineOffset = 0;
	}
	void* DynamicTextRenderUnit::Advance()
	{	
		const FontAtlas* atlas = nullptr;

		while (true)
		{
			if (clusterIndex == cache.clusterLayoutData.Count())
				return nullptr;

			auto it = atlases.Find(AtlasKey(cache.clusterLayoutData[clusterIndex].style.Get()));

			if (!it.IsNull())
			{
				atlas = &it->value.Get();
				break;
			}

			NextGlyph();
		}

		const auto& clusterData = cache.clusterLayoutData[clusterIndex];
		const auto& lineData = cache.lineLayoutData[clusterData.lineIndex];
		const auto& glyphData = cache.glyphData[glyphDataIndex];
		Vec2f up = Vec2f(-right.y, right.x);

		Vec2u16 uv1, uv2;
		atlas->GetGlyphUV(glyphData.glyphIndex, uv1, uv2);

		renderData.blend = 0.0f;
		renderData.alpha = 1.0f;
		renderData.pos = Vec2f(glyphData.clusterOffset + clusterData.lineAdvance + lineData.baselineOffset) + position;
		renderData.right = right * glyphData.size.x;
		renderData.up = up * glyphData.size.y;
		renderData.color = ColorRGBAf(cache.clusterLayoutData[clusterIndex].style.Get().color);
		renderData.uv1 = Vec2f(uv1) / atlas->GetSize();
		renderData.uv2 = Vec2f(uv2) / atlas->GetSize();
		renderData.texture = atlas->GetTexture();

		NextGlyph();

		return &renderData;
	}
	void DynamicTextRenderUnit::SetTransform(Vec2f position, Vec2f right)
	{
		this->position = position;
		this->right = right;
	}
	void DynamicTextRenderUnit::SetText(TextFormatterCache cache, Map<AtlasKey, ResourceRef<FontAtlas>> atlases)
	{
		this->cache = std::move(cache);
		this->atlases = std::move(atlases);

		cacheUpdatedEventDispatcher.Call({ *this });
	}

	void DynamicTextRenderUnit::NextGlyph()
	{
		++glyphDataIndex;
		if (glyphDataIndex == cache.clusterLayoutData[clusterIndex].glyphEndIndex)
		{
			++clusterIndex;

			if (clusterIndex != cache.clusterLayoutData.Count())
				glyphDataIndex = cache.clusterLayoutData[clusterIndex].glyphStartIndex;
		}
	}

	DynamicTextRenderUnit::AtlasKey::AtlasKey(const TextStyle& style)
		: fontFace(style.fontFace.Get()), fontSize(style.fontSize)
	{
	}

}