#include "Vulkan_RenderContext.h"
#include "Vulkan_Instance.h"
#include <iostream>

namespace Graphics2
{
    struct QueueFamilyIndices {
        std::optional<uint32_t> graphicsFamily;
        std::optional<uint32_t> presentFamily;

        bool isComplete() {
            return graphicsFamily.has_value() && presentFamily.has_value();
        }
    };

    QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice device, void* surface)
    {
        QueueFamilyIndices indices;

        uint32_t queueFamilyCount = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

        Array<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.Ptr());


        int i = 0;
        for (const auto& queueFamily : queueFamilies)
        {
            if (surface != nullptr)
            {
                VkBool32 presentSupport = false;
                vkGetPhysicalDeviceSurfaceSupportKHR(device, i, (VkSurfaceKHR)surface, &presentSupport);

                if (presentSupport) {
                    indices.presentFamily = i;
                }
            }

            if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
                indices.graphicsFamily = i;
            }

            if (indices.isComplete()) {
                break;
            }

            i++;
        }

        return indices;
    }

    bool CheckDeviceExtensionSupport(VkPhysicalDevice device, const Array<const char*>& deviceExtensions)
    {
        uint32_t extensionCount;
        vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

        Array<VkExtensionProperties> availableExtensions(extensionCount);
        vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.Ptr());

        Set<StringView> requiredExtensions;

        for (auto& ext : deviceExtensions)
            requiredExtensions.Insert((StringView)ext);

        for (const auto& extension : availableExtensions)
            requiredExtensions.Erase(extension.extensionName);

        return requiredExtensions.Empty();
    }

	Vulkan_RenderContext::Vulkan_RenderContext()
	{
		if (vulkanInstance.initRenderWindow == VK_NULL_HANDLE)
		{
			CreateVulkanInstance(true);
			++vulkanInstance.contextCount;
		}

        //Choosing a physical device
        uint32_t deviceCount = 0;
        vkEnumeratePhysicalDevices(vulkanInstance.instance, &deviceCount, nullptr);

        if (deviceCount == 0)
        {
            std::cout << "failed to find GPUs with Vulkan support!";
            std::exit(1);
        }

        Array<VkPhysicalDevice> devices(deviceCount);
        vkEnumeratePhysicalDevices(vulkanInstance.instance, &deviceCount, devices.Ptr());

        const Array<const char*> deviceExtensions = {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME
        };

        QueueFamilyIndices indices;
        for (const auto& device : devices)
        {
            indices = FindQueueFamilies(device, vulkanInstance.surface);

            if (!indices.isComplete())
                continue;

            if (!CheckDeviceExtensionSupport(device, deviceExtensions))
                continue;

            physicalDevice = device;
            break;
        }

        if (physicalDevice == VK_NULL_HANDLE)
        {
            std::cout << "failed to find a suitable GPU!\n";
            std::exit(1);
        }

        graphicsQueueFamily = indices.graphicsFamily.value();
        presentQueueFamily = indices.presentFamily.value();

        //Creating a logical device    
        Array<VkDeviceQueueCreateInfo> queueCreateInfos;
        Set<uint32_t> uniqueQueueFamilies = { graphicsQueueFamily, presentQueueFamily };

        float queuePriority = 1.0f;
        for (uint32_t queueFamily : uniqueQueueFamilies) {
            VkDeviceQueueCreateInfo queueCreateInfo{};
            queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            queueCreateInfo.queueFamilyIndex = queueFamily;
            queueCreateInfo.queueCount = 1;
            queueCreateInfo.pQueuePriorities = &queuePriority;
            queueCreateInfos.AddBack(queueCreateInfo);
        }

        VkPhysicalDeviceFeatures deviceFeatures{};

        VkDeviceCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        createInfo.pQueueCreateInfos = queueCreateInfos.Ptr();
        createInfo.queueCreateInfoCount = queueCreateInfos.Count();
        createInfo.pEnabledFeatures = &deviceFeatures;
        createInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.Count());
        createInfo.ppEnabledExtensionNames = deviceExtensions.Ptr();
        createInfo.enabledLayerCount = 0;
#ifdef _DEBUG
        createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.Count());
        createInfo.ppEnabledLayerNames = validationLayers.Ptr();
#endif        

        if (vkCreateDevice(physicalDevice, &createInfo, nullptr, &device) != VK_SUCCESS) {
            std::cout << "failed to create logical device!\n";
            std::exit(1);
        }

        vkGetDeviceQueue(device, presentQueueFamily, 0, &presentQueue);
	}
	Vulkan_RenderContext::~Vulkan_RenderContext()
	{
        vkDestroyDevice(device, nullptr);

		--vulkanInstance.contextCount;

		if (vulkanInstance.contextCount == 0)
			FreeVulkanInstance();
	}
}