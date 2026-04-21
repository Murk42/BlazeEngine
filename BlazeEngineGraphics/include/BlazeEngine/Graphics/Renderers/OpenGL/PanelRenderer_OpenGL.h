#pragma once
#include "BlazeEngine/Graphics/Renderers/PanelRenderer.h"
#include "BlazeEngine/Graphics/Renderers/OpenGL/BufferedRendererBase_OpenGL.h"
#include "BlazeEngine/Graphics/Core/OpenGL/OpenGLWrapper/OpenGLVertexArray.h"
#include "BlazeEngine/Graphics/Core/OpenGL/OpenGLWrapper/OpenGLFence.h"

namespace Blaze::Graphics::OpenGL
{
	class PanelRenderer_OpenGL : public PanelRenderer, public BufferedRendererBase_OpenGL
	{
	public:
		PanelRenderer_OpenGL(GraphicsContext_OpenGL& graphicsContext, uintMem bufferInstanceCount = 128, bool loadDefaultShaders = true);
		~PanelRenderer_OpenGL();

		void StartRender(const RenderContext_OpenGL& context) override;
		void EndRender() override;
		void Render(const PanelRenderData& data) override;
	private:
		VertexArray va;
		Fence fence;

		uintMem instanceCount;

		void Flush();
		void WaitFence();
	};
}