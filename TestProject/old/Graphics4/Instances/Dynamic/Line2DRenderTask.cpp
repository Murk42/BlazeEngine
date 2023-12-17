#include "pch.h"
#include "Line2DRenderTask.h"
#include "Graphics4/OpenGL/Line2DRenderTask_OpenGL.h"

namespace Graphics4::Dynamic
{
	Line2DRenderTaskBase::Line2DRenderTaskBase()
		: RenderTask({ })
	{
	}
	Line2DRenderTaskBase::Line2DRenderTaskBase(Line2DRenderTaskBase&& other) noexcept
		: RenderTask(std::move(other)), DynamicImplementation(std::move(other))
	{
	}
	Line2DRenderTaskBase::Line2DRenderTaskBase(Graphics4::Dynamic::GraphicsContextBase& graphicsContext)
		: RenderTask({ })
	{
		auto* graphicsContext_OpenGL = graphicsContext.TryCast<Graphics4::OpenGL::GraphicsContext_OpenGL>();

		if (graphicsContext_OpenGL != nullptr)
			SetImplementation<Graphics4::OpenGL::Line2DRenderTask_OpenGL>();
		else
			Debug::Logger::LogError("Blaze Graphics API", "Unsupported implementation");
	}

	Array<Graphics4::Dynamic::SemaphoreBase*> Line2DRenderTaskBase::PerformStage(ArrayView<Graphics4::Dynamic::SemaphoreBase*> waitSemaphores, Graphics4::Dynamic::FramebufferBase& renderTarget)
	{
		if (auto implementation = GetImplementation())
			implementation->PerformStage(waitSemaphores, renderTarget);

		return { };
	}

	Line2DRenderTaskBase& Line2DRenderTaskBase::operator=(Line2DRenderTaskBase&& other) noexcept
	{
		RenderTask::operator=(std::move(other));
		DynamicImplementation::operator=(std::move(other));
		
		return *this;
	}
}