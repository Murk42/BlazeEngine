#include "Vulkan_Instance.h"
#include <iostream>
#include "Vulkan_Instance.h"

namespace Graphics2
{
    VulkanInstance vulkanInstance;

    bool CheckValidationLayerSupport(const Array<const char*>& validationLayers) {
        uint32_t layerCount;
        vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
        Array<VkLayerProperties> availableLayers(layerCount);
        vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.Ptr());

        for (const char* layerName : validationLayers) {
            bool layerFound = false;

            for (const auto& layerProperties : availableLayers) {
                if (strcmp(layerName, layerProperties.layerName) == 0) {
                    layerFound = true;
                    break;
                }
            }

            if (!layerFound) {
                return false;
            }
        }

        return true;
    }

    static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
        VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
        VkDebugUtilsMessageTypeFlagsEXT messageType,
        const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
        void* pUserData)
    {
        if (messageSeverity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)
        {
            std::cerr << "validation layer: " << pCallbackData->pMessage << "\n\n";
        }

        return VK_FALSE;
    }

    VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger) {
        auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
        if (func != nullptr) {
            return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
        }
        else {
            return VK_ERROR_EXTENSION_NOT_PRESENT;
        }
    }

    void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator) {
        auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
        if (func != nullptr) {
            func(instance, debugMessenger, pAllocator);
        }
    }

    void CreateVulkanInstance(bool windowSupport)
    {
        VkApplicationInfo appInfo{};
        appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        appInfo.pApplicationName = "Hello Triangle";
        appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.pEngineName = "No Engine";
        appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.apiVersion = VK_API_VERSION_1_0;

        Array<const char*> extensions;

#ifdef _DEBUG        
        extensions.AddBack(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
#endif          

        if (windowSupport)
        {
            vulkanInstance.initRenderWindow = SDL_CreateWindow("Vulkan test!", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 420,
                SDL_WINDOW_VULKAN | SDL_WINDOW_HIDDEN | SDL_WINDOW_RESIZABLE);

            if (vulkanInstance.initRenderWindow == nullptr)
            {
                std::cout << "SDL_CreateWindow failed: \"" << SDL_GetError() << "\"\n";
                std::exit(1);
            }

            uint32_t windowExtensionCount = 0;
            SDL_Vulkan_GetInstanceExtensions(vulkanInstance.initRenderWindow, &windowExtensionCount, nullptr);
            Array<const char*> windowExtensions { windowExtensionCount };
            SDL_Vulkan_GetInstanceExtensions(vulkanInstance.initRenderWindow, &windowExtensionCount, windowExtensions.Ptr());

            extensions.Append(std::move(windowExtensions));
        }

        VkInstanceCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        createInfo.pApplicationInfo = &appInfo;
        createInfo.enabledExtensionCount = extensions.Count();
        createInfo.ppEnabledExtensionNames = extensions.Ptr();
        createInfo.enabledLayerCount = 0;

#ifdef _DEBUG
        VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
        debugCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
        debugCreateInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
        debugCreateInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
        debugCreateInfo.pfnUserCallback = debugCallback;
        debugCreateInfo.pUserData = nullptr; // Optional    

        if (!CheckValidationLayerSupport(validationLayers)) {
            std::cout << "Validation layers requested, but not available!\n";
            std::exit(1);
        }

        createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.Count());
        createInfo.ppEnabledLayerNames = validationLayers.Ptr();
        createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debugCreateInfo;

#endif

        if (vkCreateInstance(&createInfo, nullptr, &vulkanInstance.instance) != VK_SUCCESS)
        {
            std::cout << "Failed to create vulkan instance";
            std::exit(1);
        }

#ifdef _DEBUG
        if (CreateDebugUtilsMessengerEXT((VkInstance)vulkanInstance.instance, &debugCreateInfo, nullptr, &vulkanInstance.debugMessenger) != VK_SUCCESS) {
            std::cout << "Failed to set up debug messenger!\n";
            std::exit(1);
        }
#endif

        if (windowSupport)
        {
            if (SDL_Vulkan_CreateSurface(vulkanInstance.initRenderWindow, vulkanInstance.instance, &vulkanInstance.surface) == SDL_FALSE)
            {
                std::cout << "SDL failed to create vulkan surface: \"" << SDL_GetError() << "\"\n";
                std::exit(1); 
            }
        }
    }
    void FreeVulkanInstance()
    {
        vkDestroySurfaceKHR(vulkanInstance.instance, vulkanInstance.surface, nullptr);

#ifdef _DEBUG
        DestroyDebugUtilsMessengerEXT(vulkanInstance.instance, vulkanInstance.debugMessenger, nullptr);
#endif
        vkDestroyInstance(vulkanInstance.instance, nullptr);

        SDL_DestroyWindow(vulkanInstance.initRenderWindow);
    }
}
