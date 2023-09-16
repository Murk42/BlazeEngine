#pragma once
#include "../Graphics3/Vulkan/API_Vulkan.h"
#include "QuadRendererBase.h"
#include "QuadRenderer_Dynamic.h"

namespace Graphics3::Vulkan
{
	class QuadRenderer_Vulkan : public QuadRendererBase
	{
		GraphicsContext* graphicsContext;
		VkRenderPass renderPass;
		VkPipelineLayout pipelineLayout;
		VkPipeline pipeline;
		VkCommandPool commandPool;
		VkDeviceMemory vertexBufferMemory;
		VkBuffer vertexBuffer;
		
		VkCommandBuffer commandBuffers[2];	
		VkFence fences[2];

		uint currentCommnadBuffer;
	public:				
		QuadRenderer_Vulkan(GraphicsContext& graphicsContext, RenderContext& transferRenderContext);
		~QuadRenderer_Vulkan();

		void RenderQuad(Dynamic::RenderContextBase& context, Dynamic::FramebufferBase& target, std::initializer_list<Dynamic::Semaphore*> waitSemaphores, std::initializer_list<Dynamic::Semaphore*> signalSemaphores) override
		{
			Array<Semaphore*> _waitSemaphores { (uint)waitSemaphores.size() };
			Array<Semaphore*> _signalSemaphores { (uint)signalSemaphores.size() };
			for (uint i = 0; i < (uint)waitSemaphores.size(); ++i) _waitSemaphores[i] = (Semaphore*)waitSemaphores.begin()[i]->GetPtr();
			for (uint i = 0; i < (uint)signalSemaphores.size(); ++i) _signalSemaphores[i] = (Semaphore*)signalSemaphores.begin()[i]->GetPtr();
			RenderQuad((RenderContext&)context, (Framebuffer&)target, 
				std::initializer_list<Semaphore*>(_waitSemaphores.Ptr(), _waitSemaphores.Ptr() + _waitSemaphores.Count()), 
				std::initializer_list<Semaphore*>(_signalSemaphores.Ptr(), _signalSemaphores.Ptr() + _signalSemaphores.Count())
			);
		}
		void RenderQuad(RenderContext& context, Framebuffer& target, std::initializer_list<Semaphore*> waitSemaphores, std::initializer_list<Semaphore*> signalSemaphores);
	};

	using QuadRenderer = QuadRenderer_Vulkan;
}