#include "pch.h"
#include "BlazeEngineGraphics\Renderers/Vulkan/Line2DRenderer_Vulkan.h"

namespace Blaze::Graphics::Vulkan	
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

	Line2DRenderer_Vulkan::Line2DRenderer_Vulkan(GraphicsContext_Vulkan& graphicsContext, VkRenderPass renderPass)
		: graphicsContext(graphicsContext)
	{
        struct PushConstants
        {
            Mat4f proj;
        };
       
        //Creating pipeline and pipeline layout
		{
			std::array<VkShaderModule, 2> shaders{ { 
					LoadShaderModule(graphicsContext.GetDevice(), "assets/shaders/Vulkan/line2d.vert.spv"),
					LoadShaderModule(graphicsContext.GetDevice(), "assets/shaders/Vulkan/line2d.frag.spv")
				} };

			std::array<VkPipelineShaderStageCreateInfo, 2> shaderStages{{
				{ 
					.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
					.stage = VK_SHADER_STAGE_VERTEX_BIT,
					.module = shaders[0],
					.pName = "main",
				}, {
					.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
					.stage = VK_SHADER_STAGE_FRAGMENT_BIT,
					.module = shaders[1],
					.pName = "main",
				}
				} };            

			std::array<VkDynamicState, 2> dynamicStates = { {
				VK_DYNAMIC_STATE_VIEWPORT,
				VK_DYNAMIC_STATE_SCISSOR
			} };

			auto vertexAttributeDescriptions = Vertex::VertexLayout::GetVulkanVertexInputData();			

			std::array<VkVertexInputBindingDescription, 1> vertexBindingDescriptions{ {
				{.binding = 0, .stride = sizeof(Vertex), .inputRate = VK_VERTEX_INPUT_RATE_VERTEX }
				} };

			VkViewport viewport{				
				.x = 0.0f,
				.y = 0.0f,
				.width = (float)0,
				.height = (float)0,
				.minDepth = 0.0f,
				.maxDepth = 1.0f,
			};

			VkRect2D scissor{
				.offset = { 0, 0 },
				.extent = { 0, 0 },
			};

			VkPipelineColorBlendAttachmentState colorBlendAttachment{				
				.blendEnable = VK_TRUE,
				.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA,
				.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,
				.colorBlendOp = VK_BLEND_OP_ADD,
				.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE,
				.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO,
				.alphaBlendOp = VK_BLEND_OP_ADD,
				.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT,
			};

			VkPipelineVertexInputStateCreateInfo vertexInputInfo{
				.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
				.pNext = nullptr,
				.flags = 0,
				.vertexBindingDescriptionCount = (uint32)vertexBindingDescriptions.size(),
				.pVertexBindingDescriptions = vertexBindingDescriptions.data(),
				.vertexAttributeDescriptionCount = (uint32)vertexAttributeDescriptions.size(),
				.pVertexAttributeDescriptions = vertexAttributeDescriptions.data(),
			};            

            VkPipelineInputAssemblyStateCreateInfo inputAssembly{
				.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
				.pNext = nullptr,
				.flags = 0,
				.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
				.primitiveRestartEnable = VK_FALSE,
			};

			VkPipelineViewportStateCreateInfo viewportState{
				.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
				.pNext = nullptr,
				.flags = 0,
				.viewportCount = 1,
				.pViewports = &viewport,
				.scissorCount = 1,
				.pScissors = &scissor,
			};

			VkPipelineRasterizationStateCreateInfo rasterizer{
				.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
				.pNext = nullptr,
				.flags = 0,
				.depthClampEnable = VK_FALSE,
				.rasterizerDiscardEnable = VK_FALSE,
				.polygonMode = VK_POLYGON_MODE_FILL,
				.cullMode = VK_CULL_MODE_NONE,
				.frontFace = VK_FRONT_FACE_CLOCKWISE,
				.depthBiasEnable = VK_FALSE,
				.depthBiasConstantFactor = 0.0f,
				.depthBiasClamp = 0.0f,
				.depthBiasSlopeFactor = 0.0f,
				.lineWidth = 1.0f,
			};

			VkPipelineMultisampleStateCreateInfo multisampling{
				.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
				.pNext = nullptr,
				.flags = 0,
				.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT,
				.sampleShadingEnable = VK_FALSE,
				.minSampleShading = 1.0f,
				.pSampleMask = nullptr,
				.alphaToCoverageEnable = VK_FALSE,
				.alphaToOneEnable = VK_FALSE,
			};

			VkPipelineDepthStencilStateCreateInfo depthStencilState{
				.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO,
				.pNext = nullptr,
				.flags = 0,
				.depthTestEnable = false,
				.depthWriteEnable = false,
				.depthCompareOp = VK_COMPARE_OP_ALWAYS,
				.depthBoundsTestEnable = false,
				.stencilTestEnable = false,
				.front = { },
				.back = { },
				.minDepthBounds = 0,
				.maxDepthBounds = 0,
			};

			VkPipelineColorBlendStateCreateInfo colorBlending{
				.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
				.pNext = nullptr,
				.flags = 0,
				.logicOpEnable = VK_FALSE,
				.logicOp = VK_LOGIC_OP_COPY,
				.attachmentCount = 1,
				.pAttachments = &colorBlendAttachment,
				.blendConstants { 0.0f, 0.0f, 0.0f, 0.0f },								
			};

            VkPipelineDynamicStateCreateInfo dynamicState{
				.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
				.pNext = nullptr,
				.flags = 0,
				.dynamicStateCount = (uint32)dynamicStates.size(),
				.pDynamicStates = dynamicStates.data()
			};

			VkPushConstantRange push_constant{
				.stageFlags = VK_SHADER_STAGE_VERTEX_BIT,				
				.offset = 0,				
				.size = sizeof(PushConstants),
			};

			VkPipelineLayoutCreateInfo pipelineLayoutInfo{
				.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
				.pNext = nullptr,
				.flags = 0,
				.setLayoutCount = 0,
				.pSetLayouts = nullptr,
				.pushConstantRangeCount = 1,
				.pPushConstantRanges = &push_constant,
			};

            if (vkCreatePipelineLayout(graphicsContext.GetDevice(), &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS)
                Debug::Logger::LogError("BlazeEngine", "Failed to create pipeline layout!");

			VkGraphicsPipelineCreateInfo pipelineInfo{
				.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
				.stageCount = (uint32)shaderStages.size(),
				.pStages = shaderStages.data(),
				.pVertexInputState = &vertexInputInfo,
				.pInputAssemblyState = &inputAssembly,
				.pViewportState = &viewportState,
				.pRasterizationState = &rasterizer,
				.pMultisampleState = &multisampling,
				.pDepthStencilState = &depthStencilState,
				.pColorBlendState = &colorBlending,
				.pDynamicState = &dynamicState,
				.layout = pipelineLayout,
				.renderPass = renderPass,
				.subpass = 0,
				.basePipelineHandle = VK_NULL_HANDLE,
				.basePipelineIndex = -1,
			};

            if (vkCreateGraphicsPipelines(graphicsContext.GetDevice(), VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &pipeline) != VK_SUCCESS)
                Debug::Logger::LogError("BlazeEngine", "Failed to create graphics pipeline!");

            vkDestroyShaderModule(graphicsContext.GetDevice(), shaders[0], nullptr);
            vkDestroyShaderModule(graphicsContext.GetDevice(), shaders[1], nullptr);
        }        
	}
	Line2DRenderer_Vulkan::~Line2DRenderer_Vulkan()
	{							
		vkDestroyPipeline(graphicsContext.GetDevice(), pipeline, nullptr);
		vkDestroyPipelineLayout(graphicsContext.GetDevice(), pipelineLayout, nullptr);
	}		

	void Line2DRenderer_Vulkan::Render(Line2DRenderCache_Vulkan& renderCache, Vec2u targetSize, VkCommandBuffer commandBuffer, VkSemaphore& commandBufferFinishedSignalSemaphores, uint64& commandBufferFinishedSignalValue)
	{		
		if (renderCache.vertexCount == 0)
			return;

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

		Mat4f proj = Mat4f::OrthographicMatrix(0.0f, (float)targetSize.x, (float)targetSize.y, 0.0f, -1.0f, 1.0f).Transposed();
		vkCmdPushConstants(commandBuffer, pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, 0, 64, &proj);

		VkDeviceSize offset = 0;
		vkCmdBindVertexBuffers(commandBuffer, 0, 1, &renderCache.buffer, &offset);			
		
		vkCmdDraw(commandBuffer, (uint32)renderCache.vertexCount, 1, 0, 0);						

		commandBufferFinishedSignalSemaphores = renderCache.semaphore;
		commandBufferFinishedSignalValue = ++renderCache.nextSemaphoreSignalValue;
	}

	VkResult CreateTimelineSemaphore(VkDevice device, VkSemaphoreCreateFlags flags, uint initialValue, VkSemaphore& semaphore)
	{
		VkSemaphoreTypeCreateInfo typeInfo;
		typeInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_TYPE_CREATE_INFO;
		typeInfo.pNext = nullptr;
		typeInfo.initialValue = initialValue;
		typeInfo.semaphoreType = VK_SEMAPHORE_TYPE_TIMELINE;

		VkSemaphoreCreateInfo createInfo;
		createInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
		createInfo.pNext = &typeInfo;
		createInfo.flags = 0;

		return vkCreateSemaphore(device, &createInfo, nullptr, &semaphore);
	}


	Line2DRenderCache_Vulkan::Line2DRenderCache_Vulkan(GraphicsContext_Vulkan& graphicsContext)
		: graphicsContext(graphicsContext), vertexCount(0), 
		buffer(VK_NULL_HANDLE), bufferMemory(VK_NULL_HANDLE), semaphore(VK_NULL_HANDLE), nextSemaphoreSignalValue(0),
		stagingBuffer(VK_NULL_HANDLE), stagingBufferMemory(VK_NULL_HANDLE)
	{
		CreateTimelineSemaphore(graphicsContext.GetDevice(), 0, 0, semaphore);		
	}
	Line2DRenderCache_Vulkan::~Line2DRenderCache_Vulkan()
	{
		VkSemaphoreWaitInfo waitInfo;
		waitInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_WAIT_INFO;
		waitInfo.pNext = nullptr;
		waitInfo.pSemaphores = &semaphore;
		waitInfo.pValues = &nextSemaphoreSignalValue;
		waitInfo.semaphoreCount = 1;
		waitInfo.flags = 0;
		vkWaitSemaphores(graphicsContext.GetDevice(), &waitInfo, UINT64_MAX);
		
		vkDestroySemaphore(graphicsContext.GetDevice(), semaphore, nullptr);
		vkDestroyBuffer(graphicsContext.GetDevice(), stagingBuffer, nullptr);
		vkFreeMemory(graphicsContext.GetDevice(), stagingBufferMemory, nullptr);
		vkDestroyBuffer(graphicsContext.GetDevice(), buffer, nullptr);
		vkFreeMemory(graphicsContext.GetDevice(), bufferMemory, nullptr);
	}
	void Line2DRenderCache_Vulkan::CreateCache(const Array<Line2DRenderData_Vulkan>& renderData, VkCommandBuffer commandBuffer)
	{
		VkSemaphoreWaitInfo waitInfo{
				.sType = VK_STRUCTURE_TYPE_SEMAPHORE_WAIT_INFO,
				.pNext = nullptr,
				.flags = 0,
				.semaphoreCount = 1,
				.pSemaphores = &semaphore,
				.pValues = &nextSemaphoreSignalValue,
		};

		vkWaitSemaphores(graphicsContext.GetDevice(), &waitInfo, UINT64_MAX);

		if (buffer != VK_NULL_HANDLE)
		{
			vkDestroyBuffer(graphicsContext.GetDevice(), stagingBuffer, nullptr);
			vkFreeMemory(graphicsContext.GetDevice(), stagingBufferMemory, nullptr);
			vkDestroyBuffer(graphicsContext.GetDevice(), buffer, nullptr);
			vkFreeMemory(graphicsContext.GetDevice(), bufferMemory, nullptr);
		}

		vertexCount = renderData.Count() * 6;
		uintMem bufferSize = vertexCount * sizeof(Vertex);
		CreateBuffer(graphicsContext.GetDevice(), graphicsContext.GetPhysicalDevice(), bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);
		CreateBuffer(graphicsContext.GetDevice(), graphicsContext.GetPhysicalDevice(), bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, buffer, bufferMemory);

		Vertex* vertices = nullptr;
		CHECK_VULKAN(vkMapMemory(graphicsContext.GetDevice(), stagingBufferMemory, 0, bufferSize, 0, (void**)&vertices));

		uintMem i = 0;
		for (auto& rd : renderData)
		{
			Vec2f pos1;
			Vec2f pos2;
			Vec2f pos3;
			Vec2f pos4;

			Vec2f dir = rd.p2 - rd.p1;
			if (dir.x == 0 && dir.y == 0)
			{
				pos1 = rd.p1 + Vec2f(rd.width, rd.width);
				pos2 = rd.p1 + Vec2f(rd.width, -rd.width);
				pos3 = rd.p1 + Vec2f(-rd.width, rd.width);
				pos4 = rd.p1 + Vec2f(-rd.width, -rd.width);
			}
			else
			{
				float len = dir.Lenght();
				dir = dir / len;
				Vec2f normal = Vec2f(dir.y, -dir.x);
				float half_width = rd.width;

				//+1 is to make the rectangle around the line wider so antialiased pixels will fit inside.
				pos1 = rd.p1 - dir * (half_width + 1) - normal * (half_width + 1);
				pos2 = rd.p2 + dir * (half_width + 1) - normal * (half_width + 1);
				pos3 = rd.p1 - dir * (half_width + 1) + normal * (half_width + 1);
				pos4 = rd.p2 + dir * (half_width + 1) + normal * (half_width + 1);
			}
			vertices[i + 0] = { .pos = pos1, .pos1 = rd.p1, .pos2 = rd.p2, .color = rd.color, .width = rd.width };
			vertices[i + 1] = { .pos = pos2, .pos1 = rd.p1, .pos2 = rd.p2, .color = rd.color, .width = rd.width };
			vertices[i + 2] = { .pos = pos3, .pos1 = rd.p1, .pos2 = rd.p2, .color = rd.color, .width = rd.width };
			vertices[i + 3] = { .pos = pos2, .pos1 = rd.p1, .pos2 = rd.p2, .color = rd.color, .width = rd.width };
			vertices[i + 4] = { .pos = pos3, .pos1 = rd.p1, .pos2 = rd.p2, .color = rd.color, .width = rd.width };
			vertices[i + 5] = { .pos = pos4, .pos1 = rd.p1, .pos2 = rd.p2, .color = rd.color, .width = rd.width };

			i += 6;
		}

		vkUnmapMemory(graphicsContext.GetDevice(), stagingBufferMemory);

		VkBufferCopy copyRegion{};
		copyRegion.srcOffset = 0; // Optional
		copyRegion.dstOffset = 0; // Optional
		copyRegion.size = bufferSize;
		vkCmdCopyBuffer(commandBuffer, stagingBuffer, buffer, 1, &copyRegion);		

		VkBufferMemoryBarrier bufferMemoryBarrier{
			.sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER,
			.pNext = nullptr,
			.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT,
			.dstAccessMask = VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT,
			.srcQueueFamilyIndex = 0,
			.dstQueueFamilyIndex = 0,
			.buffer = buffer,
			.offset = 0,
			.size = vertexCount * sizeof(Vertex)
		};

		vkCmdPipelineBarrier(commandBuffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_VERTEX_INPUT_BIT, 0, 0, nullptr, 1, &bufferMemoryBarrier, 0, nullptr);
	}	
}