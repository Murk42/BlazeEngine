#pragma once
#include "BlazeEngine/BlazeEngine.h"
#include "Graphics3/API/API.h"

namespace Graphics4
{
	struct GraphicsResourceLayout
	{

	};
	struct GraphicsResource
	{
		GraphicsResourceLayout currentLayout;
	};
	struct GraphicsResourceReference
	{
		GraphicsResource* resource;
		GraphicsResourceLayout expectedLayout;
	};
	
	class RenderTask
	{
	public:
		RenderTask(ArrayView<GraphicsResourceReference> references);

		virtual Array<Graphics3::Dynamic::SemaphoreBase*> PerformStage(ArrayView<Graphics3::Dynamic::SemaphoreBase*> waitSemaphores, Graphics3::Dynamic::FramebufferBase& renderTarget) = 0;

		inline ArrayView<GraphicsResourceReference> GetResourceReferences() const { return references; }
	private:
		Array<GraphicsResourceReference> references;
	};
	class RenderStage : public RenderTask
	{
	public:
		RenderStage(String name, ArrayView<GraphicsResourceReference> references);

		inline StringView GetName() const { return name; }
	private:
		String name;		
	};	
	
	namespace Dynamic
	{
		class RenderPipelineBase
		{
		public:
			RenderPipelineBase(const RenderPipelineBase&) = delete;
			RenderPipelineBase(RenderPipelineBase&& other) noexcept;
			template<ImplementationOf<RenderPipelineBase> T>
			RenderPipelineBase(T&& implementation) noexcept;
			RenderPipelineBase(Graphics3::Dynamic::GraphicsContextBase& graphicsContext);
			~RenderPipelineBase();

			inline bool IsImplemented() const { return implementation != nullptr; }
			void Destroy();

			template<ImplementationOf<RenderPipelineBase> T>
			inline T* TryCast();

			template<ImplementationOf<RenderStage> T>
			T* AddRenderStage(T&& stage);
			template<ImplementationOf<RenderStage> T>
			T* AddRenderStageAfter(T&& stage, RenderTask& relative);
			template<ImplementationOf<RenderStage> T>
			T* AddRenderStageBefore(T&& stage, RenderTask& relative);

			template<ImplementationOf<RenderTask> T>
			T* AddRenderTask(T&& task);
			template<ImplementationOf<RenderTask> T>
			T* AddRenderTaskAfter(T&& task, RenderTask& relative);
			template<ImplementationOf<RenderTask> T>
			T* AddRenderTaskBefore(T&& task, RenderTask& relative);

			template<typename T>
			void RemoveStage(RenderStage& stage);

			RenderStage* GetRenderStage(StringView name) const;

			Array<Graphics3::Dynamic::SemaphoreBase*> PerformPipeline(Graphics3::Dynamic::FramebufferBase& renderTarget, ArrayView<Graphics3::Dynamic::SemaphoreBase*> waitSemaphores);
		protected:
			RenderPipelineBase();

			virtual void TransitionGraphicsResource(GraphicsResource* resource, GraphicsResourceLayout source, GraphicsResourceLayout destination);

		private:
			RenderPipelineBase* implementation;

			struct TaskInstance
			{
				RenderTask* task;
				bool isStage;
			};

			Map<StringView, RenderStage*> stageMap;
			List<TaskInstance> tasks;
		};

		template<ImplementationOf<RenderPipelineBase> T>
		inline T* RenderPipelineBase::TryCast()
		{
			if (implementation == nullptr)
				return dynamic_cast<T*>(this);
			return dynamic_cast<T*>(implementation);
		}

		template<ImplementationOf<RenderStage> T>
		T* RenderPipelineBase::AddRenderStage(T&& stage)
		{
			stageMap.Insert(stage.GetName(), &stage);

			T* ptr = DefaultAllocator::Create<T>(std::move(stage));
			TaskInstance instance;
			instance.isStage = true;
			instance.task = ptr;

			tasks.AddBack(std::move(instance));
			return ptr;
		}
		template<ImplementationOf<RenderStage> T>
		T* RenderPipelineBase::AddRenderStageAfter(T&& stage, RenderTask& relative)
		{
			stageMap.Insert(stage.GetName(), &stage);

			for (auto& it = tasks.FirstIterator(); it != tasks.BehindIterator(); ++it)
				if (it->task == &relative)
				{
					T* ptr = DefaultAllocator::Create<T>(std::move(stage));
					TaskInstance instance;
					instance.isStage = true;
					instance.task = ptr;

					tasks.AddAfter(it, std::move(instance));
					return ptr;
				}

			return nullptr;
		}
		template<ImplementationOf<RenderStage> T>
		T* RenderPipelineBase::AddRenderStageBefore(T&& stage, RenderTask& relative)
		{
			stageMap.Insert(stage.GetName(), &stage);

			auto prev = decltype(tasks)::Iterator();
			for (auto it = tasks.FirstIterator();;)
			{
				if (it->task == &relative)
					break;
				else
					prev = it;

				if (it == tasks.BehindIterator())
					return nullptr;
			}

			T* ptr = DefaultAllocator::Create<T>(std::move(stage));
			TaskInstance instance;
			instance.isStage = true;
			instance.task = ptr;

			if (prev.IsNull())
				tasks.AddFront(std::move(instance));
			else
				tasks.AddAfter(prev, std::move(instance));

			return ptr;
		}

		template<ImplementationOf<RenderTask> T>
		T* RenderPipelineBase::AddRenderTask(T&& task)
		{
			T* ptr = DefaultAllocator::Create<T>(std::move(task));
			TaskInstance instance;
			instance.isStage = false;
			instance.task = ptr;

			tasks.AddBack(std::move(instance));
			return ptr;
		}
		template<ImplementationOf<RenderTask> T>
		T* RenderPipelineBase::AddRenderTaskAfter(T&& task, RenderTask& relative)
		{
			for (auto& it = tasks.FirstIterator(); it != tasks.BehindIterator(); ++it)
				if (it->task == &relative)
				{
					T* ptr = DefaultAllocator::Create<T>(std::move(task));
					TaskInstance instance;
					instance.isStage = false;
					instance.task = ptr;

					tasks.AddAfter(it, std::move(instance));
					return ptr;
				}

			return nullptr;
		}
		template<ImplementationOf<RenderTask> T>
		T* RenderPipelineBase::AddRenderTaskBefore(T&& task, RenderTask& relative)
		{
			auto prev = decltype(tasks)::Iterator();
			for (auto it = tasks.FirstIterator();;)
			{
				if (it->task == &relative)
					break;
				else
					prev = it;

				if (it == tasks.BehindIterator())
					return false;
			}

			T* ptr = DefaultAllocator::Create<T>(std::move(task));
			TaskInstance instance;
			instance.isStage = false;
			instance.task = ptr;

			if (prev.IsNull())
				tasks.AddFront(std::move(instance));
			else
				tasks.AddAfter(prev, std::move(instance));

			return ptr;
		}

		template<typename T>
		void RenderPipelineBase::RemoveStage(RenderStage& stage)
		{
			stageMap.Erase(stage.GetName());

			tasks.EraseOne([&](const TaskInstance& instance) {
				return instance.task == (RenderTask*)&stage;
				});
		}		
	}
}