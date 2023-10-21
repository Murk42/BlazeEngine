#pragma once
#include "Graphics4/Dynamic/RenderPipeline.h"

namespace Graphics4
{
	namespace Dynamic
	{
		class PresentRenderStageBase : public RenderStage
		{
		public:
			PresentRenderStageBase(PresentRenderStageBase&& other) noexcept;			
			PresentRenderStageBase(Graphics3::Dynamic::RenderWindowBase& window);
			~PresentRenderStageBase();

			inline bool IsImplemented() const { return implementation != nullptr; }
			void Destroy();

			template<ImplementationOf<PresentRenderStageBase> T>
			inline T* TryCast();

			Array<Graphics3::Dynamic::SemaphoreBase*> PerformStage(ArrayView<Graphics3::Dynamic::SemaphoreBase*> waitSemaphores, Graphics3::Dynamic::FramebufferBase& renderTarget) override;
		protected:
			PresentRenderStageBase();
		private:
			PresentRenderStageBase* implementation;
		};
		
		template<ImplementationOf<PresentRenderStageBase> T>
		inline T* PresentRenderStageBase::TryCast()
		{
			if (implementation == nullptr)
				return dynamic_cast<T*>(this);
			return dynamic_cast<T*>(implementation);
		}
	}
}