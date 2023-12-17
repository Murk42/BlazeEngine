#pragma once
#include "BlazeEngineCore/Graphics/GraphicsLibrary.h"
#include "BlazeEngineCore/Graphics/Utility/BatchStreamRenderer.h"

namespace Blaze
{
	namespace Graphics
	{
		class BLAZE_CORE_API Point2DRenderer
		{
		public:
			struct Vertex
			{
				Vec2f pos;
				Vec4f color;
				float radius;
			};

			Point2DRenderer();
			~Point2DRenderer();

			Result SetProjectionMatrix(Mat4f mat);

			Result Draw(Vec2f pos, ColorRGBAf color, float radius) const;
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
}