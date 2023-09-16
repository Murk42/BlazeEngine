#pragma once
#include "BlazeEngine/Graphics/GraphicsLibrary.h"
#include "BlazeEngine/Graphics/Utility/BatchStreamRenderer.h"

namespace Blaze
{
	namespace Graphics
	{
		class BLAZE_API Line3DRenderer
		{
		public:
			struct Vertex
			{
				Vec3f pos1;
				Vec3f pos2;
				Vec4f color;
				float width;
			};			

			Line3DRenderer();
			~Line3DRenderer();

			Result SetProjectionMatrix(Mat4f mat);

			Result Draw(Vec3f pos1, Vec3f pos2, ColorRGBAf color, float width) const;
			Result Draw(ArrayView<Vertex> vertices) const;

			Result SetShaderProgram(GraphicsLibrary::ShaderProgram&&);
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