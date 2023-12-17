#pragma once
#include "BlazeEngineCore/Graphics/GraphicsLibrary.h"
#include "BlazeEngineCore/Graphics/Utility/BatchStreamRenderer.h"

namespace Blaze::Graphics
{
	class BLAZE_CORE_API Circle2DRenderer
	{
	public:
		struct Vertex
		{
			Mat4f matrix;
			Vec4f outerColor;			
			Vec4f innerColor;
			float innerRadius;
		};

		Circle2DRenderer();
		~Circle2DRenderer();
	
		Result SetProjectionMatrix(Mat4f mat);
		
		Result Draw(Vec2f center, float outerRadius, float innerRadius, ColorRGBAf outerColor, ColorRGBAf innerColor) const;
		Result Draw(Mat4f matrix, float outerRadius, float innerRadius, ColorRGBAf outerColor, ColorRGBAf innerColor) const;
		Result Draw(ArrayView<Vertex> vertices) const;

		Result SetShaderProgram(GraphicsLibrary::ShaderProgram&& program);
		Result LoadDefaultShaderProgram() const;
	private:
		mutable GraphicsLibrary::ShaderProgram program;
		mutable GraphicsLibrary::VertexArray va;
		Mat4f proj;

		mutable BatchStreamRenderer renderer;

		Result CheckProgram() const;
	};
}