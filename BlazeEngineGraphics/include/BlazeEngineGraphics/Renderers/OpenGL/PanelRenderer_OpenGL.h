#pragma once
#include "BlazeEngineGraphics/Renderers/PanelRenderer.h"
#include "BlazeEngineGraphics/RenderStructure/StreamRenderer.h"
#include "BlazeEngineGraphics/Core/OpenGL/GraphicsContext_OpenGL.h"
#include "BlazeEngineGraphics/Core/OpenGL/OpenGLWrapper/OpenGLProgram.h"
#include "BlazeEngineGraphics/Core/OpenGL/OpenGLWrapper/OpenGLVertexArray.h"
#include "BlazeEngineGraphics/Core/OpenGL/OpenGLWrapper/OpenGLGraphicsBuffer.h"

namespace Blaze::Graphics::OpenGL
{
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

		StringView GetRendererName() const override { return "PanelRenderer_OpenGL"; }
	private:		
		GraphicsContext_OpenGL& graphicsContext;
		Blaze::Graphics::OpenGLWrapper::ShaderProgram program;
		Blaze::Graphics::OpenGLWrapper::ImmutableMappedGraphicsBuffer instanceBuffer;
		Blaze::Graphics::OpenGLWrapper::VertexArray va;

		friend class PanelRenderCache_OpenGL;
	};
}