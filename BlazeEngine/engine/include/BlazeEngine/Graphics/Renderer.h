#pragma once
#include "BlazeEngine/Core/EngineCore.h"
#include "BlazeEngine/DataStructures/Color.h"
#include "BlazeEngine/DataStructures/Vector.h"
#include "BlazeEngine/DataStructures/Common.h"
#include "BlazeEngine/Graphics/Mesh.h"

namespace Blaze
{
	class Window;

	enum class RenderMode
	{
		PointIndexed,
		PointArray,
		LineIndexed,
		LineArray,
		TriangleIndexed,
		TriangleArray,
	};

	namespace Renderer
	{
		uint BLAZE_API MaxBoundTextures();

		void BLAZE_API SetViewport(const Vec2i& pos, const Vec2i& size);
		void BLAZE_API SetClearColor(Color);
		void BLAZE_API ClearTarget();
		void BLAZE_API ClearTargetColor();
		void BLAZE_API ClearTargetDepth();
		void BLAZE_API UpdateTarget();
		void BLAZE_API SetTarget(Window& win);

		void BLAZE_API SwapInterval(bool vsync);
		Vec2i BLAZE_API GetViewportPos();
		Vec2i BLAZE_API GetViewportSize();
		float BLAZE_API GetViewportRatio();

		void BLAZE_API UseDepthBuffer(bool use);
		void BLAZE_API UseFaceCulling(bool use);		
		void BLAZE_API UseBlending(bool use);		

		void BLAZE_API RenderTriangles(IndexType indexType, uint triangleCount);
		void BLAZE_API RenderPointArray(uint count, size_t offset);
	}
}