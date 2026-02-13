#pragma once
#include "BlazeEngine/Graphics/Renderers/Line2DRenderer.h"
#include "BlazeEngine/Graphics/Core/OpenGL/GraphicsContext_OpenGL.h"
#include "BlazeEngine/Graphics/Core/OpenGL/OpenGLWrapper/OpenGLVertexArray.h"
#include "BlazeEngine/Graphics/Renderers/OpenGL/BufferedRendererBase_OpenGL.h"

namespace Blaze::Graphics::OpenGL
{	
	class BLAZE_API Line2DRenderer_OpenGL : public Line2DRenderer, public BufferedRendererBase_OpenGL
	{
	public:
		Line2DRenderer_OpenGL(GraphicsContext_OpenGL& graphicsContext, uintMem bufferInstanceCount = 64, bool loadDefaultShaders = true);
		~Line2DRenderer_OpenGL();

		void StartRender(const RenderContext& context) override;
		void EndRender(const RenderContext& context) override;
		void Render(const Line2DRenderData& data, const RenderContext& context) override;

		GraphicsContextBase& GetGraphicsContext() const override { return graphicsContext; }
	private:
		GraphicsContext_OpenGL& graphicsContext;
		Blaze::Graphics::OpenGL::VertexArray va;

		uintMem instanceCount;
		Fence fence;

		void Flush(const RenderContext& context);
		void WaitFence();
	};
}