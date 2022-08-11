#pragma once
#include "BlazeEngine/Core/EngineCore.h"
#include "BlazeEngine/Graphics/GraphicsLibrary.h"

namespace Blaze
{
	namespace Graphics
	{
		class BLAZE_API Point2DRenderer
		{
			Core::ShaderProgram program;
			Core::VertexArray va;
			Core::GraphicsBuffer vb;
			void* cache;
			uint batchSize;
			uint batchOffset;

		public:
			Point2DRenderer();
			~Point2DRenderer();

			void SetProjectionMatrix(Mat4f mat);

			void SetBatchMode(uint batchSize);
			void SetImmediateMode();

			void Draw(Vec2f pos1, ColorRGBAf color, float radius);
			void Flush();
		};
	}
}