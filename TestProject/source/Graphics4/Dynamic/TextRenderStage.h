#pragma once
#include "Graphics4/Dynamic/RenderPipeline.h"

namespace Graphics4
{
	namespace Dynamic
	{				
		class TextRenderStageBase : public RenderStage, public DynamicImplementation<TextRenderStageBase>
		{					
		public:
			TextRenderStageBase(TextRenderStageBase&& other) noexcept;			
			TextRenderStageBase(Graphics3::Dynamic::GraphicsContextBase& graphicsContext);

			Array<Graphics3::Dynamic::SemaphoreBase*> PerformStage(ArrayView<Graphics3::Dynamic::SemaphoreBase*> waitSemaphores, Graphics3::Dynamic::FramebufferBase& renderTarget) override;		

			TextRenderStageBase& operator=(TextRenderStageBase&& other) noexcept;
		protected:
			TextRenderStageBase();
		};
			
	}
}