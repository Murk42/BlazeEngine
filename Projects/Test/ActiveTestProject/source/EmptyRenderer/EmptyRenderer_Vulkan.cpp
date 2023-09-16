#include "EmptyRenderer_Vulkan.h"

namespace Graphics3::Vulkan
{
    static VkShaderModule LoadShaderModule(VkDevice device, const Path& path)
    {
        VkShaderModule shader;

        File file{ path, FileAccessPermission::Read };

        size_t fileSize = file.GetSize();
        std::vector<char> buffer(fileSize);

        file.Read(buffer.data(), fileSize);
        file.Close();

        VkShaderModuleCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        createInfo.codeSize = buffer.size();
        createInfo.pCode = reinterpret_cast<const uint32_t*>(buffer.data());

        if (vkCreateShaderModule(device, &createInfo, nullptr, (VkShaderModule*)&shader) != VK_SUCCESS)
            Debug::Logger::LogError("BlazeEngine", "Failed to create shader module!");

        return shader;
    }

    static uint32_t FindMemoryType(VkPhysicalDevice physicalDevice, uint32_t typeFilter, VkMemoryPropertyFlags requiredProperties)
    {
        VkPhysicalDeviceMemoryProperties memProperties;
        vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memProperties);

        for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
            uint32 memoryTypeBits = (1 << i);
            bool isRequiredMemoryType = typeFilter & memoryTypeBits;

            VkMemoryPropertyFlags properties = memProperties.memoryTypes[i].propertyFlags;
            bool hashRequiredProperties = (properties & requiredProperties) == requiredProperties;

            if (isRequiredMemoryType && hashRequiredProperties)
                return i;
        }

        throw std::runtime_error("failed to find suitable memory type!");
    }
    static void CreateBuffer(VkDevice device, VkPhysicalDevice physicalDevice, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory)
    {
        VkBufferCreateInfo bufferInfo{};
        bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        bufferInfo.size = size;
        bufferInfo.usage = usage;
        bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        if (vkCreateBuffer(device, &bufferInfo, nullptr, &buffer) != VK_SUCCESS) {
            throw std::runtime_error("failed to create buffer!");
        }

        VkMemoryRequirements memRequirements;
        vkGetBufferMemoryRequirements(device, buffer, &memRequirements);

        VkMemoryAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocInfo.allocationSize = memRequirements.size;
        allocInfo.memoryTypeIndex = FindMemoryType(physicalDevice, memRequirements.memoryTypeBits, properties);

        if (vkAllocateMemory(device, &allocInfo, nullptr, &bufferMemory) != VK_SUCCESS) {
            throw std::runtime_error("failed to allocate buffer memory!");
        }

        vkBindBufferMemory(device, buffer, bufferMemory, 0);
    }
    static void CopyBuffer(VkDevice device, VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size, VkCommandPool commandPool, VkQueue queue)
    {
        VkCommandBufferAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandPool = commandPool;
        allocInfo.commandBufferCount = 1;

        VkCommandBuffer commandBuffer;
        vkAllocateCommandBuffers(device, &allocInfo, &commandBuffer);

        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

        vkBeginCommandBuffer(commandBuffer, &beginInfo);

        VkBufferCopy copyRegion{};
        copyRegion.srcOffset = 0; // Optional
        copyRegion.dstOffset = 0; // Optional
        copyRegion.size = size;
        vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

        vkEndCommandBuffer(commandBuffer);

        VkSubmitInfo submitInfo{};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &commandBuffer;

        vkQueueSubmit(queue, 1, &submitInfo, VK_NULL_HANDLE);
        vkQueueWaitIdle(queue);

        vkFreeCommandBuffers(device, commandPool, 1, &commandBuffer);
    }

    EmptyRenderer_Vulkan::EmptyRenderer_Vulkan(GraphicsContext& graphicsContext, RenderContext& transferRenderContext)
        : graphicsContext(&graphicsContext), currentCommnadBuffer(0), fences()
    {        

        //Creating command pool
        {
            VkCommandPoolCreateInfo poolInfo{};
            poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
            poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;            
            poolInfo.queueFamilyIndex = graphicsContext.GetGraphicsQueueFamily();

            if (vkCreateCommandPool(graphicsContext.GetDevice(), &poolInfo, nullptr, &commandPool) != VK_SUCCESS)
                Debug::Logger::LogError("BlazeEngine", "Failed to create command pool!\n");
        }

        //Creating command buffer
        {
            VkCommandBufferAllocateInfo allocInfo{};
            allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
            allocInfo.commandPool = commandPool;
            allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
            allocInfo.commandBufferCount = 2;

            if (vkAllocateCommandBuffers(graphicsContext.GetDevice(), &allocInfo, commandBuffers) != VK_SUCCESS)
                Debug::Logger::LogError("BlazeEngine", "failed to allocate command buffers!\n");
        }        

        //Creating fences
        {
            for (auto& fence : fences)
            {
                VkFenceCreateInfo createInfo{ };
                createInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
                createInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

                vkCreateFence(graphicsContext.GetDevice(), &createInfo, nullptr, &fence);
            }
        }
    }

    EmptyRenderer_Vulkan::~EmptyRenderer_Vulkan()
    {
        vkDeviceWaitIdle(graphicsContext->GetDevice());

        for (auto& fence : fences)
        {
            vkWaitForFences(graphicsContext->GetDevice(), 1, &fence, VK_TRUE, UINT64_MAX);
            vkDestroyFence(graphicsContext->GetDevice(), fence, nullptr);
        }
        
        vkFreeCommandBuffers(graphicsContext->GetDevice(), commandPool, 2, commandBuffers);
        vkDestroyCommandPool(graphicsContext->GetDevice(), commandPool, nullptr);        
    }

    void EmptyRenderer_Vulkan::RenderEmpty(RenderContext& context, Framebuffer& target, Recti renderArea, std::initializer_list<Semaphore*> waitSemaphores, std::initializer_list<Semaphore*> signalSemaphores)
	{
        auto& commandBuffer = commandBuffers[currentCommnadBuffer];
        auto& fence = fences[currentCommnadBuffer];

        vkWaitForFences(graphicsContext->GetDevice(), 1, &fence, VK_TRUE, UINT64_MAX);   
        vkResetFences(graphicsContext->GetDevice(), 1, &fence);

        vkResetCommandBuffer(commandBuffer, 0);

        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.flags = 0; // Optional
        beginInfo.pInheritanceInfo = nullptr; // Optional

        if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS)
            Debug::Logger::LogError("BlazeEngine", "Failed to begin recording command buffer!");

        VkImageSubresourceRange imageSubresourceRange{ };
        imageSubresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        imageSubresourceRange.baseMipLevel = 0;
        imageSubresourceRange.levelCount = 1;
        imageSubresourceRange.baseArrayLayer = 0;
        imageSubresourceRange.layerCount = 1;

        VkImageMemoryBarrier imageMemoryBarrier{ };
        imageMemoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        imageMemoryBarrier.pNext = nullptr;
        imageMemoryBarrier.srcAccessMask = VK_ACCESS_MEMORY_WRITE_BIT;
        imageMemoryBarrier.dstAccessMask = VK_ACCESS_MEMORY_READ_BIT;
        imageMemoryBarrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        imageMemoryBarrier.newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
        imageMemoryBarrier.srcQueueFamilyIndex = 0;
        imageMemoryBarrier.dstQueueFamilyIndex = 0;
        imageMemoryBarrier.image = target.GetImage();
        imageMemoryBarrier.subresourceRange = imageSubresourceRange;

        vkCmdPipelineBarrier(commandBuffer, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_DEPENDENCY_BY_REGION_BIT, 0, nullptr, 0, nullptr, 1, &imageMemoryBarrier);

        if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS)
            Debug::Logger::LogError("BlazeEngine", "Failed to record command buffer!\n");

        Array<VkSemaphore> _waitSemaphores{ [&](VkSemaphore* it, uint i) {
            *it = waitSemaphores.begin()[i]->GetHandle();
        }, (uint)waitSemaphores.size() };
        Array<VkSemaphore> _signalSemaphores{ [&](VkSemaphore* it, uint i) {
            *it = signalSemaphores.begin()[i]->GetHandle();
        }, (uint)signalSemaphores.size() };
        Array<VkPipelineStageFlags> _waitStages{ (uint)signalSemaphores.size(), VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT };

        VkSubmitInfo submitInfo{};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &commandBuffer;
        submitInfo.waitSemaphoreCount = _waitSemaphores.Count();
        submitInfo.pWaitSemaphores = _waitSemaphores.Ptr();
        submitInfo.pWaitDstStageMask = _waitStages.Ptr();
        submitInfo.signalSemaphoreCount = _signalSemaphores.Count();
        submitInfo.pSignalSemaphores = _signalSemaphores.Ptr();        

        if (vkQueueSubmit(context.GetQueue(), 1, &submitInfo, fence) != VK_SUCCESS)
            Debug::Logger::LogError("BlazeEngine", "Failed to submit draw command buffer!");

        currentCommnadBuffer = (currentCommnadBuffer + 1) % 2;
	}
}