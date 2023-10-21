#include "TextRenderer_Vulkan.h"
#include "vk_mem_alloc.h"
#include "Graphics3/Vulkan/VulkanInstance.h"

namespace Graphics3::Vulkan
{
    extern std::unique_ptr<VulkanInstance> vulkanInstance;

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

    class FontRenderData
    {
    public:
        struct CharacterData
        {
            Vec2f uv1;
            Vec2f uv2;
            Vec2f renderOffset;
            Vec2f renderSize;
        };

        GraphicsContext* graphicsContext;
        VkImage image;
        VmaAllocation imageAllocation;

        Map<UnicodeChar, CharacterData> charactersData;

        FontRenderData(GraphicsContext& graphicsContext, const Font& font, const FontResolution& fontResolution)
            : graphicsContext(&graphicsContext), image(VK_NULL_HANDLE), imageAllocation(nullptr)
        {
            BlazeVMAData* vmaData = GetBlazeVMAData(graphicsContext);

            VkBuffer stagingBuffer;
            VmaAllocation stagingBufferAllocation;

            void* bufferPtr;

            Vec2i size;
            constexpr uint pixelSize = 1;
            //uint stride;

            auto CreateTexture = [&](Vec2i _size)
            {                
                size = _size;
                graphicsContext.CreateImage2D(size, VK_FORMAT_R8_UINT, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_IMAGE_LAYOUT_UNDEFINED, image, imageAllocation);
                bufferPtr = graphicsContext.CreateStagingBuffer(size.x * size.y * pixelSize, stagingBuffer, stagingBufferAllocation);
            };
            auto CopyPixels = [&](UnicodeChar character, Vec2i offset, BitmapView bitmap)
            {
                void* dst = (char*)bufferPtr + (offset.x + offset.y * size.x) * pixelSize;                
                uint dstStride = size.x * pixelSize;                

                void* src = (char*)bitmap.GetPixels();                
                uint srcStride = bitmap.GetStride();                

                uint count = bitmap.GetSize().y;

                for (uint i = 0; i < count; ++i)
                {
                    memcpy(dst, src, srcStride);

                    dst = (char*)dst + dstStride;
                    src = (char*)src + srcStride;
                }

                Vec2f bitmapSize = (Vec2f)bitmap.GetSize();

                FontResolutionGlyphData characterMetrics;
                fontResolution.GetGlyphData(character, characterMetrics);

                CharacterData characterData;
                characterData.uv1 = ((Vec2f)offset + Vec2f(0, bitmapSize.y)) / (Vec2f)size;
                characterData.uv1 = ((Vec2f)offset + Vec2f(bitmapSize.x, 0)) / (Vec2f)size;
                characterData.renderOffset = bitmapSize + characterMetrics.size;
                characterData.renderSize = bitmapSize;

                charactersData.Insert(character, characterData);
            };

            FontGlyphRenderers::SDFFontGlyphRenderer glyphRenderer;
            FontGlyphBitmaps bitmaps{ font, fontResolution, glyphRenderer };
            bitmaps.CreateAtlas(CreateTexture, CopyPixels);            

            graphicsContext.TransitionImageLayout(image, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, VK_ACCESS_TRANSFER_WRITE_BIT);

            graphicsContext.CopyBufferToImage(stagingBuffer, image, 0, size.x, Vec2i(), size, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

            graphicsContext.TransitionImageLayout(image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, VK_ACCESS_TRANSFER_WRITE_BIT, VK_ACCESS_SHADER_READ_BIT);

            vmaDestroyBuffer(vmaData->GetAllocator(), stagingBuffer, stagingBufferAllocation);
        }
        FontRenderData(FontRenderData&& other) noexcept
        {
            graphicsContext = other.graphicsContext;
            image = other.image;
            imageAllocation = other.imageAllocation;
            charactersData = std::move(other.charactersData);

            other.graphicsContext = VK_NULL_HANDLE;
            other.image = VK_NULL_HANDLE;
            other.imageAllocation = nullptr;
        }
        ~FontRenderData()
        {
            Clear();
        }

        void Clear()
        {
            if (graphicsContext == nullptr)
                return;

            BlazeVMAData* vmaData = GetBlazeVMAData(*graphicsContext);

            vmaDestroyImage(vmaData->GetAllocator(), image, imageAllocation);
        }

        FontRenderData& operator=(FontRenderData&& other) noexcept
        {
            Clear();

            graphicsContext = other.graphicsContext;
            image = other.image;
            imageAllocation = other.imageAllocation;
            charactersData = std::move(other.charactersData);

            other.graphicsContext = VK_NULL_HANDLE;
            other.image = VK_NULL_HANDLE;
            other.imageAllocation = nullptr;

            return *this;
        }
    private:
        VkFormat GetFormatForOptimalTilingImage(BitmapColorFormat bitmapFormat, BitmapColorComponentType bitmapType)
        {            
            switch (bitmapFormat)
            {
            case Blaze::BitmapColorFormat::Red:
                switch (bitmapType)
                {
                case Blaze::BitmapColorComponentType::Int8:
                    break;
                case Blaze::BitmapColorComponentType::Uint8:
                    break;
                case Blaze::BitmapColorComponentType::Int16:
                    break;
                case Blaze::BitmapColorComponentType::Uint16:
                    break;
                case Blaze::BitmapColorComponentType::Int32:
                    break;
                case Blaze::BitmapColorComponentType::Uint32:
                    break;
                case Blaze::BitmapColorComponentType::Float:
                    break;
                case Blaze::BitmapColorComponentType::Double:
                    break;
                default:
                    break;
                }
            case Blaze::BitmapColorFormat::RGBA:
                break;                            
            default: return VK_FORMAT_UNDEFINED;                
            }
        }
    };
        
    FontRenderData* GetFontRenderData(GraphicsContext& graphicsContext, const Font* font, const FontResolution* fontResolution)
    {
        auto it = fontResolution->dataMap.Find("Default");
        
        if (it.IsNull())        
            return fontResolution->dataMap.Insert("Default", FontRenderData(graphicsContext, *font, *fontResolution)).iterator.GetValue<FontRenderData>();        
        else
            return it.GetValue<FontRenderData>();
    }

    struct PushConstants
    {
        Mat4f proj;
        Mat4f model;
    };

    TextRenderer_Vulkan::TextRenderer_Vulkan(GraphicsContext& graphicsContext, RenderContext& transferRenderContext)
        : graphicsContext(&graphicsContext), commandBufferIndex(0)
    {        

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
            subpassDependencies[0].dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
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
            shaders[0] = LoadShaderModule(graphicsContext.GetDevice(), "compiledShaders/text_vert.spv");            
            shaders[1] = LoadShaderModule(graphicsContext.GetDevice(), "compiledShaders/text_frag.spv");

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

            VkVertexInputAttributeDescription vertexAttributeDescriptions[3]{ };
            vertexAttributeDescriptions[0].offset = 0;
            vertexAttributeDescriptions[0].binding = 0;
            vertexAttributeDescriptions[0].location = 0;
            vertexAttributeDescriptions[0].format = VK_FORMAT_R32G32_SFLOAT;

            vertexAttributeDescriptions[1].offset = 8;
            vertexAttributeDescriptions[1].binding = 0;
            vertexAttributeDescriptions[1].location = 1;
            vertexAttributeDescriptions[1].format = VK_FORMAT_R32G32_SFLOAT;            

            vertexAttributeDescriptions[2].offset = 16;
            vertexAttributeDescriptions[2].binding = 0;
            vertexAttributeDescriptions[2].location = 2;
            vertexAttributeDescriptions[2].format = VK_FORMAT_R32G32B32A32_SFLOAT;


            VkVertexInputBindingDescription vertexBindingDescriptions[1]{ };
            vertexBindingDescriptions[0].binding = 0;
            vertexBindingDescriptions[0].stride = 32;
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
            allocInfo.commandBufferCount = commandBufferCount;

            if (vkAllocateCommandBuffers(graphicsContext.GetDevice(), &allocInfo, commandBuffers) != VK_SUCCESS)
                Debug::Logger::LogError("BlazeEngine", "failed to allocate command buffers!\n");
        }

        //Createing command buffer fences
        {
            for (auto& fence : commandBufferFences)
            {
                VkFenceCreateInfo info{ };
                info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
                info.flags = VK_FENCE_CREATE_SIGNALED_BIT;

                if (vkCreateFence(graphicsContext.GetDevice(), &info, nullptr, &fence))
                    Debug::Logger::LogError("BlazeEngine", "failed to create fences!\n");
            }
        }

        //Create vertex buffers
        {
            auto vmaData = GetBlazeVMAData(graphicsContext);

            vertexBufferSize = sizeof(Vertex) * 1024 * 6;
            vertexBufferPtr = graphicsContext.CreateStagingBuffer(vertexBufferSize, stagingVertexBuffer, stagingVertexBufferAllocation);

            VkBufferCreateInfo bufferInfo{ };
            bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;            
            bufferInfo.size = vertexBufferSize;
            bufferInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;

            VmaAllocationCreateInfo allocationInfo{ };            
            allocationInfo.usage = VMA_MEMORY_USAGE_AUTO;
            allocationInfo.flags = 0;                

            vmaCreateBuffer(vmaData->GetAllocator(), &bufferInfo, &allocationInfo, &vertexBuffer, &vertexBufferAllocation, nullptr);
        }        
    }
    TextRenderer_Vulkan::~TextRenderer_Vulkan()
    {
        auto vmaData = GetBlazeVMAData(*graphicsContext);

        vkDeviceWaitIdle(graphicsContext->GetDevice());

        vkDestroyBuffer(graphicsContext->GetDevice(), vertexBuffer, nullptr);
        vmaDestroyBuffer(vmaData->GetAllocator(), stagingVertexBuffer, stagingVertexBufferAllocation);
        vmaDestroyBuffer(vmaData->GetAllocator(), vertexBuffer, vertexBufferAllocation);        
        for (auto fence : commandBufferFences)
            vkDestroyFence(graphicsContext->GetDevice(), fence, nullptr);
        vkFreeCommandBuffers(graphicsContext->GetDevice(), commandPool, commandBufferCount, commandBuffers);
        vkDestroyCommandPool(graphicsContext->GetDevice(), commandPool, nullptr);
        vkDestroyRenderPass(graphicsContext->GetDevice(), renderPass, nullptr);
        vkDestroyPipeline(graphicsContext->GetDevice(), pipeline, nullptr);
        vkDestroyPipelineLayout(graphicsContext->GetDevice(), pipelineLayout, nullptr);
    } 
    void TextRenderer_Vulkan::RenderText(RenderContext& context, Framebuffer& target, TextRenderData renderData, std::initializer_list<Semaphore*> waitSemaphores, std::initializer_list<Semaphore*> signalSemaphores)
	{                
        FontRenderData* fontRenderData = GetFontRenderData(*graphicsContext, renderData.font, renderData.fontResolution);               

        auto charactersData = renderData.textLayouter->GetCharactersData();
        
        auto charactersDataIt = charactersData.FirstIterator();
        auto colorIt = renderData.colors.FirstIterator();

        float scale = renderData.fontHeight / renderData.fontResolution->GetFontHeight();        

        bool isFirst = true;
        bool isLast = false;
        while (true)
        {            
            auto& commandBuffer = commandBuffers[commandBufferIndex];
            auto& commandBufferFence = commandBufferFences[commandBufferIndex];

            Vertex vertices[6];
            uint vertexCount = 0;
            uint maxVertexCount = vertexBufferSize / sizeof(Vertex);
            while (vertexCount < maxVertexCount)
            {
                auto& characterData = *charactersDataIt;
                Vec2f pos1, pos2, uv1, uv2;
                Vec4f color;
                if (characterData.IsVisible())
                {
                    auto characterRenderDataIt = fontRenderData->charactersData.Find(charactersDataIt->character);                    
                    if (!characterRenderDataIt.IsNull())
                    {
                        auto& characterRenderData = characterRenderDataIt->value;

                        pos1 += characterRenderData.renderOffset;
                        pos2 = pos1 + characterRenderData.renderSize;
                        uv1 = characterRenderData.uv1;
                        uv2 = characterRenderData.uv2;

                        Vec4f color;
                        if (colorIt.IsNull())
                            color = Vec4f(1, 0, 0, 1);
                        else
                            color = *colorIt;

                        vertices[0] = { Vec2f(pos1.x, pos1.y), Vec2f(uv1.x, uv1.y), color };
                        vertices[1] = { Vec2f(pos1.x, pos2.y), Vec2f(uv1.x, uv2.y), color };
                        vertices[2] = { Vec2f(pos2.x, pos1.y), Vec2f(uv2.x, uv1.y), color };
                        vertices[3] = { Vec2f(pos2.x, pos1.y), Vec2f(uv2.x, uv1.y), color };
                        vertices[4] = { Vec2f(pos1.x, pos2.y), Vec2f(uv1.x, uv2.y), color };
                        vertices[5] = { Vec2f(pos2.x, pos2.y), Vec2f(uv2.x, uv2.y), color };                        

                        memcpy((char*)vertexBufferPtr + vertexCount * sizeof(Vertex), vertices, sizeof(Vertex) * 6);

                        vertexCount += 6;
                    }
                }
                
                ++charactersDataIt;
                if (!colorIt.IsNull())
                    ++colorIt;

                if (charactersDataIt == charactersData.BehindIterator())
                {
                    isLast = true;
                    break;
                }
            }

            vkWaitForFences(graphicsContext->GetDevice(), 1, &commandBufferFence, VK_TRUE, UINT64_MAX);
            vkResetFences(graphicsContext->GetDevice(), 1, &commandBufferFence);

            vkResetCommandBuffer(commandBuffer, 0);

            VkCommandBufferBeginInfo beginInfo{};
            beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
            beginInfo.flags = 0; // Optional
            beginInfo.pInheritanceInfo = nullptr; // Optional

            if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS)
                Debug::Logger::LogError("BlazeEngine", "Failed to begin recording command buffer!");
            
            {
                VkBufferMemoryBarrier barrier{};
                barrier.sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER;
                barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
                barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
                barrier.offset = 0;
                barrier.size = vertexCount * sizeof(Vertex);
                barrier.buffer = vertexBuffer;
                barrier.srcAccessMask = VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT;
                barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

                vkCmdPipelineBarrier(
                    commandBuffer,
                    VK_PIPELINE_STAGE_VERTEX_INPUT_BIT, VK_ACCESS_TRANSFER_WRITE_BIT,
                    0,
                    0, nullptr,
                    1, &barrier,
                    0, nullptr
                );
            }

            VkBufferCopy copyRegion{};
            copyRegion.srcOffset = 0; // Optional
            copyRegion.dstOffset = 0; // Optional
            copyRegion.size = vertexCount * sizeof(Vertex);
            vkCmdCopyBuffer(commandBuffer, stagingVertexBuffer, vertexBuffer, 1, &copyRegion);
            
            {
                VkBufferMemoryBarrier barrier{};
                barrier.sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER;
                barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
                barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
                barrier.offset = 0;
                barrier.size = vertexCount * sizeof(Vertex);
                barrier.buffer = vertexBuffer;
                barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
                barrier.dstAccessMask = VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT;

                vkCmdPipelineBarrier(
                    commandBuffer,
                    VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_VERTEX_INPUT_BIT,
                    0,
                    0, nullptr,
                    1, &barrier,
                    0, nullptr
                );
            }

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

            PushConstants pushConstants;
            pushConstants.proj = Mat4f::OrthographicMatrix(0, targetSize.x, 0, targetSize.y, -1, 1);            
            pushConstants.model = Mat4f::Identity(); //TODO calculate string size
            vkCmdPushConstants(commandBuffer, pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, 0, 64, &pushConstants);

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
            Array<VkPipelineStageFlags> _waitStages{ (uint)signalSemaphores.size(), VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT };

            VkSubmitInfo submitInfo{};
            submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
            submitInfo.commandBufferCount = 1;
            submitInfo.pCommandBuffers = &commandBuffer;
            if (isFirst)
            {
                submitInfo.waitSemaphoreCount = _waitSemaphores.Count();
                submitInfo.pWaitSemaphores = _waitSemaphores.Ptr();
                submitInfo.pWaitDstStageMask = _waitStages.Ptr();
            }
            if (isLast)
            {
                submitInfo.signalSemaphoreCount = _signalSemaphores.Count();
                submitInfo.pSignalSemaphores = _signalSemaphores.Ptr();
            }

            if (vkQueueSubmit(context.GetQueue(), 1, &submitInfo, commandBufferFence) != VK_SUCCESS)
            Debug::Logger::LogError("BlazeEngine", "Failed to submit draw command buffer!");

            commandBufferIndex = (commandBufferIndex + 1) % commandBufferCount;

            isFirst = false;

            if (isLast)
                break;
        }        
	}
}