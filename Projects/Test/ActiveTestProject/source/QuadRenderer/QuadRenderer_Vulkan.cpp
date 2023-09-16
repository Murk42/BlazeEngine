#include "QuadRenderer_Vulkan.h"

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

    QuadRenderer_Vulkan::QuadRenderer_Vulkan(GraphicsContext& graphicsContext, RenderContext& transferRenderContext)
        : graphicsContext(&graphicsContext), currentCommnadBuffer(0)
    {
        struct PushConstants
        {
            Mat4f proj;
        };

        //Creating renderpass
        {
            VkAttachmentDescription attachmentDescriptions[1]{ };
            attachmentDescriptions[0].format = VK_FORMAT_B8G8R8A8_SRGB;
            attachmentDescriptions[0].samples = VK_SAMPLE_COUNT_1_BIT;
            attachmentDescriptions[0].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
            attachmentDescriptions[0].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
            attachmentDescriptions[0].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
            attachmentDescriptions[0].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
            attachmentDescriptions[0].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
            attachmentDescriptions[0].finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

            VkSubpassDescription subpassDescriptions[1]{ };
            VkAttachmentReference subpassesAttachmentReferences[1][1]{ };

            subpassesAttachmentReferences[0][0].attachment = 0;
            subpassesAttachmentReferences[0][0].layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
            subpassDescriptions[0].pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
            subpassDescriptions[0].colorAttachmentCount = _countof(subpassesAttachmentReferences[0]);
            subpassDescriptions[0].pColorAttachments = subpassesAttachmentReferences[0];

            VkSubpassDependency subpassDependencies[1]{ };
            subpassDependencies[0].srcSubpass = VK_SUBPASS_EXTERNAL;
            subpassDependencies[0].srcStageMask = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
            subpassDependencies[0].srcAccessMask = 0;

            subpassDependencies[0].dstSubpass = 0;
            subpassDependencies[0].dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;;
            subpassDependencies[0].dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

            VkRenderPassCreateInfo renderPassInfo{};
            renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
            renderPassInfo.attachmentCount = _countof(attachmentDescriptions);
            renderPassInfo.pAttachments = attachmentDescriptions;
            renderPassInfo.subpassCount = _countof(subpassDescriptions);
            renderPassInfo.pSubpasses = subpassDescriptions;
            renderPassInfo.dependencyCount = _countof(subpassDependencies);
            renderPassInfo.pDependencies = subpassDependencies;

            if (vkCreateRenderPass(graphicsContext.GetDevice(), &renderPassInfo, nullptr, &renderPass) != VK_SUCCESS)
                Debug::Logger::LogError("BlazeEngine", "Failed to create render pass!");
        }

        //Creating pipeline and pipeline layout
        {
            VkShaderModule shaders[2]{ };
            shaders[0] = LoadShaderModule(graphicsContext.GetDevice(), "compiledShaders/vert.spv");
            shaders[1] = LoadShaderModule(graphicsContext.GetDevice(), "compiledShaders/frag.spv");

            VkPipelineShaderStageCreateInfo shaderStages[2]{ };

            shaderStages[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
            shaderStages[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
            shaderStages[0].module = shaders[0];
            shaderStages[0].pName = "main";

            shaderStages[1].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
            shaderStages[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
            shaderStages[1].module = shaders[1];
            shaderStages[1].pName = "main";

            std::vector<VkDynamicState> dynamicStates = {
                VK_DYNAMIC_STATE_VIEWPORT,
                VK_DYNAMIC_STATE_SCISSOR
            };

            VkPipelineDynamicStateCreateInfo dynamicState{};
            dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
            dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
            dynamicState.pDynamicStates = dynamicStates.data();

            VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
            vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;

            VkVertexInputAttributeDescription vertexAttributeDescriptions[1]{ };
            vertexAttributeDescriptions[0].offset = 0;
            vertexAttributeDescriptions[0].binding = 0;
            vertexAttributeDescriptions[0].location = 0;
            vertexAttributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;

            VkVertexInputBindingDescription vertexBindingDescriptions[1]{ };
            vertexBindingDescriptions[0].binding = 0;
            vertexBindingDescriptions[0].stride = 12;
            vertexBindingDescriptions[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

            vertexInputInfo.vertexBindingDescriptionCount = _countof(vertexBindingDescriptions);
            vertexInputInfo.pVertexBindingDescriptions = vertexBindingDescriptions;
            vertexInputInfo.vertexAttributeDescriptionCount = _countof(vertexAttributeDescriptions);
            vertexInputInfo.pVertexAttributeDescriptions = vertexAttributeDescriptions;

            VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
            inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
            inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
            inputAssembly.primitiveRestartEnable = VK_FALSE;

            VkViewport viewport{};
            viewport.x = 0.0f;
            viewport.y = 0.0f;
            viewport.width = (float)0;
            viewport.height = (float)0;
            viewport.minDepth = 0.0f;
            viewport.maxDepth = 1.0f;

            VkRect2D scissor{};
            scissor.offset = { 0, 0 };
            scissor.extent = { 0, 0 };

            VkPipelineViewportStateCreateInfo viewportState{};
            viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
            viewportState.viewportCount = 1;
            viewportState.pViewports = &viewport;
            viewportState.scissorCount = 1;
            viewportState.pScissors = &scissor;

            VkPipelineRasterizationStateCreateInfo rasterizer{};
            rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
            rasterizer.depthClampEnable = VK_FALSE;
            rasterizer.rasterizerDiscardEnable = VK_FALSE;
            rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
            rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
            rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
            rasterizer.depthBiasEnable = VK_FALSE;
            rasterizer.depthBiasConstantFactor = 0.0f; // Optional
            rasterizer.depthBiasClamp = 0.0f; // Optional
            rasterizer.depthBiasSlopeFactor = 0.0f; // Optional
            rasterizer.lineWidth = 1.0f;

            VkPipelineMultisampleStateCreateInfo multisampling{};
            multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
            multisampling.sampleShadingEnable = VK_FALSE;
            multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
            multisampling.minSampleShading = 1.0f; // Optional
            multisampling.pSampleMask = nullptr; // Optional
            multisampling.alphaToCoverageEnable = VK_FALSE; // Optional
            multisampling.alphaToOneEnable = VK_FALSE; // Optional

            VkPipelineColorBlendAttachmentState colorBlendAttachment{};
            colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
            colorBlendAttachment.blendEnable = VK_FALSE;
            colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
            colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
            colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD; // Optional
            colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
            colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
            colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD; // Optional

            VkPipelineColorBlendStateCreateInfo colorBlending{};
            colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
            colorBlending.logicOpEnable = VK_FALSE;
            colorBlending.logicOp = VK_LOGIC_OP_COPY; // Optional
            colorBlending.attachmentCount = 1;
            colorBlending.pAttachments = &colorBlendAttachment;
            colorBlending.blendConstants[0] = 0.0f; // Optional
            colorBlending.blendConstants[1] = 0.0f; // Optional
            colorBlending.blendConstants[2] = 0.0f; // Optional
            colorBlending.blendConstants[3] = 0.0f; // Optional       

            VkPushConstantRange push_constant{ };
            push_constant.offset = 0;
            push_constant.size = sizeof(PushConstants);
            push_constant.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

            VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
            pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
            pipelineLayoutInfo.setLayoutCount = 0; // Optional
            pipelineLayoutInfo.pSetLayouts = nullptr; // Optional
            pipelineLayoutInfo.pushConstantRangeCount = 1; // Optional
            pipelineLayoutInfo.pPushConstantRanges = &push_constant; // Optional

            if (vkCreatePipelineLayout(graphicsContext.GetDevice(), &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS)
                Debug::Logger::LogError("BlazeEngine", "Failed to create pipeline layout!");

            VkGraphicsPipelineCreateInfo pipelineInfo{};
            pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
            pipelineInfo.stageCount = _countof(shaderStages);
            pipelineInfo.pStages = shaderStages;
            pipelineInfo.pVertexInputState = &vertexInputInfo;
            pipelineInfo.pInputAssemblyState = &inputAssembly;
            pipelineInfo.pViewportState = &viewportState;
            pipelineInfo.pRasterizationState = &rasterizer;
            pipelineInfo.pMultisampleState = &multisampling;
            pipelineInfo.pDepthStencilState = nullptr; // Optional
            pipelineInfo.pColorBlendState = &colorBlending;
            pipelineInfo.pDynamicState = &dynamicState;
            pipelineInfo.layout = pipelineLayout;
            pipelineInfo.renderPass = renderPass;
            pipelineInfo.subpass = 0;
            pipelineInfo.basePipelineHandle = VK_NULL_HANDLE; // Optional
            pipelineInfo.basePipelineIndex = -1; // Optional

            if (vkCreateGraphicsPipelines(graphicsContext.GetDevice(), VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &pipeline) != VK_SUCCESS)
                Debug::Logger::LogError("BlazeEngine", "Failed to create graphics pipeline!");

            vkDestroyShaderModule(graphicsContext.GetDevice(), shaders[0], nullptr);
            vkDestroyShaderModule(graphicsContext.GetDevice(), shaders[1], nullptr);
        }

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

        //Create vertex buffer
        {
            Vec3f vertices[]{
            Vec3f(-1, -1, 0),
            Vec3f(1, -1, 0),
            Vec3f(-1, 1, 0),
            Vec3f(-1, 1, 0),
            Vec3f(1, -1, 0),
            Vec3f(1, 1, 0),
            };

            VkBuffer stagingBuffer;
            VkDeviceMemory stagingBufferMemory;
            CreateBuffer(graphicsContext.GetDevice(), graphicsContext.GetPhysicalDevice(), 72, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

            void* map;
            vkMapMemory(graphicsContext.GetDevice(), stagingBufferMemory, 0, 72, 0, &map);
            memcpy(map, vertices, 72);
            vkUnmapMemory(graphicsContext.GetDevice(), stagingBufferMemory);

            CreateBuffer(graphicsContext.GetDevice(), graphicsContext.GetPhysicalDevice(), 72, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, vertexBuffer, vertexBufferMemory);

            CopyBuffer(graphicsContext.GetDevice(), stagingBuffer, vertexBuffer, 72, commandPool, transferRenderContext.GetQueue());

            vkDestroyBuffer(graphicsContext.GetDevice(), stagingBuffer, nullptr);
            vkFreeMemory(graphicsContext.GetDevice(), stagingBufferMemory, nullptr);
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

    QuadRenderer_Vulkan::~QuadRenderer_Vulkan()
    {
        vkDeviceWaitIdle(graphicsContext->GetDevice());

        for (auto& fence : fences)
        {
            vkWaitForFences(graphicsContext->GetDevice(), 1, &fence, VK_TRUE, UINT64_MAX);
            vkDestroyFence(graphicsContext->GetDevice(), fence, nullptr);
        }

        vkDestroyBuffer(graphicsContext->GetDevice(), vertexBuffer, nullptr);
        vkFreeMemory(graphicsContext->GetDevice(), vertexBufferMemory, nullptr);
        vkFreeCommandBuffers(graphicsContext->GetDevice(), commandPool, 2, commandBuffers);
        vkDestroyCommandPool(graphicsContext->GetDevice(), commandPool, nullptr);
        vkDestroyRenderPass(graphicsContext->GetDevice(), renderPass, nullptr);
        vkDestroyPipeline(graphicsContext->GetDevice(), pipeline, nullptr);
        vkDestroyPipelineLayout(graphicsContext->GetDevice(), pipelineLayout, nullptr);
    }

    void QuadRenderer_Vulkan::RenderQuad(RenderContext& context, Framebuffer& target, std::initializer_list<Semaphore*> waitSemaphores, std::initializer_list<Semaphore*> signalSemaphores)
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

        Vec2i targetSize = target.GetSize();

        VkRenderPassBeginInfo renderPassInfo{};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassInfo.renderPass = renderPass;
        renderPassInfo.framebuffer = target.GetHandle();
        renderPassInfo.renderArea.offset = { 0, 0 };
        renderPassInfo.renderArea.extent = { (uint)targetSize.x, (uint)targetSize.y };

        VkClearValue clearColor = { {{0.0f, 0.0f, 0.0f, 1.0f}} };
        renderPassInfo.clearValueCount = 1;
        renderPassInfo.pClearValues = &clearColor;

        vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

        vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);

        VkViewport viewport{};
        viewport.x = 0;
        viewport.y = 0;
        viewport.width = static_cast<float>(targetSize.x);
        viewport.height = static_cast<float>(targetSize.y);
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;
        vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

        VkRect2D scissor{};
        scissor.offset = { 0, 0 };
        scissor.extent = { (uint)targetSize.x, (uint)targetSize.y };
        vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

        Mat4f proj = Mat4f::OrthographicMatrix(-2, 2, -2, 2, -2, 2);
        vkCmdPushConstants(commandBuffer, pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, 0, 64, &proj);

        VkDeviceSize offset = 0;
        vkCmdBindVertexBuffers(commandBuffer, 0, 1, &vertexBuffer, &offset);

        vkCmdDraw(commandBuffer, 6, 1, 0, 0);

        vkCmdEndRenderPass(commandBuffer);

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