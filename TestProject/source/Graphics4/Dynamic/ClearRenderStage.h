#pragma once
#include "Graphics4/Dynamic/RenderPipeline.h"

namespace Graphics4::Dynamic
{
	class ClearRenderStageBase : public RenderStage, public DynamicImplementation<ClearRenderStageBase>
	{
	public:
		ClearRenderStageBase(ClearRenderStageBase&& other) noexcept;
		ClearRenderStageBase(Graphics3::Dynamic::RenderWindowBase& window);		

		Array<Graphics3::Dynamic::SemaphoreBase*> PerformStage(ArrayView<Graphics3::Dynamic::SemaphoreBase*> waitSemaphores, Graphics3::Dynamic::FramebufferBase& renderTarget) override;		

		ClearRenderStageBase& operator=(ClearRenderStageBase&& other) noexcept;
	protected:
		ClearRenderStageBase();
	};	
}