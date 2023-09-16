#include "Vulkan_CommandBuffer.h"
#include "Vulkan_CommandBufferPool.h"
#include "Vulkan_RenderPass.h"
#include "Vulkan_Framebuffer.h"
#include <iostream>

namespace Experimental::Graphics::Vulkan
{
    CommandBufferImpl::CommandBufferImpl(CommandBufferPoolImpl* pool)
        : renderContext(pool->renderContext)
	{
        VkCommandBufferAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.commandPool = pool->commandPool;
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandBufferCount = 1;               

        if (vkAllocateCommandBuffers(renderContext->device, &allocInfo, &commandBuffer) != VK_SUCCESS)
        {
            std::cout << "failed to allocate command buffers!\n";
            std::exit(1);
        }
	}
	CommandBufferImpl::~CommandBufferImpl()
	{
        vkFreeCommandBuffers(renderContext->device, 0, 1, &commandBuffer);
	}
    void CommandBufferImpl::Record(RenderPassImpl* renderPass, FramebufferImpl* framebuffer, Recti rect)
    {
        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.flags = 0; // Optional
        beginInfo.pInheritanceInfo = nullptr; // Optional

        if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) 
        {
            std::cout << "failed to begin recording command buffer!";
            std::exit(1);
        }

        VkRenderPassBeginInfo renderPassInfo{};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassInfo.renderPass = renderPass->renderPass;
        renderPassInfo.framebuffer = framebuffer->framebuffer;
        renderPassInfo.renderArea.offset = { rect.x, rect.y };
        renderPassInfo.renderArea.extent = { (uint)rect.w, (uint)rect.h };

        VkClearValue clearColor = { {{0.0f, 0.0f, 0.0f, 1.0f}} };
        renderPassInfo.clearValueCount = 1;
        renderPassInfo.pClearValues = &clearColor;

        vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

        for (uint i = 0; i < renderPass->subpasses.Count(); ++i)
        {
            auto& subpass = renderPass->subpasses[i];
            vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, subpass.);

            VkViewport viewport{};
            viewport.x = 0.0f;
            viewport.y = 0.0f;
            viewport.width = static_cast<float>(swapChainExtent.width);
            viewport.height = static_cast<float>(swapChainExtent.height);
            viewport.minDepth = 0.0f;
            viewport.maxDepth = 1.0f;
            vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

            VkRect2D scissor{};
            scissor.offset = { 0, 0 };
            scissor.extent = swapChainExtent;
            vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

            vkCmdDraw(commandBuffer, 3, 1, 0, 0);
        }

        vkCmdEndRenderPass(commandBuffer);

        if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
            std::cout << "failed to record command buffer!\n";
            std::exit(1);
        }
    }
    void CommandBufferImpl::Reset()
    {
        vkResetCommandBuffer(commandBuffer, 0);
    }
}