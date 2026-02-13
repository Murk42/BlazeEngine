#pragma once
#include "BlazeEngine/Graphics/Core/GraphicsAPI_OpenGL.h"
#include "BlazeEngine/Graphics/Renderers/OpenGL/BufferedRendererBase_OpenGL.h"
#include "BlazeEngine/UI/Graphics/Renderers/AntialiasedTextRenderer.h"

namespace Blaze::Graphics::OpenGL
{
	class BLAZE_API AntialiasedTextRenderer_OpenGL : public AntialiasedTextRenderer, public BufferedRendererBase_OpenGL
	{
	public:
		AntialiasedTextRenderer_OpenGL(GraphicsContext_OpenGL& graphicsContext, uintMem bufferInstanceCount = 64, bool loadDefaultShaders = true);
		~AntialiasedTextRenderer_OpenGL();

		void StartRender(const RenderContext& context) override;
		void EndRender(const RenderContext& context) override;
		void Render(const TextRectRenderData& data, const RenderContext& context) override;

		GraphicsContextBase& GetGraphicsContext() const { return graphicsContext; }
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