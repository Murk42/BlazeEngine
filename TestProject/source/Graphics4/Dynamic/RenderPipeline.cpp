#include "pch.h"
#include "RenderPipeline.h"
#include "Graphics4/OpenGL/RenderPipeline_OpenGL.h"

namespace Graphics4
{
	RenderTask::RenderTask(ArrayView<GraphicsResourceReference> references)
		: references(references)
	{
	}
	RenderStage::RenderStage(String name, ArrayView<GraphicsResourceReference> references)
		: name(name), RenderTask(references)
	{
	}
}

namespace Graphics4::Dynamic
{
	RenderPipelineBase::RenderPipelineBase(RenderPipelineBase&& other) noexcept
		: implementation(other.implementation), stageMap(std::move(other.stageMap)), tasks(std::move(other.tasks))
	{
		other.implementation = nullptr;
	}
	RenderPipelineBase::RenderPipelineBase(Graphics3::Dynamic::GraphicsContextBase& graphicsContext)
		: implementation(this)
	{		
		auto graphicsContext_OpenGL = graphicsContext.TryCast<Graphics3::OpenGL::GraphicsContext_OpenGL>();
		
		if (graphicsContext_OpenGL != nullptr)		
			implementation = DefaultAllocator::Create<OpenGL::RenderPipeline_OpenGL>(*graphicsContext_OpenGL);
		else
			Debug::Logger::LogError("Blaze graphics API", "Unsupported graphics context implementation.");		
	}
	RenderPipelineBase::~RenderPipelineBase()
	{		
		Destroy();
	}

	void RenderPipelineBase::Destroy()
	{
		if (implementation != nullptr)
		{
			if (implementation != this)			
				DefaultAllocator::Destroy(implementation);			

			implementation = nullptr;
		}

		tasks.Clear();
		stageMap.Clear();
	}

	RenderStage* RenderPipelineBase::GetRenderStage(StringView name) const
	{
		auto it = stageMap.Find(name);
		if (it.IsNull())
			return nullptr;
		return it->value;
	}

	Array<Graphics3::Dynamic::SemaphoreBase*> RenderPipelineBase::PerformPipeline(Graphics3::Dynamic::FramebufferBase& renderTarget, ArrayView<Graphics3::Dynamic::SemaphoreBase*> waitSemaphores)
	{
		Array<Graphics3::Dynamic::SemaphoreBase*> semaphores;		
		
		tasks.EraseAll([&](TaskInstance& instance) {
			RenderTask& task = *instance.task;

			auto resources = task.GetResourceReferences();
			for (auto& ref : resources)
				TransitionGraphicsResource(ref.resource, ref.resource->currentLayout, ref.expectedLayout);

			semaphores = task.PerformStage(waitSemaphores, renderTarget);

			return !instance.isStage;
			});

		return semaphores;
	}

	void RenderPipelineBase::TransitionGraphicsResource(GraphicsResource* resource, GraphicsResourceLayout source, GraphicsResourceLayout destination)
	{
		if (implementation == nullptr)
			return;
	}

	RenderPipelineBase::RenderPipelineBase()
		: implementation(nullptr)
	{

	}	
}