#pragma once
#include "Graphics4/RenderTask.h"
#include "Graphics4/Dynamic/API_dynamic.h"

namespace Graphics4::Dynamic
{	
	class Line2DRenderTaskBase : public RenderTask, public DynamicImplementation<Line2DRenderTaskBase>
	{
	public:
		Line2DRenderTaskBase();
		Line2DRenderTaskBase(const Line2DRenderTaskBase&) = delete;
		Line2DRenderTaskBase(Line2DRenderTaskBase&& other) noexcept;
		Line2DRenderTaskBase(Graphics4::Dynamic::GraphicsContextBase& graphicsContext);

		Array<Graphics4::Dynamic::SemaphoreBase*> PerformStage(ArrayView<Graphics4::Dynamic::SemaphoreBase*> waitSemaphores, Graphics4::Dynamic::FramebufferBase& renderTarget) override;

		Line2DRenderTaskBase& operator=(const Line2DRenderTaskBase&) = delete;
		Line2DRenderTaskBase& operator=(Line2DRenderTaskBase&& other) noexcept;
	private:
	};
}