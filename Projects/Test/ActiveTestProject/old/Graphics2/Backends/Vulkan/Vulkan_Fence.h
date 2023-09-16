#pragma once
#include "vulkan/vulkan.h"
#include "Vulkan_RenderContext.h"
#include "../../Fence.h"

namespace Graphics2
{
	class Vulkan_RenderContext;

	class Vulkan_Fence : public Fence
	{
	public:
		using ContextType = Vulkan_RenderContext;

		Vulkan_RenderContext* renderContext;
		VkFence fence;

		Vulkan_Fence(Vulkan_RenderContext* renderContext);
		~Vulkan_Fence() override;

		void Create(FenceSignalState state) override;
		void Destroy() override;

		void Wait() override;
		void Reset() override;
	};
}
