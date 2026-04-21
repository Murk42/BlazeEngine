#pragma once
#include "BlazeEngine/Graphics/Core/OpenGL/OpenGLWrapper/OpenGLVertexArray.h"
#include "BlazeEngine/Graphics/Core/OpenGL/OpenGLWrapper/OpenGLFence.h"
#include "BlazeEngine/Graphics/Renderers/OpenGL/BufferedRendererBase_OpenGL.h"
#include "BlazeEngine/UI/Graphics/Renderers/AntialiasedTextRenderer.h"

namespace Blaze::Graphics::OpenGL
{
	class BLAZE_API AntialiasedTextRenderer_OpenGL : public AntialiasedTextRenderer, public BufferedRendererBase_OpenGL
	{
	public:
		AntialiasedTextRenderer_OpenGL(GraphicsContext_OpenGL& graphicsContext, uintMem bufferInstanceCount = 64, bool loadDefaultShaders = true);
		~AntialiasedTextRenderer_OpenGL();

		void StartRender(const RenderContext_OpenGL& context) override;
		void EndRender() override;
		void Render(const TextRectRenderData& data) override;
	private:
		VertexArray va;

		const Texture2D* textures[8]{ };
		uintMem instanceCount;

		Fence fence;

		void Flush();
		void WaitFence();
	};
}