#pragma once
#include "BlazeEngine/Graphics/Renderers/TexturedRectRenderer.h"
#include "BlazeEngine/Graphics/Renderers/OpenGL/BufferedRendererBase_OpenGL.h"
#include "BlazeEngine/Graphics/Core/OpenGL/OpenGLWrapper/OpenGLVertexArray.h"
#include "BlazeEngine/Graphics/Core/OpenGL/OpenGLWrapper/OpenGLFence.h"

namespace Blaze::Graphics::OpenGL
{
	class BLAZE_API TexturedRectRenderer_OpenGL : public TexturedRectRenderer, public BufferedRendererBase_OpenGL
	{
	public:
		TexturedRectRenderer_OpenGL(GraphicsContext_OpenGL& graphicsContext, uintMem bufferInstanceCount = 64, bool loadDefaultShaders = true);
		~TexturedRectRenderer_OpenGL();

		void StartRender(const RenderContext_OpenGL& context) override;
		void EndRender() override;
		void Render(const TexturedRectRenderData& data) override;
	private:
		VertexArray va;

		const Texture2D* textures[8]{ };
		uintMem instanceCount;

		Fence fence;

		void Flush();
		void WaitFence();
	};
}