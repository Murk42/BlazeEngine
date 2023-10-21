#pragma once

#if defined(GRAPHICS_OPENGL)

#include "OpenGL/RenderPipeline_OpenGL.h"
#include "OpenGL/PresentRenderStage_OpenGL.h"
#include "OpenGL/ClearRenderStage_OpenGL.h"
#include "OpenGL/TextRenderStage_OpenGL.h"

namespace Graphics4
{
	using RenderPipeline = RenderPipeline_OpenGL;
	using PresentRenderStage = PresentRenderStage_OpenGL;
	using ClearRenderStage = ClearRenderStage_OpenGL;
	using TextRenderStage = TextRenderStage_OpenGL;
}

#else

#include "Dynamic/RenderPipeline.h"
#include "Dynamic/PresentRenderStage.h"
#include "Dynamic/TextRenderStage.h"
#include "Dynamic/ClearRenderStage.h"

namespace Graphics4
{
	using RenderPipeline = Dynamic::RenderPipelineBase;
	using PresentRenderStage = Dynamic::PresentRenderStageBase;
	using ClearRenderStage = Dynamic::ClearRenderStageBase;
	using TextRenderStage = Dynamic::TextRenderStageBase;
}
#endif