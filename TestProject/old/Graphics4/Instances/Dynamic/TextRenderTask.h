#pragma once
#include "Graphics4/Dynamic/RenderPipeline.h"

namespace Graphics4
{
	namespace Dynamic
	{				
		class TextRenderTaskBase : public RenderStage, public DynamicImplementation<TextRenderTaskBase>
		{					
		public:
			TextRenderTaskBase();
			TextRenderTaskBase(TextRenderTaskBase&& other) noexcept;			
			TextRenderTaskBase(Graphics4::Dynamic::GraphicsContextBase& graphicsContext);

			Array<Graphics4::Dynamic::SemaphoreBase*> PerformStage(ArrayView<Graphics4::Dynamic::SemaphoreBase*> waitSemaphores, Graphics4::Dynamic::FramebufferBase& renderTarget) override;		

			TextRenderTaskBase& operator=(TextRenderTaskBase&& other) noexcept;		
		};
			
	}
}