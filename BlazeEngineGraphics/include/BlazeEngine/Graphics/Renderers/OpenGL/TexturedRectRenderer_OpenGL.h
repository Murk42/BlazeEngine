#pragma once
#include "BlazeEngine/Graphics/Core/GraphicsAPI_OpenGL.h"
#include "BlazeEngine/Graphics/Renderers/TexturedRectRenderer.h"
#include "BlazeEngine/Graphics/Renderers/OpenGL/BufferedRendererBase_OpenGL.h"

namespace Blaze::Graphics::OpenGL
{
	class BLAZE_API TexturedRectRenderer_OpenGL : public TexturedRectRenderer, public BufferedRendererBase_OpenGL
	{
	public:
		TexturedRectRenderer_OpenGL(GraphicsContext_OpenGL& graphicsContext, uintMem bufferInstanceCount = 64, bool loadDefaultShaders = true);
		~TexturedRectRenderer_OpenGL();

		void StartRender(const RenderContext& context) override;
		void EndRender(const RenderContext& context) override;
		void Render(const TexturedRectRenderData& data, const RenderContext& context) override;
	private:
		GraphicsContext_OpenGL& graphicsContext;
		VertexArray va;

		const Texture2D* textures[8]{ };
		uintMem instanceCount;

		Fence fence;

		void Flush(const RenderContext& context);
		void WaitFence();
	};
}