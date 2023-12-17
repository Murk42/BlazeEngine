#pragma once

#if defined(GRAPHICS_OPENGL)

#include "OpenGL/RenderPipeline_OpenGL.h"
#include "OpenGL/ClearRenderStage_OpenGL.h"
#include "OpenGL/TextRenderTask_OpenGL.h"
#include "OpenGL/TextRenderer_OpenGL.h"
#include "OpenGL/Line2DRenderTask_OpenGL.h"
#include "OpenGL/Line2DRenderer_OpenGL.h"

namespace Graphics4
{
	using RenderPipeline = OpenGL::RenderPipeline_OpenGL;	
	using ClearRenderStage = OpenGL::ClearRenderStage_OpenGL;
	using TextRenderTask = OpenGL::TextRenderTask_OpenGL;
	using TextRenderer = OpenGL::TextRenderer_OpenGL;
	using Line2DRenderTask = OpenGL::Line2DRenderTask_OpenGL;
	using Line2DRenderer = OpenGL::Line2DRenderer_OpenGL;
}

#else

#include "Dynamic/TextRenderTask.h"
#include "Dynamic/TextRenderer.h"
#include "Dynamic/ClearRenderStage.h"
#include "Dynamic/Line2DRenderTask.h"
#include "Dynamic/Line2DRenderer.h"

namespace Graphics4
{	
	using ClearRenderStage = Dynamic::ClearRenderStageBase;
	using TextRenderTask = Dynamic::TextRenderTaskBase;
	using TextRenderer = Dynamic::TextRendererBase;
	using Line2DRenderTask = Dynamic::Line2DRenderTaskBase;
	using Line2DRenderer = Dynamic::Line2DRendererBase;
}
#endif