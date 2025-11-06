#pragma once
#include "BlazeEngine/Core/Common/Color.h"
#include "BlazeEngine/Graphics/Core/OpenGL/GraphicsContext_OpenGL.h"
#include "BlazeEngine/Graphics/Core/OpenGL/OpenGLWrapper/OpenGLVertexArray.h"
#include "BlazeEngine/Graphics/Core/OpenGL/OpenGLWrapper/OpenGLProgram.h"
#include "BlazeEngine/Graphics/Core/OpenGL/OpenGLWrapper/OpenGLGraphicsBuffer.h"

namespace Blaze::Graphics::OpenGL
{
	struct Line2DRenderData_OpenGL
	{
		Vec2f p1, p2;
		ColorRGBAf color;
		float width;
	};
	struct Line2DRenderCache_OpenGL
	{
		struct Vertex
		{
			Vec2f pos;
			Vec2f pos1;
			Vec2f pos2;
			Vec4f color;
			float width;
		};

		Array<Vertex> vertices;
	};

	class BLAZE_API Line2DRenderer_OpenGL
	{
	public:
		Line2DRenderer_OpenGL(GraphicsContext_OpenGL& graphicsContext);

		void CreateCache(const Array<Line2DRenderData_OpenGL>& renderData, Line2DRenderCache_OpenGL& task);
		void Render(const Line2DRenderCache_OpenGL& renderCache, Vec2u targetSize);
	private:
		GraphicsContext_OpenGL& graphicsContext;
		Blaze::Graphics::OpenGL::ShaderProgram program;
		Blaze::Graphics::OpenGL::ImmutableMappedGraphicsBuffer vb;
		Blaze::Graphics::OpenGL::VertexArray va;

		friend struct Line2DRenderCache_OpenGL;
	};
}