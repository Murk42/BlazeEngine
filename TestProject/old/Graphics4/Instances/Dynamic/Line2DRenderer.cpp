#include "pch.h"
#include "Line2DRenderer.h"
#include "Graphics4/OpenGL/Line2DRenderer_OpenGL.h"

namespace Graphics4::Dynamic
{
	Line2DRendererBase::Line2DRendererBase(Graphics4::Dynamic::GraphicsContextBase& graphicsContext)
	{
		auto* graphicsContext_OpenGL = graphicsContext.TryCast<Graphics4::OpenGL::GraphicsContext_OpenGL>();
		
		if (graphicsContext_OpenGL != nullptr)
			SetImplementation<Graphics4::OpenGL::Line2DRenderer_OpenGL>(*graphicsContext_OpenGL);
		else
			Debug::Logger::LogError("Blaze Graphics API", "Unsupported implementation");
	}
	bool Line2DRendererBase::CreateTask(const Array<Line2DRenderData>& renderData, Line2DRenderTaskBase& task)
	{
		if (auto impl = GetImplementation())
			return impl->CreateTask(renderData, task);

		return false;
	}
	Line2DRendererBase::Line2DRendererBase()
	{
	}
}