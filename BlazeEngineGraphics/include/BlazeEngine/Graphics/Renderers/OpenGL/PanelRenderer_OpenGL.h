#pragma once
#include "BlazeEngine/Graphics/Core/GraphicsAPI_OpenGL.h"
#include "BlazeEngine/Graphics/Renderers/PanelRenderer.h"
#include "BlazeEngine/Graphics/Renderers/OpenGL/BufferedRendererBase_OpenGL.h"

namespace Blaze::Graphics::OpenGL
{
	class PanelRenderer_OpenGL : public PanelRenderer, public BufferedRendererBase_OpenGL
	{
	public:
		PanelRenderer_OpenGL(GraphicsContext_OpenGL& graphicsContext, uintMem bufferInstanceCount = 128, bool loadDefaultShaders = true);
		~PanelRenderer_OpenGL();

		void StartRender(const RenderContext& context) override;
		void EndRender(const RenderContext& context) override;
		void Render(const PanelRenderData& data, const RenderContext& context) override;
	private:
		GraphicsContext_OpenGL& graphicsContext;
		VertexArray va;
		Fence fence;

		uintMem instanceCount;

		void Flush(const RenderContext& context);
		void WaitFence();
	};
}