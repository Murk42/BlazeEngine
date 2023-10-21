#pragma once
#include "Graphics4/Dynamic/RenderPipeline.h"
#include "Graphics3/OpenGL/API_OpenGL.h"

namespace Graphics4::OpenGL
{
	class RenderPipeline_OpenGL : public Dynamic::RenderPipelineBase
	{
	public:
		RenderPipeline_OpenGL(Graphics3::OpenGL::GraphicsContext_OpenGL& graphicsContext);

		Array<Graphics3::Semaphore*> PerformPipeline(Graphics3::Framebuffer& renderTarget, ArrayView<Graphics3::Semaphore*> waitSemaphores);
		void TransitionGraphicsResource(GraphicsResource* resource, GraphicsResourceLayout source, GraphicsResourceLayout destination) override;
	private:
	};	
}