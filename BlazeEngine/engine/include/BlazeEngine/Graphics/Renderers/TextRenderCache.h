#pragma once
#include "BlazeEngine/Core/EngineCore.h"
#include "BlazeEngine/Graphics/GraphicsLibrary.h"

namespace Blaze
{
	class FontResolution;
}

namespace Blaze::Graphics
{
	class BaseTextVertexGenerator;

	class BLAZE_API TextRenderCache
	{
		Core::GraphicsBuffer vb;
		Core::VertexArray va;

		Vec2f bottomLeft;
		Vec2f topRight;

		FontResolution* fontResolution;

		uint vertexCount;
	public:
		struct Vertex
		{
			Vec2f p1;
			Vec2f p2;
			Vec2f uv1;
			Vec2f uv2;
		};

		TextRenderCache();
		TextRenderCache(TextRenderCache&&) noexcept;
		TextRenderCache(StringViewUTF8 text, FontResolution* fontResolution);

		void GenerateVertices(StringViewUTF8 text, FontResolution* fontResolution);
		void GenerateVertices(StringViewUTF8 text, FontResolution* fontResolution, BaseTextVertexGenerator& vertexGenerator);		

		inline Vec2f GetBottomLeft() const { return bottomLeft; }
		inline Vec2f GetTopRight() const { return topRight; }
		inline uint GetVertexCount() const { return vertexCount; }
		inline FontResolution* GetFontResolution() const { return fontResolution; }

		inline Core::VertexArray& GetVertexArray() { return va; }
		inline const Core::VertexArray& GetVertexArray() const { return va; }
	};
}