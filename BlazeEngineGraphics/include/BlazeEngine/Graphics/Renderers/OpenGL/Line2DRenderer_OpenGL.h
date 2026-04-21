#pragma once
#include "BlazeEngine/Graphics/Renderers/Line2DRenderer.h"
#include "BlazeEngine/Graphics/Core/OpenGL/GraphicsContext_OpenGL.h"
#include "BlazeEngine/Graphics/Core/OpenGL/OpenGLWrapper/OpenGLVertexArray.h"
#include "BlazeEngine/Graphics/Renderers/OpenGL/BufferedRendererBase_OpenGL.h"
#include "BlazeEngine/Graphics/Core/OpenGL/OpenGLWrapper/OpenGLVertexArray.h"
#include "BlazeEngine/Graphics/Core/OpenGL/OpenGLWrapper/OpenGLFence.h"

namespace Blaze::Graphics::OpenGL
{	
	class BLAZE_API Line2DRenderer_OpenGL : public Line2DRenderer, public BufferedRendererBase_OpenGL
	{
	public:
		Line2DRenderer_OpenGL(GraphicsContext_OpenGL& graphicsContext, uintMem bufferInstanceCount = 64, bool loadDefaultShaders = true);
		~Line2DRenderer_OpenGL();

		void StartRender(const RenderContext_OpenGL& context) override;
		void EndRender() override;
		void Render(const Line2DRenderData& data) override;
	private:
		Blaze::Graphics::OpenGL::VertexArray va;

		uintMem instanceCount;
		Fence fence;

		void Flush();
		void WaitFence();
	};
}