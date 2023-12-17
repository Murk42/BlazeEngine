#pragma once
#include "BlazeEngineCore/Graphics/GraphicsLibrary.h"
#include "BlazeEngineCore/Graphics/Utility/BatchStreamRenderer.h"
#include "BlazeEngineCore/Graphics/Transform.h"

namespace Blaze
{
	namespace Graphics
	{
		struct ImageGraphicsData
		{
			Vec2f size = Vec2f(100, 100);
			GraphicsLibrary::Texture2D* texture = nullptr;
			Vec2f uvs[4] = { Vec2f(0, 0), Vec2f(1, 0), Vec2f(0, 1), Vec2f(1, 0) };
			ColorRGBAf mask = 0xffffffff;
			float blendFactor = 0.0f;
		};

		class BLAZE_CORE_API ImageRenderer
		{
		public:
			struct Vertex
			{
				Mat4f matrix;				
				Vec2f uvs[4];
				Vec2f size;
				ColorRGBAf mask;
				float blend;
			};

			ImageRenderer();
			~ImageRenderer();

			Result SetProjectionMatrix(Mat4f mat);

			Result Render(Transform2D transform, const ImageGraphicsData& data) const;
			Result Render(Mat4f matrix, const ImageGraphicsData& data) const;

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