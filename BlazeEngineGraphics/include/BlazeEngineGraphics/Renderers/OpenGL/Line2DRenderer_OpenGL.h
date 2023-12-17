#pragma once
#include "BlazeEngineGraphics\OpenGL/Core/GraphicsContext_OpenGL.h"
#include "BlazeEngineGraphics\OpenGLWrapper/OpenGLContext.h"

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

	class BLAZE_GRAPHICS_API Line2DRenderer_OpenGL
	{
	public:
		Line2DRenderer_OpenGL(GraphicsContext_OpenGL& graphicsContext);
		
		void CreateCache(const Array<Line2DRenderData_OpenGL>& renderData, Line2DRenderCache_OpenGL& task);
		void Render(const Line2DRenderCache_OpenGL& renderCache, Vec2u targetSize);
	private:		
		Blaze::Graphics::OpenGLWrapper::ShaderProgram program;
		Blaze::Graphics::OpenGLWrapper::ImmutableMappedGraphicsBuffer vb;
		Blaze::Graphics::OpenGLWrapper::VertexArray va;

		friend class Line2DRenderCache_OpenGL;
	};
}