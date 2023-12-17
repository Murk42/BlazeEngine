#pragma once
#include "Graphics4/OpenGL/API_OpenGL.h"
#include "Graphics4/Dynamic/Line2DRenderer.h"
#include "OpenGLWrapper/OpenGLContext.h"

namespace Graphics4::OpenGL
{
	class Line2DRenderTask_OpenGL;

	class Line2DRenderer_OpenGL : public Dynamic::Line2DRendererBase
	{
	public:
		Line2DRenderer_OpenGL(Graphics4::OpenGL::GraphicsContext_OpenGL& graphicsContext);

		bool CreateTask(const Array<Line2DRenderData>& renderData, Dynamic::Line2DRenderTaskBase& task) override;
		bool CreateTask(const Array<Line2DRenderData>& renderData, Line2DRenderTask_OpenGL& task);		
	private:
		struct Vertex
		{
			Vec2f pos;	
			Vec2f pos1;
			Vec2f pos2;
			Vec4f color;	
			float width;
		};
		struct RenderCache
		{			
			Array<Vertex> vertices;			
		};	

		Graphics4::OpenGL::GraphicsContext_OpenGL& graphicsContext;
		OpenGLWrapper::ShaderProgram program;
		OpenGLWrapper::ImmutableMappedGraphicsBuffer vb;
		OpenGLWrapper::VertexArray va;

		void Render(const RenderCache& renderCache, Graphics4::OpenGL::Framebuffer_OpenGL& renderTarget);

		friend class Line2DRenderTask_OpenGL;
	};
}