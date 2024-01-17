#pragma once
#include "BlazeEngineGraphics/Core/StreamRenderer.h"

namespace Blaze::Graphics::OpenGL
{
	struct PanelRenderData_OpenGL
	{				
		Vec2f pos;
		Vec2f size;
		float rotation;
		float borderWidth;
		float cornerRadius;
		ColorRGBAf fillColor;
		ColorRGBAf borderColor;
	};

	class PanelRenderer_OpenGL;

	class PanelRenderCache_OpenGL
	{
	public:
		void CreateNew(PanelRenderer_OpenGL& renderer, const Array<PanelRenderData_OpenGL>& renderData);
	private:
		struct Vertex
		{
			uint8 sign;
		};
		struct Instance
		{
			Vec4f fillColor;
			Vec4f borderColor;
			Vec2f p1;
			Vec2f p2;
			Vec2f p3;			
			float borderWidth;
			float cornerRadius;			
		};

		Array<Instance> instances;
		friend class PanelRenderer_OpenGL;
	};

	class PanelRenderer_OpenGL
		: public StreamRenderer
	{
	public:
		PanelRenderer_OpenGL(GraphicsContext_OpenGL& graphicsContext);
		
		void Render(const PanelRenderCache_OpenGL& renderCache, Vec2u targetSize);
		void Render(RenderStream& renderStream, Vec2u targetSize);
	private:		
		Blaze::Graphics::OpenGLWrapper::ShaderProgram program;
		Blaze::Graphics::OpenGLWrapper::ImmutableStaticGraphicsBuffer vertexBuffer;
		Blaze::Graphics::OpenGLWrapper::ImmutableMappedGraphicsBuffer instanceBuffer;
		Blaze::Graphics::OpenGLWrapper::VertexArray va;

		friend class PanelRenderCache_OpenGL;
	};
}