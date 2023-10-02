#pragma once
#include "BlazeEngine/Graphics/GraphicsLibrary.h"
#include "BlazeEngine/Graphics/Utility/BatchStreamRenderer.h"

namespace Blaze
{
	namespace Graphics
	{
		class BLAZE_API Line2DRenderer
		{
		public:
			struct Vertex
			{
				Vec2f pos1;
				Vec2f pos2;
				Vec4f color;
				float width;
			};

			Line2DRenderer();
			~Line2DRenderer();

			Result SetProjectionMatrix(Mat4f mat);			

			Result Draw(Vec2f pos1, Vec2f pos2, ColorRGBAf color, float width) const;
			Result Draw(ArrayView<Vertex> vertices) const;

			Result SetShaders(std::initializer_list<GraphicsLibrary::Shader*> shaders);
			Result LoadDefaultShaders() const;
		private:
			mutable GraphicsLibrary::ShaderProgram program;
			mutable GraphicsLibrary::VertexArray va;
			Mat4f proj;

			BatchStreamRenderer renderer;

			Result CheckProgram() const;
		};
	}
}