#include "Vulkan_RenderPass.h"
#include "Vulkan_Instance.h"
#include <iostream>

namespace Experimental::Graphics::Vulkan
{
    static VkFormat GetVulkanAttachmentFormat(AttachmentFormat format)
    {
        switch (format)
        {
        case AttachmentFormat::Undefined:                  return VK_FORMAT_UNDEFINED;
        case AttachmentFormat::R4G4_UNORM_PACK8:           return VK_FORMAT_R4G4_UNORM_PACK8;
        case AttachmentFormat::R4G4B4A4_UNORM_PACK16:      return VK_FORMAT_R4G4B4A4_UNORM_PACK16;
        case AttachmentFormat::B4G4R4A4_UNORM_PACK16:      return VK_FORMAT_B4G4R4A4_UNORM_PACK16;
        case AttachmentFormat::R5G6B5_UNORM_PACK16:        return VK_FORMAT_R5G6B5_UNORM_PACK16;
        case AttachmentFormat::B5G6R5_UNORM_PACK16:        return VK_FORMAT_B5G6R5_UNORM_PACK16;
        case AttachmentFormat::R5G5B5A1_UNORM_PACK16:      return VK_FORMAT_R5G5B5A1_UNORM_PACK16;
        case AttachmentFormat::B5G5R5A1_UNORM_PACK16:      return VK_FORMAT_B5G5R5A1_UNORM_PACK16;
        case AttachmentFormat::A1R5G5B5_UNORM_PACK16:      return VK_FORMAT_A1R5G5B5_UNORM_PACK16;
        case AttachmentFormat::R8_UNORM:                   return VK_FORMAT_R8_UNORM;
        case AttachmentFormat::R8_SNORM:                   return VK_FORMAT_R8_SNORM;
        case AttachmentFormat::R8_USCALED:                 return VK_FORMAT_R8_USCALED;
        case AttachmentFormat::R8_SSCALED:                 return VK_FORMAT_R8_SSCALED;
        case AttachmentFormat::R8_UINT:                    return VK_FORMAT_R8_UINT;
        case AttachmentFormat::R8_SINT:                    return VK_FORMAT_R8_SINT;
        case AttachmentFormat::R8_SRGB:                    return VK_FORMAT_R8_SRGB;
        case AttachmentFormat::R8G8_UNORM:                 return VK_FORMAT_R8G8_UNORM;
        case AttachmentFormat::R8G8_SNORM:                 return VK_FORMAT_R8G8_SNORM;
        case AttachmentFormat::R8G8_USCALED:               return VK_FORMAT_R8G8_USCALED;
        case AttachmentFormat::R8G8_SSCALED:               return VK_FORMAT_R8G8_SSCALED;
        case AttachmentFormat::R8G8_UINT:                  return VK_FORMAT_R8G8_UINT;
        case AttachmentFormat::R8G8_SINT:                  return VK_FORMAT_R8G8_SINT;
        case AttachmentFormat::R8G8_SRGB:                  return VK_FORMAT_R8G8_SRGB;
        case AttachmentFormat::R8G8B8_UNORM:               return VK_FORMAT_R8G8B8_UNORM;
        case AttachmentFormat::R8G8B8_SNORM:               return VK_FORMAT_R8G8B8_SNORM;
        case AttachmentFormat::R8G8B8_USCALED:             return VK_FORMAT_R8G8B8_USCALED;
        case AttachmentFormat::R8G8B8_SSCALED:             return VK_FORMAT_R8G8B8_SSCALED;
        case AttachmentFormat::R8G8B8_UINT:                return VK_FORMAT_R8G8B8_UINT;
        case AttachmentFormat::R8G8B8_SINT:                return VK_FORMAT_R8G8B8_SINT;
        case AttachmentFormat::R8G8B8_SRGB:                return VK_FORMAT_R8G8B8_SRGB;
        case AttachmentFormat::B8G8R8_UNORM:               return VK_FORMAT_B8G8R8_UNORM;
        case AttachmentFormat::B8G8R8_SNORM:               return VK_FORMAT_B8G8R8_SNORM;
        case AttachmentFormat::B8G8R8_USCALED:             return VK_FORMAT_B8G8R8_USCALED;
        case AttachmentFormat::B8G8R8_SSCALED:             return VK_FORMAT_B8G8R8_SSCALED;
        case AttachmentFormat::B8G8R8_UINT:                return VK_FORMAT_B8G8R8_UINT;
        case AttachmentFormat::B8G8R8_SINT:                return VK_FORMAT_B8G8R8_SINT;
        case AttachmentFormat::B8G8R8_SRGB:                return VK_FORMAT_B8G8R8_SRGB;
        case AttachmentFormat::R8G8B8A8_UNORM:             return VK_FORMAT_R8G8B8A8_UNORM;
        case AttachmentFormat::R8G8B8A8_SNORM:             return VK_FORMAT_R8G8B8A8_SNORM;
        case AttachmentFormat::R8G8B8A8_USCALED:           return VK_FORMAT_R8G8B8A8_USCALED;
        case AttachmentFormat::R8G8B8A8_SSCALED:           return VK_FORMAT_R8G8B8A8_SSCALED;
        case AttachmentFormat::R8G8B8A8_UINT:              return VK_FORMAT_R8G8B8A8_UINT;
        case AttachmentFormat::R8G8B8A8_SINT:              return VK_FORMAT_R8G8B8A8_SINT;
        case AttachmentFormat::R8G8B8A8_SRGB:              return VK_FORMAT_R8G8B8A8_SRGB;
        case AttachmentFormat::B8G8R8A8_UNORM:             return VK_FORMAT_B8G8R8A8_UNORM;
        case AttachmentFormat::B8G8R8A8_SNORM:             return VK_FORMAT_B8G8R8A8_SNORM;
        case AttachmentFormat::B8G8R8A8_USCALED:           return VK_FORMAT_B8G8R8A8_USCALED;
        case AttachmentFormat::B8G8R8A8_SSCALED:           return VK_FORMAT_B8G8R8A8_SSCALED;
        case AttachmentFormat::B8G8R8A8_UINT:              return VK_FORMAT_B8G8R8A8_UINT;
        case AttachmentFormat::B8G8R8A8_SINT:              return VK_FORMAT_B8G8R8A8_SINT;
        case AttachmentFormat::B8G8R8A8_SRGB:              return VK_FORMAT_B8G8R8A8_SRGB;
        case AttachmentFormat::A8B8G8R8_UNORM_PACK32:      return VK_FORMAT_A8B8G8R8_UNORM_PACK32;
        case AttachmentFormat::A8B8G8R8_SNORM_PACK32:      return VK_FORMAT_A8B8G8R8_SNORM_PACK32;
        case AttachmentFormat::A8B8G8R8_USCALED_PACK32:    return VK_FORMAT_A8B8G8R8_USCALED_PACK32;
        case AttachmentFormat::A8B8G8R8_SSCALED_PACK32:    return VK_FORMAT_A8B8G8R8_SSCALED_PACK32;
        case AttachmentFormat::A8B8G8R8_UINT_PACK32:       return VK_FORMAT_A8B8G8R8_UINT_PACK32;
        case AttachmentFormat::A8B8G8R8_SINT_PACK32:       return VK_FORMAT_A8B8G8R8_SINT_PACK32;
        case AttachmentFormat::A8B8G8R8_SRGB_PACK32:       return VK_FORMAT_A8B8G8R8_SRGB_PACK32;
        case AttachmentFormat::A2R10G10B10_UNORM_PACK32:   return VK_FORMAT_A2R10G10B10_UNORM_PACK32;
        case AttachmentFormat::A2R10G10B10_SNORM_PACK32:   return VK_FORMAT_A2R10G10B10_SNORM_PACK32;
        case AttachmentFormat::A2R10G10B10_USCALED_PACK32: return VK_FORMAT_A2R10G10B10_USCALED_PACK32;
        case AttachmentFormat::A2R10G10B10_SSCALED_PACK32: return VK_FORMAT_A2R10G10B10_SSCALED_PACK32;
        case AttachmentFormat::A2R10G10B10_UINT_PACK32:    return VK_FORMAT_A2R10G10B10_UINT_PACK32;
        case AttachmentFormat::A2R10G10B10_SINT_PACK32:    return VK_FORMAT_A2R10G10B10_SINT_PACK32;
        case AttachmentFormat::A2B10G10R10_UNORM_PACK32:   return VK_FORMAT_A2B10G10R10_UNORM_PACK32;
        case AttachmentFormat::A2B10G10R10_SNORM_PACK32:   return VK_FORMAT_A2B10G10R10_SNORM_PACK32;
        case AttachmentFormat::A2B10G10R10_USCALED_PACK32: return VK_FORMAT_A2B10G10R10_USCALED_PACK32;
        case AttachmentFormat::A2B10G10R10_SSCALED_PACK32: return VK_FORMAT_A2B10G10R10_SSCALED_PACK32;
        case AttachmentFormat::A2B10G10R10_UINT_PACK32:    return VK_FORMAT_A2B10G10R10_UINT_PACK32;
        case AttachmentFormat::A2B10G10R10_SINT_PACK32:    return VK_FORMAT_A2B10G10R10_SINT_PACK32;
        case AttachmentFormat::R16_UNORM:                  return VK_FORMAT_R16_UNORM;
        case AttachmentFormat::R16_SNORM:                  return VK_FORMAT_R16_SNORM;
        case AttachmentFormat::R16_USCALED:                return VK_FORMAT_R16_USCALED;
        case AttachmentFormat::R16_SSCALED:                return VK_FORMAT_R16_SSCALED;
        case AttachmentFormat::R16_UINT:                   return VK_FORMAT_R16_UINT;
        case AttachmentFormat::R16_SINT:                   return VK_FORMAT_R16_SINT;
        case AttachmentFormat::R16_SFLOAT:                 return VK_FORMAT_R16_SFLOAT;
        case AttachmentFormat::R16G16_UNORM:               return VK_FORMAT_R16G16_UNORM;
        case AttachmentFormat::R16G16_SNORM:               return VK_FORMAT_R16G16_SNORM;
        case AttachmentFormat::R16G16_USCALED:             return VK_FORMAT_R16G16_USCALED;
        case AttachmentFormat::R16G16_SSCALED:             return VK_FORMAT_R16G16_SSCALED;
        case AttachmentFormat::R16G16_UINT:                return VK_FORMAT_R16G16_UINT;
        case AttachmentFormat::R16G16_SINT:                return VK_FORMAT_R16G16_SINT;
        case AttachmentFormat::R16G16_SFLOAT:              return VK_FORMAT_R16G16_SFLOAT;
        case AttachmentFormat::R16G16B16_UNORM:            return VK_FORMAT_R16G16B16_UNORM;
        case AttachmentFormat::R16G16B16_SNORM:            return VK_FORMAT_R16G16B16_SNORM;
        case AttachmentFormat::R16G16B16_USCALED:          return VK_FORMAT_R16G16B16_USCALED;
        case AttachmentFormat::R16G16B16_SSCALED:          return VK_FORMAT_R16G16B16_SSCALED;
        case AttachmentFormat::R16G16B16_UINT:             return VK_FORMAT_R16G16B16_UINT;
        case AttachmentFormat::R16G16B16_SINT:             return VK_FORMAT_R16G16B16_SINT;
        case AttachmentFormat::R16G16B16_SFLOAT:           return VK_FORMAT_R16G16B16_SFLOAT;
        case AttachmentFormat::R16G16B16A16_UNORM:         return VK_FORMAT_R16G16B16A16_UNORM;
        case AttachmentFormat::R16G16B16A16_SNORM:         return VK_FORMAT_R16G16B16A16_SNORM;
        case AttachmentFormat::R16G16B16A16_USCALED:       return VK_FORMAT_R16G16B16A16_USCALED;
        case AttachmentFormat::R16G16B16A16_SSCALED:       return VK_FORMAT_R16G16B16A16_SSCALED;
        case AttachmentFormat::R16G16B16A16_UINT:          return VK_FORMAT_R16G16B16A16_UINT;
        case AttachmentFormat::R16G16B16A16_SINT:          return VK_FORMAT_R16G16B16A16_SINT;
        case AttachmentFormat::R16G16B16A16_SFLOAT:        return VK_FORMAT_R16G16B16A16_SFLOAT;
        case AttachmentFormat::R32_UINT:                   return VK_FORMAT_R32_UINT;
        case AttachmentFormat::R32_SINT:                   return VK_FORMAT_R32_SINT;
        case AttachmentFormat::R32_SFLOAT:                 return VK_FORMAT_R32_SFLOAT;
        case AttachmentFormat::R32G32_UINT:                return VK_FORMAT_R32G32_UINT;
        case AttachmentFormat::R32G32_SINT:                return VK_FORMAT_R32G32_SINT;
        case AttachmentFormat::R32G32_SFLOAT:              return VK_FORMAT_R32G32_SFLOAT;
        case AttachmentFormat::R32G32B32_UINT:             return VK_FORMAT_R32G32B32_UINT;
        case AttachmentFormat::R32G32B32_SINT:             return VK_FORMAT_R32G32B32_SINT;
        case AttachmentFormat::R32G32B32_SFLOAT:           return VK_FORMAT_R32G32B32_SFLOAT;
        case AttachmentFormat::R32G32B32A32_UINT:          return VK_FORMAT_R32G32B32A32_UINT;
        case AttachmentFormat::R32G32B32A32_SINT:          return VK_FORMAT_R32G32B32A32_SINT;
        case AttachmentFormat::R32G32B32A32_SFLOAT:        return VK_FORMAT_R32G32B32A32_SFLOAT;
        case AttachmentFormat::R64_UINT:                   return VK_FORMAT_R64_UINT;
        case AttachmentFormat::R64_SINT:                   return VK_FORMAT_R64_SINT;
        case AttachmentFormat::R64_SFLOAT:                 return VK_FORMAT_R64_SFLOAT;
        case AttachmentFormat::R64G64_UINT:                return VK_FORMAT_R64G64_UINT;
        case AttachmentFormat::R64G64_SINT:                return VK_FORMAT_R64G64_SINT;
        case AttachmentFormat::R64G64_SFLOAT:              return VK_FORMAT_R64G64_SFLOAT;
        case AttachmentFormat::R64G64B64_UINT:             return VK_FORMAT_R64G64B64_UINT;
        case AttachmentFormat::R64G64B64_SINT:             return VK_FORMAT_R64G64B64_SINT;
        case AttachmentFormat::R64G64B64_SFLOAT:           return VK_FORMAT_R64G64B64_SFLOAT;
        case AttachmentFormat::R64G64B64A64_UINT:          return VK_FORMAT_R64G64B64A64_UINT;
        case AttachmentFormat::R64G64B64A64_SINT:          return VK_FORMAT_R64G64B64A64_SINT;
        case AttachmentFormat::R64G64B64A64_SFLOAT:        return VK_FORMAT_R64G64B64A64_SFLOAT;
        case AttachmentFormat::B10G11R11_UFLOAT_PACK32:    return VK_FORMAT_B10G11R11_UFLOAT_PACK32;
        case AttachmentFormat::E5B9G9R9_UFLOAT_PACK32:     return VK_FORMAT_E5B9G9R9_UFLOAT_PACK32;
        case AttachmentFormat::D16_UNORM:                  return VK_FORMAT_D16_UNORM;
        case AttachmentFormat::X8_D24_UNORM_PACK32:        return VK_FORMAT_X8_D24_UNORM_PACK32;
        case AttachmentFormat::D32_SFLOAT:                 return VK_FORMAT_D32_SFLOAT;
        case AttachmentFormat::S8_UINT:                    return VK_FORMAT_S8_UINT;
        case AttachmentFormat::D16_UNORM_S8_UINT:          return VK_FORMAT_D16_UNORM_S8_UINT;
        case AttachmentFormat::D24_UNORM_S8_UINT:          return VK_FORMAT_D24_UNORM_S8_UINT;
        case AttachmentFormat::D32_SFLOAT_S8_UINT:         return VK_FORMAT_D32_SFLOAT_S8_UINT;
        default:
            return (VkFormat)-1;
        }
    }

    static VkAttachmentLoadOp GetVulkanAttachmentLoadCommand(AttachmentLoadCommand command)
    {
        switch (command)
        {
        case AttachmentLoadCommand::Load: return VK_ATTACHMENT_LOAD_OP_LOAD;
        case AttachmentLoadCommand::Clear: return VK_ATTACHMENT_LOAD_OP_CLEAR;
        case AttachmentLoadCommand::DontCare: return VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        }
    }

    static VkAttachmentStoreOp GetVulkanAttachmentStoreCommand(AttachmentStoreCommand command)
    {
        switch (command)
        {
        case AttachmentStoreCommand::Store: return VK_ATTACHMENT_STORE_OP_STORE;
        case AttachmentStoreCommand::None: return VK_ATTACHMENT_STORE_OP_NONE;
        case AttachmentStoreCommand::DontCare: return VK_ATTACHMENT_STORE_OP_DONT_CARE;
        }
    }

    static bool IsColorFormat(AttachmentFormat format)
    {
        switch (format)
        {
        case AttachmentFormat::D16_UNORM:
        case AttachmentFormat::X8_D24_UNORM_PACK32:
        case AttachmentFormat::D32_SFLOAT:
        case AttachmentFormat::S8_UINT:
        //case AttachmentFormat::D16_UNORM_S8_UINT:
        //case AttachmentFormat::D24_UNORM_S8_UINT:
            return false;
        default:
            return true;
        }
    }

    static bool IsDepthFormat(AttachmentFormat format)
    {
        switch (format)
        {
        case AttachmentFormat::D16_UNORM:
        case AttachmentFormat::X8_D24_UNORM_PACK32:
        case AttachmentFormat::D32_SFLOAT:
            return true;
        default:
            return false;
        }
    }

    static bool IsStencilFormat(AttachmentFormat format)
    {
        switch (format)
        {
        case AttachmentFormat::S8_UINT:
            return true;
        default:
            return false;
        }
    }

    static bool IsDepthStencilFormat(AttachmentFormat format)
    {
        switch (format)
        {
        case AttachmentFormat::D16_UNORM:
        case AttachmentFormat::X8_D24_UNORM_PACK32:
        case AttachmentFormat::D32_SFLOAT:
        case AttachmentFormat::S8_UINT:
        case AttachmentFormat::D16_UNORM_S8_UINT:
        case AttachmentFormat::D24_UNORM_S8_UINT:
            return true;
        default:
            return false;
        }
    }

    static VkImageLayout GetVulkanAttachmentLayout(AttachmentLayout layout, AttachmentFormat format)
    {
        if (layout == AttachmentLayout::General)
            return VK_IMAGE_LAYOUT_GENERAL;
        if (layout == AttachmentLayout::Present)
            return VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
    
        if (IsColorFormat(format))
        {
            switch (layout)
            {
            case AttachmentLayout::ReadWrite: return VK_IMAGE_LAYOUT_ATTACHMENT_OPTIMAL;
            case AttachmentLayout::Read:
                std::cout << "Attachment cannot be a color attachment and have a read only layout\n";
                std::exit(1);
                break;
            case AttachmentLayout::ReadDepth_ReadWriteStencil:
            case AttachmentLayout::ReadWriteDepth_ReadStencil:
                std::cout << "Attachment cannot be a color attachment and have a depth stencil layout\n";
                std::exit(1);
                break;
            }
        }
        else if (IsDepthFormat(format))
        {
            switch (layout)
            {
            case AttachmentLayout::ReadWrite: return VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL;
            case AttachmentLayout::Read:      return VK_IMAGE_LAYOUT_DEPTH_READ_ONLY_OPTIMAL;
            case AttachmentLayout::ReadDepth_ReadWriteStencil:
            case AttachmentLayout::ReadWriteDepth_ReadStencil:
                std::cout << "Attachment cannot be a depth attachment and have a depth stencil layout\n";
                std::exit(1);
                break;
            }
        }
        else if (IsStencilFormat(format))
        {
            switch (layout)
            {
            case AttachmentLayout::ReadWrite: return VK_IMAGE_LAYOUT_STENCIL_ATTACHMENT_OPTIMAL;
            case AttachmentLayout::Read:      return VK_IMAGE_LAYOUT_STENCIL_READ_ONLY_OPTIMAL;
            case AttachmentLayout::ReadDepth_ReadWriteStencil:
            case AttachmentLayout::ReadWriteDepth_ReadStencil:
                std::cout << "Attachment cannot be a stencil attachment and have a depth stencil layout\n";
                std::exit(1);
                break;
            }
        }
        else if (IsDepthStencilFormat(format))
        {
            switch (layout)
            {
            case AttachmentLayout::ReadWrite: return VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
            case AttachmentLayout::Read:      return VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL;
            case AttachmentLayout::ReadDepth_ReadWriteStencil: return VK_IMAGE_LAYOUT_DEPTH_READ_ONLY_STENCIL_ATTACHMENT_OPTIMAL;
            case AttachmentLayout::ReadWriteDepth_ReadStencil: return VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_STENCIL_READ_ONLY_OPTIMAL;
            }
        }
    }
    
    VkPipelineStageFlags GetVulkanPipelineStage(PipelineStage stage)
    {
        switch (stage)
        {
        case PipelineStage::VertexInput:                  return VK_PIPELINE_STAGE_VERTEX_INPUT_BIT;
        case PipelineStage::VertexShader:                 return VK_PIPELINE_STAGE_VERTEX_SHADER_BIT;
        case PipelineStage::TessellationControlShader:    return VK_PIPELINE_STAGE_TESSELLATION_CONTROL_SHADER_BIT;
        case PipelineStage::TessellationEvaluationShader: return VK_PIPELINE_STAGE_TESSELLATION_EVALUATION_SHADER_BIT;
        case PipelineStage::GeometryShader:               return VK_PIPELINE_STAGE_GEOMETRY_SHADER_BIT;
        case PipelineStage::FragmentShader:               return VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
        case PipelineStage::EarlyFragmentTests:           return VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
        case PipelineStage::LateFragmentTests:            return VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
        case PipelineStage::ColorAttachmentOutput:        return VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        }
    }
    
    static VkAccessFlags GetVulkanAccessFlags(AttachmentAccess access)
    {
        switch (access)
        {
        case AttachmentAccess::ColorRead:         return VK_ACCESS_COLOR_ATTACHMENT_READ_BIT;
        case AttachmentAccess::ColorWrite:        return VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
        case AttachmentAccess::DepthStencilRead:  return VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT;
        case AttachmentAccess::DepthStencilWrite: return VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
        }
    }

	RenderPassImpl::RenderPassImpl(RenderContextImpl* renderContext, ArrayView<RenderPassAttachment> attachments, ArrayView<RenderSubpass> subpasses, ArrayView<RenderSubpassDependency> dependencies)
        : renderContext(renderContext), subpasses(subpasses)
	{
        Array<VkAttachmentDescription> attachmentDescriptions { attachments.Count()};        

        for (int i = 0; i < attachments.Count(); ++i)
        {                                    
            attachmentDescriptions[i].format = GetVulkanAttachmentFormat(attachments[i].format);
            attachmentDescriptions[i].samples = VK_SAMPLE_COUNT_1_BIT;
            attachmentDescriptions[i].loadOp = GetVulkanAttachmentLoadCommand(attachments[i].loadCommand);
            attachmentDescriptions[i].storeOp = GetVulkanAttachmentStoreCommand(attachments[i].storeCommand);
            attachmentDescriptions[i].stencilLoadOp = GetVulkanAttachmentLoadCommand(attachments[i].stencilLoadCommand);
            attachmentDescriptions[i].stencilStoreOp = GetVulkanAttachmentStoreCommand(attachments[i].stencilStoreCommand);
            attachmentDescriptions[i].initialLayout = GetVulkanAttachmentLayout(attachments[i].initialLayout, attachments[i].format);
            attachmentDescriptions[i].finalLayout = GetVulkanAttachmentLayout(attachments[i].finalLayout, attachments[i].format);
        }

        Array<VkSubpassDescription> subpassDescriptions { subpasses.Count()};
        Array<Array< VkAttachmentReference>> subpassesAttachmentReferences;

        for (int i = 0; i < subpasses.Count(); ++i)
        {
            subpassesAttachmentReferences[i].Resize(subpasses[i].usedAttachments.Count());

            for (int j = 0; j < subpasses[i].usedAttachments.Count(); ++j)
            {
                unsigned index = subpasses[i].usedAttachments[j].renderPassAttachmentIndex;

                subpassesAttachmentReferences[i][j].attachment = index;
                subpassesAttachmentReferences[i][j].layout = GetVulkanAttachmentLayout(subpasses[i].usedAttachments[j].layout, attachments[index].format);
            }
            
            subpassDescriptions[i].pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
            subpassDescriptions[i].colorAttachmentCount = subpassesAttachmentReferences[i].Count();
            subpassDescriptions[i].pColorAttachments = subpassesAttachmentReferences[i].Ptr();             
        }

        Array<VkSubpassDependency> subpassDependencies { dependencies.Count() };

        for (int i = 0; i < dependencies.Count(); ++i)
        {
            uint32_t subpassIndex;

            if (dependencies[i].waitTarget.subpassIndex == -1)
                subpassIndex = VK_SUBPASS_EXTERNAL;
            else
                subpassIndex = dependencies[i].waitTarget.subpassIndex;

            subpassDependencies[0].srcSubpass = 
            subpassDependencies[0].srcStageMask = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;;
            subpassDependencies[0].srcAccessMask = GetVulkanAccessFlags(dependencies[i].waitTarget.commandAccess);
            
            subpassDependencies[0].dstSubpass = dependencies[i].executeTarget.subpassIndex;
            subpassDependencies[0].dstStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;;
            subpassDependencies[0].dstAccessMask = GetVulkanAccessFlags(dependencies[i].executeTarget.commandAccess);
        }

        VkRenderPassCreateInfo renderPassInfo{};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        renderPassInfo.attachmentCount = attachmentDescriptions.Count();
        renderPassInfo.pAttachments = attachmentDescriptions.Ptr();
        renderPassInfo.subpassCount = subpassDescriptions.Count();
        renderPassInfo.pSubpasses = subpassDescriptions.Ptr();
        renderPassInfo.dependencyCount = subpassDependencies.Count();
        renderPassInfo.pDependencies = subpassDependencies.Ptr();

        if (vkCreateRenderPass(renderContext->device, &renderPassInfo, nullptr, &renderPass) != VK_SUCCESS) {
            std::cout << "failed to create render pass!";
            std::exit(1);
        }        
	}
	RenderPassImpl::~RenderPassImpl()
	{
        vkDestroyRenderPass(renderContext->device, renderPass, nullptr);
	}
}