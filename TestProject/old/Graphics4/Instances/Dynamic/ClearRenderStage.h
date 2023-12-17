#pragma once
#include "Graphics4/Dynamic/RenderStage.h"

namespace Graphics4::Dynamic
{
	class ClearRenderStageBase : public RenderStage, public DynamicImplementation<ClearRenderStageBase>
	{
	public:
		ClearRenderStageBase(ClearRenderStageBase&& other) noexcept;
		ClearRenderStageBase(Graphics4::Dynamic::RenderWindowBase& window);		

		Array<Graphics4::Dynamic::SemaphoreBase*> PerformStage(ArrayView<Graphics4::Dynamic::SemaphoreBase*> waitSemaphores, Graphics4::Dynamic::FramebufferBase& renderTarget) override;		

		ClearRenderStageBase& operator=(ClearRenderStageBase&& other) noexcept;
	protected:
		ClearRenderStageBase();
	};	
}