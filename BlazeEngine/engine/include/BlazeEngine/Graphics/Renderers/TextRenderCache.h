#pragma once
#include "BlazeEngine/Graphics/GraphicsLibrary.h"
#include <span>

namespace Blaze
{
	class FontResolution;
}

namespace Blaze::Graphics
{
	class BaseTextVertexGenerator;

	class BLAZE_API TextRenderCache
	{
	public:
		struct Vertex
		{
			Vec2f p1;
			Vec2f p2;
			Vec2f uv1;
			Vec2f uv2;
			Vec4f color;
		};
				
		TextRenderCache();
		TextRenderCache(TextRenderCache&&) noexcept;
		TextRenderCache(StringViewUTF8 text, float fontHeight, FontResolution* fontResolution, std::span<const ColorRGBAf> colors);
		~TextRenderCache();

		Result GenerateVertices(StringViewUTF8 text, float fontHeight, FontResolution* fontResolution, std::span<const ColorRGBAf> colors);
		Result GenerateVertices(StringViewUTF8 text, float fontHeight, FontResolution* fontResolution, std::span<const ColorRGBAf> colors, BaseTextVertexGenerator& vertexGenerator);

		void SetColors(std::span<ColorRGBAf> colors);
		void SetColor(ColorRGBAf color);

		inline Vec2f GetBottomLeft() const { return bottomLeft; }
		inline Vec2f GetTopRight() const { return topRight; }
		inline uint GetVertexCount() const { return vertexCount; }
		inline FontResolution* GetFontResolution() const { return fontResolution; }

		inline std::span<const Vertex> GetVertices() const { return { vertices, vertexCount }; }
	private:		
		Vec2f bottomLeft;
		Vec2f topRight;

		Vertex* vertices;
		FontResolution* fontResolution;
		uint vertexCount;
	};
}