#include "BlazeEngine/Graphics/Renderers/TextRenderCache.h"
#include "BlazeEngine/Graphics/Renderers/TextVertexGenerator.h"

namespace Blaze::Graphics
{
	TextRenderCache::TextRenderCache()
		: vertices(nullptr), vertexCount(0), fontResolution(nullptr)
	{		
	}
	TextRenderCache::TextRenderCache(TextRenderCache&& data) noexcept
		:  vertices(data.vertices), vertexCount(data.vertexCount), fontResolution(data.fontResolution)
	{
		data.vertices = nullptr;
		data.vertexCount = 0;
	}
	TextRenderCache::TextRenderCache(StringViewUTF8 text, float fontHeight, FontResolution* fontRes, std::span<const ColorRGBAf> colors)
		: TextRenderCache()
	{
		GenerateVertices(text, fontHeight, fontRes, colors);
	}

	TextRenderCache::~TextRenderCache()
	{
		delete[] vertices;
	}
	

	Result TextRenderCache::GenerateVertices(StringViewUTF8 text, float fontHeight, FontResolution* fontResolution, std::span<const ColorRGBAf> colors)
	{
		DefaultTextVertexGenerator generator;
		return GenerateVertices(text, fontHeight, fontResolution, colors, generator);
	}
	Result TextRenderCache::GenerateVertices(StringViewUTF8 text, float fontHeight, FontResolution* fontResolution, std::span<const ColorRGBAf> colors, BaseTextVertexGenerator& generator)
	{		
		if (fontResolution == nullptr)
			return BLAZE_WARNING_RESULT("Blaze Engine", "fontResolution is nullptr");		

		generator.Setup(text, fontResolution);

		this->fontResolution = fontResolution;

		bottomLeft = { FLT_MAX, FLT_MAX };
		topRight = { FLT_MIN, FLT_MIN };

		delete[] vertices;
		vertices = new Vertex[generator.GetMaxVertexCount()];
		Vertex* it = vertices;
		float advance;

		float scale = fontHeight / fontResolution->GetResolution();

		uint index = 0;
		while (!generator.IsEnd())
		{
			if (generator.GenerateVertex(it->p1, it->p2, it->uv1, it->uv2, advance))
			{
				bottomLeft.x = std::min({ bottomLeft.x, it->p1.x, it->p2.x });
				bottomLeft.y = std::min({ bottomLeft.y, it->p1.y, it->p2.y });
				topRight.x = std::max({ topRight.x, it->p1.x, it->p2.x });
				topRight.y = std::max({ topRight.y, it->p1.y, it->p2.y });

				it->p1 *= scale;
				it->p2 *= scale;	

				if (index < colors.size())
					it->color = (Vec4f)colors[index];
				else if (colors.size() > 0)
					it->color = (Vec4f)colors.back();
				else
					it->color = Vec4f(1, 1, 1, 1);

				++it;
				++index;
			}
		}

		this->vertexCount = it - vertices;

		return Result();

		//vb.AllocateDynamicStorage(
		//	BufferView(vertices, vertexCount * sizeof(Vertex)),
		//	Core::GraphicsBufferDynamicStorageHint::DynamicDraw
		//);		
	}

	void TextRenderCache::SetColors(std::span<ColorRGBAf> colors)
	{
		for (int i = 0; i < vertexCount; ++i)
		{
			if (i < colors.size())
				vertices[i].color = (Vec4f)colors[i];
			else
				vertices[i].color = Vec4f(1, 1, 1, 1);
		}			
	}
	void TextRenderCache::SetColor(ColorRGBAf color)
	{
		for (int i = 0; i < vertexCount; ++i)
			vertices[i].color = (Vec4f)color;
	}
}