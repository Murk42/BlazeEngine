#include "pch.h"
#include "BlazeEngineGraphics/UI/Graphics/RenderStreams/OpenGL/TextRenderStream_OpenGL.h"
#include "BlazeEngineGraphics/UI/Elements/UIText.h"

namespace Blaze::Graphics::OpenGL
{	
	bool RectIntersect(Rectf r1, Rectf r2)
	{
		return r1.x + r1.w > r2.x && r1.x < r2.x + r2.w &&
			r1.y + r1.h > r2.y && r1.y < r2.y + r2.h;
	}
	float clamp(float value, float min, float max)
	{
		if (value > max)
			return max;
		if (value < min)
			return min;
		return value;
	}

	AtlasData_OpenGL::UVRect AtlasData_OpenGL::GetUVRect(UnicodeChar character) const
	{
		auto it = uvs.Find(character);
		if (!it.IsNull())
			return it->value;

		return {};
	}
	FontAtlasesData_OpenGL::FontAtlasesData_OpenGL(Font* font, Array<uint> fontHeights, const CharacterSet& characterSet)
	{
		for (auto fontHeight : fontHeights)
		{
			auto& data = *atlasData.AddBack();
			data.fontHeight = fontHeight;

			// doesnt work because of alpha channel: #define HORIZONTAL_LCD

#ifdef HORIZONTAL_LCD
			FontGlyphRenderers::HorizontalLCDFontGlyphRenderer glyphRenderer;
#else
			FontGlyphRenderers::AntialiasedFontGlyphRenderer glyphRenderer;
#endif
			auto metrics = font->GetMetrics(fontHeight);

			Vec2f atlasSize;
			metrics.CreateAtlas(characterSet, glyphRenderer, [&](auto size) {
#ifdef HORIZONTAL_LCD
				data.atlasData.atlas.Create(size, OpenGLWrapper::TextureInternalPixelFormat::RGB8);
#else
				data.atlasData.atlas.Create(size, OpenGLWrapper::TextureInternalPixelFormat::R8);
#endif
				atlasSize = Vec2f(size);
				}, [&](UnicodeChar character, Vec2u offset, BitmapView bm) {
					Vec2f uv1 = Vec2f(offset);
					Vec2f uv2 = Vec2f(offset + bm.GetSize());
					uv1 /= atlasSize;
					uv2 /= atlasSize;

					std::swap(uv1.y, uv2.y);

					data.atlasData.atlas.CopyPixels(offset, bm);
					data.atlasData.uvs.Insert(character, AtlasData_OpenGL::UVRect({ uv1, uv2 }));
					});

			data.atlasData.atlas.SetSettings({
				.min = OpenGLWrapper::TextureSampling::Linear,
				.mag = OpenGLWrapper::TextureSampling::Linear,
				.mip = OpenGLWrapper::TextureSampling::Linear
				});
#ifdef HORIZONTAL_LCD
			data.atlasData.atlas.SetSwizzle(
				OpenGLWrapper::TextureSwizzle::R,
				OpenGLWrapper::TextureSwizzle::G,
				OpenGLWrapper::TextureSwizzle::B,
				OpenGLWrapper::TextureSwizzle::O);
#else
			data.atlasData.atlas.SetSwizzle(
				OpenGLWrapper::TextureSwizzle::One,
				OpenGLWrapper::TextureSwizzle::One,
				OpenGLWrapper::TextureSwizzle::One,
				OpenGLWrapper::TextureSwizzle::R);
#endif
		}
	}
	AtlasData_OpenGL* FontAtlasesData_OpenGL::GetAtlasData(uint fontHeight)
	{
		AtlasData_OpenGL* bestBellow = nullptr;
		uint fontHeightBellow = 0;
		AtlasData_OpenGL* bestAbove = nullptr;
		uint fontHeightAbove = UINT_MAX;

		for (auto& data : atlasData)
		{
			if (data.fontHeight == fontHeight)
				return &data.atlasData;

			if (data.fontHeight > fontHeight && data.fontHeight < fontHeightAbove)
			{
				bestAbove = &data.atlasData;
				fontHeightAbove = data.fontHeight;
			}

			if (data.fontHeight < fontHeight && data.fontHeight > fontHeightBellow)
			{
				bestBellow = &data.atlasData;
				fontHeightBellow = data.fontHeight;
			}

		}

		if (bestAbove == nullptr)
			return bestBellow;
		return bestAbove;
	}

	TextRenderStream_OpenGL::TextRenderStream_OpenGL(UI::Text* text)
		: text(text)
	{

	}
	void TextRenderStream_OpenGL::BeginStream()
	{
		Font* newFont = text->GetFont();
		uint newFontHegiht = text->GetPixelFontHeight() * text->GetFinalScale();

		if (newFont != font || newFontHegiht != fontHeight)
		{
			font = newFont;
			fontHeight = newFontHegiht * text->GetFinalScale();

			auto fontAtlasesData = font->dataMap.Find("FontAtlasData_OpenGL").GetValue<FontAtlasesData_OpenGL>();

			if (fontAtlasesData == nullptr)
				atlasData = nullptr;
			else
				atlasData = fontAtlasesData->GetAtlasData(fontHeight);
		}
		
		characterIndex = 0;

		pos = text->GetFinalPosition();
		scale = text->GetFinalScale();

		float rotation = text->GetFinalRotation();
		float cos = Math::Cos(rotation);
		float sin = Math::Sin(rotation);
		right = Vec2f(cos, sin) * scale;
		up = Vec2f(-sin, cos) * scale;
		
		if (text->cullingNode != nullptr && text->cullingNode->GetFinalRotation() == rotation)
		{
			Vec2f cullingPos = text->cullingNode->GetFinalPosition();
			Vec2f cullingSize = text->cullingNode->GetFinalSize();

			Vec2f cullingSpacePos = cullingPos - pos;

			cullingRect.pos.x = right.DotProduct(cullingSpacePos) / scale / scale;
			cullingRect.pos.y = up.DotProduct(cullingSpacePos) / scale / scale;
			cullingRect.size = cullingSize / scale;
		}
		else
			cullingRect = { Vec2f(), Vec2f() };

		if (abs(cos) == 1 || abs(sin) == 1)
		{
			pos.x = std::ceil(pos.x);
			pos.y = std::ceil(pos.y);
		}
	}
	void* TextRenderStream_OpenGL::GetCurrent() 
	{
		if (text->GetCharacterData().Count() == characterIndex)
		{
			rd = {
				.color = 0x00000000,
				.blend = 1
			};
			return &rd;
		}

		auto data = text->GetCharacterData()[characterIndex];
			
		float percentLeft = 0;
		float percentRight = 1;
		float percentDown = 0;
		float percentUp = 1;

		if (text->cullingNode != nullptr)
		{
			if (data.pos.x < cullingRect.x)			
				percentLeft = clamp((cullingRect.x - data.pos.x) / data.size.x, 0, 1);
			if (data.pos.x + data.size.x > cullingRect.x + cullingRect.w)
				percentRight = clamp((cullingRect.x + cullingRect.w - data.pos.x) / data.size.x, 0, 1);
			if (data.pos.y < cullingRect.y)
				percentDown = clamp((cullingRect.y - data.pos.y) / data.size.y, 0, 1);
			if (data.pos.y + data.size.y > cullingRect.y + cullingRect.h)
				percentUp = clamp((cullingRect.y + cullingRect.h - data.pos.y) / data.size.y, 0, 1);
		}

		rd = {
			.texture = nullptr,
			.uv1 = Vec2f(),
			.uv2 = Vec2f(),
			.pos = pos + 
				right * (data.pos.x + percentLeft * data.size.x)+ 
				up * (data.pos.y + percentDown * data.size.y),
			.right = right * data.size.x * (percentRight - percentLeft),
			.up = up * data.size.y * (percentUp - percentDown),
			.color = ColorRGBAf(1.0, 0.0f, 0.0f, 1.0f),
			.blend = 1.0f,
			.alpha = 1.0f
		};

		if (atlasData != nullptr)
		{
			auto uvRect = AtlasData_OpenGL::UVRect();
			uvRect = atlasData->GetUVRect(data.character);

			Vec2f uvSize = uvRect.uv2 - uvRect.uv1;

			rd.blend = 1.0f;
			rd.alpha = 1.0f;
			rd.color = data.color;
			rd.uv1 = uvRect.uv1 + Vec2f(percentLeft, percentDown) * uvSize;
			rd.uv2 = uvRect.uv1 + Vec2f(percentRight, percentUp) * uvSize;
			rd.texture = atlasData->GetAtlas();
		}		

		return &rd;
	}
	void TextRenderStream_OpenGL::Advance()
	{
		auto data = text->GetCharacterData();

		if (data.Count() == characterIndex)
			return;

		if (text->cullingNode != nullptr)
		{			
			while (true)
			{
				++characterIndex;

				if (data.Count() == characterIndex)
					return;

				if (RectIntersect(cullingRect, { data[characterIndex].pos, data[characterIndex].size }))
					break;
			}
		}
		else		
			++characterIndex;		
	}
	bool TextRenderStream_OpenGL::IsEmpty()
	{
		return text->GetCharacterData().Count() == 0 || text->GetCharacterData().Count() == characterIndex;
	}
}