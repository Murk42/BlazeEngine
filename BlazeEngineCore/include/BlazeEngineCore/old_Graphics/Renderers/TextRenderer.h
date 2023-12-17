#pragma once
#include "BlazeEngineCore/Graphics/GraphicsLibrary.h"
#include "BlazeEngineCore/Graphics/Transform.h"
#include "BlazeEngineCore/Graphics/Utility/BatchStreamRenderer.h"

#include "BlazeEngineCore/Resources/Font/Font.h"
#include "BlazeEngineCore/Math/Matrix.h"

namespace Blaze::Graphics
{		
	class BaseTextVertexGenerator;

	class BLAZE_CORE_API TextGraphicsData
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

		TextGraphicsData();
		TextGraphicsData(TextGraphicsData&&) noexcept;
		TextGraphicsData(StringViewUTF8 text, float fontHeight, FontResolution* fontResolution, ArrayView<ColorRGBAf> colors, BaseTextVertexGenerator& vertexGenerator);
		~TextGraphicsData();

		Result GenerateVertices(StringViewUTF8 text, float fontHeight, FontResolution* fontResolution, ArrayView<ColorRGBAf> colors, BaseTextVertexGenerator& vertexGenerator);

		void SetColors(ArrayView<ColorRGBAf> colors);

		inline Vec2f GetSize() const { return size; }
		inline uint GetVertexCount() const { return vertexCount; }
		inline float GetFontHeight() const { return fontHeight; }
		inline FontResolution* GetFontResolution() const { return fontResolution; }

		inline ArrayView<Vertex> GetVertices() const { return { vertices, vertexCount }; }
	private:
		Vec2f size;

		Vertex* vertices;
		FontResolution* fontResolution;
		float fontHeight;
		uint vertexCount;
	};

	class BLAZE_CORE_API TextRenderer
	{
	public:
		enum class TextRenderingMethod
		{
			Normal,
			SDF,
			LCD
		};
		TextRenderer();
		~TextRenderer();

		void SetProjectionMatrix(Mat4f mat);		
					
		Result Render(Transform2D transform, const TextGraphicsData&) const;
		Result Render(Mat4f matrix, const TextGraphicsData&) const;

		Result SetShaderProgram(GraphicsLibrary::ShaderProgram&&, TextRenderingMethod methodType);
		Result LoadDefaultShaderProgram(TextRenderingMethod methodType) const;
		Result LoadDefaultShaderPrograms() const;
	private:
		mutable GraphicsLibrary::ShaderProgram programNormal;
		mutable GraphicsLibrary::ShaderProgram programSDF;
		mutable GraphicsLibrary::ShaderProgram programLCD;
		mutable GraphicsLibrary::VertexArray va;
		Mat4f proj;		

		mutable BatchStreamRenderer renderer;

		Result CheckProgram(TextRenderingMethod method, GraphicsLibrary::ShaderProgram*& program) const;
		Result GetProgram(TextRenderingMethod method, GraphicsLibrary::ShaderProgram*& program) const;
	};
}