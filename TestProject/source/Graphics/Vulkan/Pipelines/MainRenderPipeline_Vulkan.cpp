#include "pch.h"
#include "MainRenderPipeline_Vulkan.h"


MainRenderPipeline_Vulkan::MainRenderPipeline_Vulkan(
	Vulkan::GraphicsContext_Vulkan& graphicsContext,
	Vulkan::RenderWindow_Vulkan& renderWindow) :
	graphicsContext(graphicsContext),
	renderWindow(renderWindow),	
	rwfbSemaphores{ { graphicsContext} },// { graphicsContext }, { graphicsContext }, { graphicsContext }, { graphicsContext } },
	presentSemaphores{ { graphicsContext} },// { graphicsContext }, { graphicsContext }, { graphicsContext }, { graphicsContext } },
	currentFrame(0)
{
	//Creating renderpass
	{
		std::array<VkAttachmentDescription, 1> attachmentDescriptions{ {
			{
				.format = VK_FORMAT_B8G8R8A8_SRGB,
				.samples = VK_SAMPLE_COUNT_1_BIT,
				.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
				.storeOp = VK_ATTACHMENT_STORE_OP_STORE,
				.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
				.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
				.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
				.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
			}
		} };

		std::array<std::array<VkAttachmentReference, 1>, 1> subpassesAttachmentReferences{ {
			{ { 
			{
				.attachment = 0,
				.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
			} 
			} },
		} };
		subpassesAttachmentReferences[0][0].attachment = 0;
		subpassesAttachmentReferences[0][0].layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		std::array<VkSubpassDescription, 1> subpassDescriptions{ {
			{
				.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS,
				.colorAttachmentCount = subpassesAttachmentReferences[0].size(),
				.pColorAttachments = subpassesAttachmentReferences[0].data(),
			}
		} };

		std::array<VkSubpassDependency, 1> subpassDependencies { {
			{ 
				.srcSubpass = VK_SUBPASS_EXTERNAL,
				.dstSubpass = 0,
				.srcStageMask = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
				.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
				.srcAccessMask = 0,
				.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
			}
		} };		

		VkRenderPassCreateInfo renderPassInfo{};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		renderPassInfo.attachmentCount = attachmentDescriptions.size();
		renderPassInfo.pAttachments = attachmentDescriptions.data();
		renderPassInfo.subpassCount = subpassDescriptions.size();
		renderPassInfo.pSubpasses = subpassDescriptions.data();
		renderPassInfo.dependencyCount = subpassDependencies.size();
		renderPassInfo.pDependencies = subpassDependencies.data();

		CHECK_VULKAN(vkCreateRenderPass(graphicsContext.GetDevice(), &renderPassInfo, nullptr, &renderPass));			
	}

	VkCommandPoolCreateInfo poolInfo{};
	poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
	poolInfo.queueFamilyIndex = graphicsContext.GetGraphicsQueueFamily();
	CHECK_VULKAN(vkCreateCommandPool(graphicsContext.GetDevice(), &poolInfo, nullptr, &commandPool));

	VkCommandBufferAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.commandPool = commandPool;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandBufferCount = framesInFlight;
	CHECK_VULKAN(vkAllocateCommandBuffers(graphicsContext.GetDevice(), &allocInfo, commandBuffers));

	for (auto& fence : fences)
	{
		VkFenceCreateInfo createInfo{ };
		createInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		createInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

		CHECK_VULKAN(vkCreateFence(graphicsContext.GetDevice(), &createInfo, nullptr, &fence));
	}

	vkGetDeviceQueue(graphicsContext.GetDevice(), graphicsContext.GetGraphicsQueueFamily(), 0, &queue);
}

MainRenderPipeline_Vulkan::~MainRenderPipeline_Vulkan()
{
	CHECK_VULKAN(vkWaitForFences(graphicsContext.GetDevice(), framesInFlight, fences, VK_TRUE, UINT64_MAX));

	for (auto& fence : fences)	
		vkDestroyFence(graphicsContext.GetDevice(), fence, nullptr);
	
	vkFreeCommandBuffers(graphicsContext.GetDevice(), commandPool, framesInFlight, commandBuffers);
	vkDestroyCommandPool(graphicsContext.GetDevice(), commandPool, nullptr);
	vkDestroyRenderPass(graphicsContext.GetDevice(), renderPass, nullptr);
}

void MainRenderPipeline_Vulkan::Execute(UI2::NodeRenderTask_Vulkan& nodeRenderTask)
{	
	Vulkan::RenderWindowFramebuffer_Vulkan* rwfb;

	if (!renderWindow.AcquireNextFramebuffer(rwfbSemaphores[currentFrame], rwfb))
		return;

	Vec2u targetSize = rwfb->GetFramebuffer().GetSize();
		
	VkFence& fence = fences[currentFrame];
	VkCommandBuffer& commandBuffer = commandBuffers[currentFrame];
	CHECK_VULKAN(vkWaitForFences(graphicsContext.GetDevice(), 1, &fence, VK_TRUE, UINT64_MAX));
	CHECK_VULKAN(vkResetFences(graphicsContext.GetDevice(), 1, &fence));

	CHECK_VULKAN(vkResetCommandBuffer(commandBuffer, 0));

	VkCommandBufferBeginInfo beginInfo{};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = 0; // Optional
	beginInfo.pInheritanceInfo = nullptr; // Optional
	CHECK_VULKAN(vkBeginCommandBuffer(commandBuffer, &beginInfo));

	nodeRenderTask.PrepareRender(commandBuffer);

	VkRenderPassBeginInfo renderPassInfo{
		.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
		.renderPass = renderPass,
		.framebuffer = rwfb->GetFramebuffer().GetHandle(),
		.renderArea = {
			.offset = { 0, 0 },
			.extent = {targetSize.x, targetSize.y} }		
	}; 

	VkClearValue clearColor = { {{0.0f, 0.0f, 0.0f, 1.0f}} };
	renderPassInfo.clearValueCount = 1;
	renderPassInfo.pClearValues = &clearColor;

	vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
		
	VkSemaphore commandBufferFinishedSignalSemaphore;
	uint64 commandBufferFinishedSignalValue;

	nodeRenderTask.Render(targetSize, commandBuffer, commandBufferFinishedSignalSemaphore, commandBufferFinishedSignalValue);	

	vkCmdEndRenderPass(commandBuffer);

	CHECK_VULKAN(vkEndCommandBuffer(commandBuffer));

	VkSemaphore waitSemaphores[] = { rwfbSemaphores[currentFrame].GetHandle() };
	VkPipelineStageFlags waitDstStageMasks[_countof(waitSemaphores)] = {VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT};
	uint64 waitSemaphoreValues[_countof(waitSemaphores)] = { 0 };

	VkSemaphore signalSemaphores[] = { commandBufferFinishedSignalSemaphore, presentSemaphores[currentFrame].GetHandle() };
	uint64 signalSemaphoreValues[_countof(signalSemaphores)] = { commandBufferFinishedSignalValue, 0 };

	VkTimelineSemaphoreSubmitInfo timelineSubmitInfo{
		.sType = VK_STRUCTURE_TYPE_TIMELINE_SEMAPHORE_SUBMIT_INFO,
		.pNext = nullptr,		
		.waitSemaphoreValueCount = _countof(waitSemaphoreValues),
		.pWaitSemaphoreValues = waitSemaphoreValues,
		.signalSemaphoreValueCount = _countof(signalSemaphoreValues),
		.pSignalSemaphoreValues = signalSemaphoreValues,
	};

	VkSubmitInfo submitInfo{
		.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
		.pNext = &timelineSubmitInfo,		
		.waitSemaphoreCount = _countof(waitSemaphores),
		.pWaitSemaphores = waitSemaphores,
		.pWaitDstStageMask = waitDstStageMasks,
		.commandBufferCount = 1,
		.pCommandBuffers = &commandBuffer,
		.signalSemaphoreCount = _countof(signalSemaphores),
		.pSignalSemaphores = signalSemaphores,
	};

	CHECK_VULKAN(vkQueueSubmit(queue, 1, &submitInfo, fence));

	renderWindow.PresentFramebuffer(*rwfb, { &presentSemaphores[currentFrame] });

	currentFrame = (currentFrame + 1) % framesInFlight;
}