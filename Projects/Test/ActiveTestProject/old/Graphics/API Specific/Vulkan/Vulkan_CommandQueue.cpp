#include "Vulkan_CommandQueue.h"
#include "Vulkan_Instance.h"
#include "../../CommandBuffer.h"
#include "Vulkan_CommandBuffer.h"
#include "../../Semaphore.h"
#include "Vulkan_Semaphore.h"
#include "../../Fence.h"
#include "Vulkan_Fence.h"
#include "../../Framebuffer.h"
#include "Vulkan_Framebuffer.h"
#include "../../Swapchain.h"
#include "Vulkan_Swapchain.h"
#include <iostream>

namespace Experimental::Graphics::Vulkan
{
	CommandQueueImpl::CommandQueueImpl(RenderContextImpl* renderContext, CommandQueueType type)
		: renderContext(renderContext)
	{
		uint32 queueFamilyIndex;

		switch (type)
		{
		case Experimental::Graphics::CommandQueueType::Render: queueFamilyIndex = renderContext->graphicsQueueFamily; break;
		case Experimental::Graphics::CommandQueueType::Present: queueFamilyIndex = renderContext->presentQueueFamily;  break;
		}

		vkGetDeviceQueue(renderContext->device, queueFamilyIndex, renderContext->graphicsQueueCount, &queue);
	}
	CommandQueueImpl::~CommandQueueImpl()
	{
		//Empty on purpose
	}

	void CommandQueueImpl::Submit(ArrayView<CommandBuffer*> commandBuffers, ArrayView<CommandQueueWait> waitSemaphores, ArrayView<Semaphore*> signalSemaphores, FenceImpl* fence)
	{
		Array<VkCommandBuffer> _commandBuffers { commandBuffers.Count() };
		Array<VkSemaphore> _waitSemaphores { waitSemaphores.Count() };
		Array<VkPipelineStageFlags> _waitStages { waitSemaphores.Count() };
		Array<VkSemaphore> _signalSemaphores { signalSemaphores.Count() };

		for (uint i = 0; i < commandBuffers.Count(); ++i)
			_commandBuffers[i] = ((CommandBufferImpl*)commandBuffers[i]->GetHandle())->commandBuffer;

		for (uint i = 0; i < waitSemaphores.Count(); ++i)
		{
			_waitSemaphores[i] = ((SemaphoreImpl*)waitSemaphores[i].semaphore->GetHandle())->semaphore;
			VkPipelineStageFlags stage;

			switch (waitSemaphores[i].stage)
			{
				case PipelineStage::VertexInput:					stage = VK_PIPELINE_STAGE_VERTEX_INPUT_BIT; break;
				case PipelineStage::VertexShader:					stage = VK_PIPELINE_STAGE_VERTEX_SHADER_BIT; break;
				case PipelineStage::TessellationControlShader :		stage = VK_PIPELINE_STAGE_TESSELLATION_CONTROL_SHADER_BIT; break;
				case PipelineStage::TessellationEvaluationShader:	stage = VK_PIPELINE_STAGE_TESSELLATION_EVALUATION_SHADER_BIT; break;
				case PipelineStage::GeometryShader:					stage = VK_PIPELINE_STAGE_GEOMETRY_SHADER_BIT; break;
				case PipelineStage::FragmentShader:					stage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT; break;
				case PipelineStage::EarlyFragmentTests:				stage = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT; break;
				case PipelineStage::LateFragmentTests:				stage = VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT; break;
				case PipelineStage::ColorAttachmentOutput:			stage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT; break;
			}

			_waitStages[i] = stage;
		}

		for (uint i = 0; i < signalSemaphores.Count(); ++i)		
			_signalSemaphores[i] = ((SemaphoreImpl*)signalSemaphores[i]->GetHandle())->semaphore;

		VkSubmitInfo submitInfo{};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;		
		submitInfo.commandBufferCount = _commandBuffers.Count();
		submitInfo.pCommandBuffers = _commandBuffers.Ptr();		
		submitInfo.waitSemaphoreCount = waitSemaphores.Count();
		submitInfo.pWaitSemaphores = _waitSemaphores.Ptr();
		submitInfo.pWaitDstStageMask = _waitStages.Ptr();		
		submitInfo.signalSemaphoreCount = _signalSemaphores.Count();
		submitInfo.pSignalSemaphores = _signalSemaphores.Ptr();

		if (vkQueueSubmit(queue, 1, &submitInfo, fence->fence) != VK_SUCCESS)
		{
			std::cout << "failed to submit draw command buffer!";
			std::exit(1);
		}
	}
	void CommandQueueImpl::SubmitPresentCommand(SwapchainImpl* swapchain, Framebuffer* framebuffer, Array<Semaphore*> waitSemaphores)
	{
		Array<VkSemaphore> _waitSemaphores { waitSemaphores.Count() };

		for (uint i = 0; i < waitSemaphores.Count(); ++i)
			_waitSemaphores[i] = ((SemaphoreImpl*)waitSemaphores[i]->GetHandle())->semaphore;

		uint32 imageIndex = framebuffer - swapchain->framebuffers.Ptr();

		VkPresentInfoKHR presentInfo{};
		presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
		presentInfo.waitSemaphoreCount = _waitSemaphores.Count();
		presentInfo.pWaitSemaphores = _waitSemaphores.Ptr();		
		presentInfo.swapchainCount = 1;
		presentInfo.pSwapchains = &swapchain->swapChain;
		presentInfo.pImageIndices = &imageIndex;						

		presentInfo.pResults = nullptr; // Optional
		
		if (vkQueuePresentKHR(queue, &presentInfo) != VK_SUCCESS)
		{
			std::cout << "failed to acquire swap chain image!\n";
			std::exit(1);
		}
	}
}