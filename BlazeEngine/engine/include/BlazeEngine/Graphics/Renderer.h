#pragma once
#include "BlazeEngine/Core/EngineCore.h"
#include "BlazeEngine/DataStructures/Color.h"
#include "BlazeEngine/DataStructures/Vector.h"
#include "BlazeEngine/Graphics/Material.h"
#include "BlazeEngine/DataStructures/Common.h"
#include "BlazeEngine/Graphics/ShaderProgram.h"
#include "BlazeEngine/Graphics/Mesh.h"
#include "BlazeEngine/Graphics/VertexLayout.h"

namespace Blaze
{
	class Window;

	namespace Renderer
	{
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
		
		void BLAZE_API RenderPointArray(const ShaderProgram& material, const VertexLayout& vl, uint count = 0, size_t offset = 0);
		void BLAZE_API RenderPointArray(const ShaderProgram& material, const Mesh& mesh, uint count = 0, size_t offset = 0);
		void BLAZE_API RenderPointArray(BaseMaterial& material, const VertexLayout& vl, uint count = 0, size_t offset = 0);
		void BLAZE_API RenderPointArray(BaseMaterial& material, const Mesh& mesh, uint count = 0, size_t offset = 0);
		
		void BLAZE_API RenderPoints(const ShaderProgram& sp, const VertexLayout& vl, uint pointCount = 0, size_t offset = 0);		
		void BLAZE_API RenderPoints(const ShaderProgram& sp, const Mesh& mesh, uint pointCount = 0, size_t offset = 0);		
		void BLAZE_API RenderPoints(BaseMaterial& material, const VertexLayout& vl, uint pointCount = 0, size_t offset = 0);		
		void BLAZE_API RenderPoints(BaseMaterial& material, const Mesh& mesh, uint pointCount = 0, size_t offset = 0);
		
		void BLAZE_API RenderTriangles(ShaderProgram& sp, VertexLayout& vl, uint triangleCount = 0, size_t offset = 0);		
		void BLAZE_API RenderTriangles(ShaderProgram& sp, Mesh& mesh, uint triangleCount = 0, size_t offset = 0);		
		void BLAZE_API RenderTriangles(BaseMaterial& material, Mesh& mesh, uint triangleCount = 0, size_t offset = 0);		
	}
}