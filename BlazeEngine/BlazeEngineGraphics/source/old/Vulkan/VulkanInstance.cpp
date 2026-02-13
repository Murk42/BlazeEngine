#include "pch.h"
#include "BlazeEngineGraphics\Vulkan/Core/VulkanInstance.h"

namespace Blaze::Graphics::Vulkan
{
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
            Debug::Logger::LogError("Vulkan API", pCallbackData->pMessage);

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

    VulkanInstance::VulkanInstance()
        : instance(VK_NULL_HANDLE)
#ifdef BLAZE_VULKAN_VALIDATION_LAYERS
        , debugMessenger(VK_NULL_HANDLE)
#endif
    {
        VkApplicationInfo appInfo{};
        appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        appInfo.pApplicationName = "Hello Triangle";
        appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.pEngineName = "No Engine";
        appInfo.engineVersion = VK_MAKE_VERSION(1, 3, 0);
        appInfo.apiVersion = VK_API_VERSION_1_3;

        Array<const char*> extensions;

#ifdef BLAZE_VULKAN_VALIDATION_LAYERS
        extensions.AddBack(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);                
#endif          

        bool windowSupport = true;
        if (windowSupport)
        {
            initWindow = WindowSDL("", Vec2i(INT_MAX), Vec2u(640, 480), WindowHandleGraphicsaAPI::Vulkan);

            if (initWindow.IsNullWindow())
                Debug::Logger::LogFatal("Blaze Vulkan API", "SDL_CreateWindow failed: \"" + (String)SDL_GetError() + "\"\n");                            

            uint32_t windowExtensionCount = 0;
            SDL_Vulkan_GetInstanceExtensions((SDL_Window*)initWindow.GetHandle(), &windowExtensionCount, nullptr);
            Array<const char*> windowExtensions { windowExtensionCount };
            SDL_Vulkan_GetInstanceExtensions((SDL_Window*)initWindow.GetHandle(), &windowExtensionCount, windowExtensions.Ptr());

            extensions.Append(std::move(windowExtensions));
        }

        VkInstanceCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        createInfo.pApplicationInfo = &appInfo;
        createInfo.enabledExtensionCount = extensions.Count();
        createInfo.ppEnabledExtensionNames = extensions.Ptr();
        createInfo.enabledLayerCount = 0;

#ifdef BLAZE_VULKAN_VALIDATION_LAYERS
        VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
        debugCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
        debugCreateInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
        debugCreateInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
        debugCreateInfo.pfnUserCallback = debugCallback;
        debugCreateInfo.pUserData = nullptr; // Optional    

        if (!CheckValidationLayerSupport(validationLayers)) 
            Debug::Logger::LogFatal("Blaze Vulkan API", "Validation layers requested, but not available!\n");            

        createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.Count());
        createInfo.ppEnabledLayerNames = validationLayers.Ptr();
        createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debugCreateInfo;
#endif

        if (vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS)
            Debug::Logger::LogFatal("Blaze Vulkan API", "Failed to create vulkan instance");            

#ifdef BLAZE_VULKAN_VALIDATION_LAYERS
        if (CreateDebugUtilsMessengerEXT((VkInstance)instance, &debugCreateInfo, nullptr, &debugMessenger) != VK_SUCCESS) 
            Debug::Logger::LogFatal("Blaze Vulkan API", "Failed to set up debug messenger!\n");            
#endif        
    }

    VulkanInstance::VulkanInstance(VulkanInstance&& other) noexcept
        : initWindow(std::move(other.initWindow)), instance(VK_NULL_HANDLE)
#ifdef BLAZE_VULKAN_VALIDATION_LAYERS
        , debugMessenger(VK_NULL_HANDLE)
#endif
    {
        instance = other.instance;
        other.instance = VK_NULL_HANDLE;

#ifdef BLAZE_VULKAN_VALIDATION_LAYERS
        debugMessenger = other.debugMessenger;
        other.debugMessenger = VK_NULL_HANDLE;
#endif
    }

    VulkanInstance::~VulkanInstance()
    {
        Destroy();
    }

    void VulkanInstance::Destroy()
    {
#ifdef BLAZE_VULKAN_VALIDATION_LAYERS
        DestroyDebugUtilsMessengerEXT(instance, debugMessenger, nullptr);
#endif
        vkDestroyInstance(instance, nullptr);

        initWindow.Destroy();

        instance = NULL;
#ifdef BLAZE_VULKAN_VALIDATION_LAYERS
        debugMessenger = NULL;
#endif        
    }

    WindowSDL VulkanInstance::CreateWindow(StringView title, Vec2i pos, Vec2u size)
    {
        if (initWindow.IsNullWindow())
            return WindowSDL(title, pos, size, WindowHandleGraphicsaAPI::Vulkan);
        else
            return std::move(initWindow);        
    }

    void VulkanInstance::FreeWindow(WindowSDL& window)
    {        
        window.Destroy();
    }    

    VulkanInstance& VulkanInstance::operator=(VulkanInstance&& other) noexcept
    {
        Destroy();

        initWindow = std::move(other.initWindow);

        instance = other.instance;
        other.instance = VK_NULL_HANDLE;

#ifdef BLAZE_VULKAN_VALIDATION_LAYERS
        debugMessenger = other.debugMessenger;
        other.debugMessenger = VK_NULL_HANDLE;
#endif

        return *this;
    }
}
