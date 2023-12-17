#pragma once
#include "Graphics4/Dynamic/TextRenderer.h"
#include "Graphics4/OpenGL/API_OpenGL.h"
#include "OpenGLWrapper/OpenGLContext.h"

namespace Graphics4::OpenGL
{
	class TextRenderTask_OpenGL;

	class TextRenderer_OpenGL : public Dynamic::TextRendererBase
	{
	public:
		TextRenderer_OpenGL(Graphics4::OpenGL::GraphicsContext_OpenGL& graphicsContext);	

		bool CreateTask(const TextRenderData& renderData, Dynamic::TextRenderTaskBase& task) override;
		bool CreateTask(const TextRenderData& renderData, TextRenderTask_OpenGL& task);		
	private:
		struct Vertex
		{
			Vec2f pos;
			Vec2f uv;
			Vec4f color;
		};
		struct TextRenderCache
		{
			Vec2f size;
			Array<Vertex> vertices;
			TextTransformData transform;
			OpenGLWrapper::Texture2D* atlas;
		};

		Graphics4::OpenGL::GraphicsContext_OpenGL& graphicsContext;
		OpenGLWrapper::ShaderProgram program;
		OpenGLWrapper::ImmutableMappedGraphicsBuffer vb;
		OpenGLWrapper::VertexArray va;				

		void Render(const TextRenderCache& renderCache, Graphics4::OpenGL::Framebuffer_OpenGL& renderTarget);

		friend class TextRenderTask_OpenGL;
	};
}