#include "BlazeEngine/Graphics/Renderers/TextRenderCache.h"
#include "BlazeEngine/Graphics/Renderers/TextVertexGenerator.h"

namespace Blaze::Graphics
{
	TextRenderCache::TextRenderCache()
		: vertexCount(0), fontResolution(nullptr)
	{
		va.EnableVertexAttribute(0);
		va.EnableVertexAttribute(1);
		va.EnableVertexAttribute(2);
		va.EnableVertexAttribute(3);
		va.SetVertexAttributeFormat(0, Core::VertexAttributeType::Float, 2, false, sizeof(float) * 0);
		va.SetVertexAttributeFormat(1, Core::VertexAttributeType::Float, 2, false, sizeof(float) * 2);
		va.SetVertexAttributeFormat(2, Core::VertexAttributeType::Float, 2, false, sizeof(float) * 4);
		va.SetVertexAttributeFormat(3, Core::VertexAttributeType::Float, 2, false, sizeof(float) * 6);
		va.SetVertexAttributeBuffer(0, &vb, sizeof(Vertex), 0);
		va.SetVertexAttributeBuffer(1, &vb, sizeof(Vertex), 0);
		va.SetVertexAttributeBuffer(2, &vb, sizeof(Vertex), 0);
		va.SetVertexAttributeBuffer(3, &vb, sizeof(Vertex), 0);
	}
	TextRenderCache::TextRenderCache(TextRenderCache&& data) noexcept
		: vb(std::move(data.vb)), va(std::move(data.va)), vertexCount(data.vertexCount), fontResolution(data.fontResolution)
	{
	}
	TextRenderCache::TextRenderCache(StringViewUTF8 text, FontResolution* fontRes)
		: TextRenderCache()
	{
		GenerateVertices(text, fontRes);
	}

	void TextRenderCache::GenerateVertices(StringViewUTF8 text, FontResolution* fontResolution)
	{
		DefaultTextVertexGenerator generator;
		GenerateVertices(text, fontResolution, generator);
	}
	void TextRenderCache::GenerateVertices(StringViewUTF8 text, FontResolution* fontResolution, BaseTextVertexGenerator& generator)
	{		
		generator.Setup(text, fontResolution);

		this->fontResolution = fontResolution;

		bottomLeft = { FLT_MAX, FLT_MAX };
		topRight = { FLT_MIN, FLT_MIN };

		Vertex* vertices = new Vertex[generator.GetMaxVertexCount()];
		Vertex* it = vertices;
		float advance;

		while (!generator.IsEnd())
		{
			if (generator.GenerateVertex(it->p1, it->p2, it->uv1, it->uv2, advance))
			{
				bottomLeft.x = std::min({ bottomLeft.x, it->p1.x, it->p2.x });
				bottomLeft.y = std::min({ bottomLeft.y, it->p1.y, it->p2.y });
				topRight.x = std::max({ topRight.x, it->p1.x, it->p2.x });
				topRight.y = std::max({ topRight.y, it->p1.y, it->p2.y });
				++it;
			}
		}

		this->vertexCount = it - vertices;

		vb.AllocateDynamicStorage(
			BufferView(vertices, vertexCount * sizeof(Vertex)),
			Core::GraphicsBufferDynamicStorageHint::DynamicDraw
		);

		delete[] vertices;
	}
}