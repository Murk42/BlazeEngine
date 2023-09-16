#include "Vulkan_Pipeline.h"
#include "Vulkan_Instance.h"
#include "Vulkan_Shader.h"
#include "../../Shader.h"
#include "../../VertexLayout.h"
#include "Vulkan_RenderPass.h"
#include <iostream>

namespace Experimental::Graphics::Vulkan
{
    static VkFormat GetVulkanVertexAttributeFormat(VertexAttributeType type)
    {
        switch (type)
        {
        case Experimental::Graphics::VertexAttributeType::Float: return VK_FORMAT_R32_SFLOAT;            
        case Experimental::Graphics::VertexAttributeType::Vec2f: return VK_FORMAT_R32G32_SFLOAT;
        case Experimental::Graphics::VertexAttributeType::Vec3f: return VK_FORMAT_R32G32B32_SFLOAT;
        case Experimental::Graphics::VertexAttributeType::Vec4f: return VK_FORMAT_R32G32B32A32_SFLOAT;
        }
    }

    PipelineImpl::PipelineImpl(RenderPassImpl* renderPass, uint subpassIndex, ArrayView<Shader*> shaders, VertexLayout& vertexLayout, ArrayView<VertexBinding> vertexBindings)
        : renderContext(renderPass->renderContext)
    {
        Array<VkPipelineShaderStageCreateInfo> shaderStages { shaders.Count() };

        for (int i = 0; i < shaders.Count(); ++i)
        {
            const Shader* shader = shaders[i];
            shaderStages[i].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;

            switch (shader->type)
            {
            case ShaderType::Vertex:
                shaderStages[i].stage = VK_SHADER_STAGE_VERTEX_BIT;
                break;
            case ShaderType::Fragment:
                shaderStages[i].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
                break;
            }

            shaderStages[i].module = ((ShaderImpl*)shader->GetHandle())->shader;
            shaderStages[i].pName = "main";
        }

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

        Array<VkVertexInputAttributeDescription> vertexAttributeDescriptions { vertexLayout.attributes.Count() };
        for (uint i = 0; i < vertexLayout.attributes.Count(); ++i)
        {
            vertexAttributeDescriptions[i].offset = vertexLayout.attributes[i].byteOffset;
            vertexAttributeDescriptions[i].binding = vertexLayout.attributes[i].bufferIndex;
            vertexAttributeDescriptions[i].location = vertexLayout.attributes[i].location;
            vertexAttributeDescriptions[i].format = GetVulkanVertexAttributeFormat(vertexLayout.attributes[i].type);            
        }

        Array<VkVertexInputBindingDescription> vertexBindingDescriptions { vertexBindings.Count() };
        for (uint i = 0; i < vertexBindings.Count(); ++i)
        {
            VkVertexInputRate rate;
            switch (vertexBindings[i].rate)
            {
            case VertexInputRate::PerVertex: rate = VK_VERTEX_INPUT_RATE_VERTEX; break;
            case VertexInputRate::PerInstance: rate = VK_VERTEX_INPUT_RATE_INSTANCE; break;            
            }

            vertexBindingDescriptions[i].binding = vertexBindings[i].bufferIndex;
            vertexBindingDescriptions[i].stride = vertexBindings[i].stride;
            vertexBindingDescriptions[i].inputRate = rate;
        }
        
        vertexInputInfo.vertexBindingDescriptionCount = vertexBindingDescriptions.Count();
        vertexInputInfo.pVertexBindingDescriptions = vertexBindingDescriptions.Ptr();
        vertexInputInfo.vertexAttributeDescriptionCount = vertexAttributeDescriptions.Count();
        vertexInputInfo.pVertexAttributeDescriptions = vertexAttributeDescriptions.Ptr();

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

        VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
        pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutInfo.setLayoutCount = 0; // Optional
        pipelineLayoutInfo.pSetLayouts = nullptr; // Optional
        pipelineLayoutInfo.pushConstantRangeCount = 0; // Optional
        pipelineLayoutInfo.pPushConstantRanges = nullptr; // Optional

        if (vkCreatePipelineLayout(renderContext->device, &pipelineLayoutInfo, nullptr, &layout) != VK_SUCCESS)
        {
            std::cout << "failed to create pipeline layout!";
            std::exit(1);
        }

        VkGraphicsPipelineCreateInfo pipelineInfo{};
        pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;        
        pipelineInfo.stageCount = shaderStages.Count();
        pipelineInfo.pStages = shaderStages.Ptr();
        pipelineInfo.pVertexInputState = &vertexInputInfo;
        pipelineInfo.pInputAssemblyState = &inputAssembly;
        pipelineInfo.pViewportState = &viewportState;
        pipelineInfo.pRasterizationState = &rasterizer;
        pipelineInfo.pMultisampleState = &multisampling;
        pipelineInfo.pDepthStencilState = nullptr; // Optional
        pipelineInfo.pColorBlendState = &colorBlending;
        pipelineInfo.pDynamicState = &dynamicState;
        pipelineInfo.layout = layout;
        pipelineInfo.renderPass = renderPass->renderPass;
        pipelineInfo.subpass = subpassIndex;
        pipelineInfo.basePipelineHandle = VK_NULL_HANDLE; // Optional
        pipelineInfo.basePipelineIndex = -1; // Optional

        if (vkCreateGraphicsPipelines(renderContext->device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &pipeline) != VK_SUCCESS)
        {
            std::cout << "failed to create graphics pipeline!";
            std::exit(1);
        }
    }
	PipelineImpl::~PipelineImpl()
	{
        vkDestroyPipeline(renderContext->device, pipeline, nullptr);
        vkDestroyPipelineLayout(renderContext->device, layout, nullptr);
	}
}