#pragma once
#include "Graphics4/Dynamic/RenderPipeline.h"

namespace Graphics4::OpenGL
{
	class GraphicsContext_OpenGL;	

	class RenderPipelineContext_OpenGL : public Dynamic::RenderPipelineContextBase
	{
	public:
		RenderPipelineContext_OpenGL();
	};

	class RenderPipeline_OpenGL : public Dynamic::RenderPipelineBase
	{
	public:
		RenderPipeline_OpenGL(GraphicsContext_OpenGL& graphicsContext);

		void PerformPipeline(Dynamic::RenderPipelineContextBase& renderContext) override;		
		void PerformPipeline(Dynamic::RenderPipelineContextBase& renderContext) override;
	private:
	};
}