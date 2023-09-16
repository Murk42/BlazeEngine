#include "Vulkan_Shader.h"
#include "Vulkan_Instance.h"
#include <iostream>
#include <fstream>

namespace Experimental::Graphics::Vulkan
{
	ShaderImpl::ShaderImpl(RenderContextImpl* renderContext, const Path& path)
		: renderContext(renderContext)
	{
        std::ifstream file(path.GetString().Ptr(), std::ios::ate | std::ios::binary);

        if (!file.is_open()) {
            std::cout << "failed to open file!\n";
            std::exit(1);
        }

        size_t fileSize = (size_t)file.tellg();
        std::vector<char> buffer(fileSize);

        file.seekg(0);
        file.read(buffer.data(), fileSize);

        file.close();

        VkShaderModuleCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        createInfo.codeSize = buffer.size();
        createInfo.pCode = reinterpret_cast<const uint32_t*>(buffer.data());

        if (vkCreateShaderModule(renderContext->device, &createInfo, nullptr, (VkShaderModule*)&shader) != VK_SUCCESS)
        {
            std::cout << "failed to create shader module!";
            std::exit(1);
        }
	}
	ShaderImpl::~ShaderImpl()
	{
        vkDestroyShaderModule(renderContext->device, shader, nullptr);
	}
}