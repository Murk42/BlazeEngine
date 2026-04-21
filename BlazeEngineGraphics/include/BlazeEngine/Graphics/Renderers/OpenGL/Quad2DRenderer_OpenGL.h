#pragma once
#include "BlazeEngine/Graphics/Renderers/Quad2DRenderer.h"
#include "BlazeEngine/Graphics/Core/OpenGL/GraphicsContext_OpenGL.h"
#include "BlazeEngine/Graphics/Renderers/OpenGL/BufferedRendererBase_OpenGL.h"

#include "BlazeEngine/Graphics/Core/OpenGL/OpenGLWrapper/OpenGLVertexArray.h"
#include "BlazeEngine/Graphics/Core/OpenGL/OpenGLWrapper/OpenGLFence.h"

namespace Blaze::Graphics::OpenGL
{
	class BLAZE_API Quad2DRenderer_OpenGL : public Quad2DRenderer, public BufferedRendererBase_OpenGL
	{
	public:
		Quad2DRenderer_OpenGL(GraphicsContext_OpenGL& graphicsContext, uintMem bufferInstanceCount = 64, bool loadDefaultShaders = true);
		~Quad2DRenderer_OpenGL();

		void StartRender(const RenderContext_OpenGL& context) override;
		void EndRender() override;
		void Render(const Quad2DRenderData& data) override;

		using Blaze::Graphics::Quad2DRenderer::Render;
	private:
		Blaze::Graphics::OpenGL::VertexArray va;

		uintMem instanceCount;
		Fence fence;

		void Flush();
		void WaitFence();
	};
}