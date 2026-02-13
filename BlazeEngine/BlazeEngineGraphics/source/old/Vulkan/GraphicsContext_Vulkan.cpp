#include "pch.h"
#include "BlazeEngineGraphics\Vulkan/Core/GraphicsContext_Vulkan.h"
#include "BlazeEngineGraphics\Vulkan/Core/VulkanInstance.h"

namespace Blaze::Graphics::Vulkan
{

	std::unique_ptr<VulkanInstance> vulkanInstance;

	struct QueueFamilyIndices {
		std::optional<uint32_t> graphicsFamily;
		std::optional<uint32_t> presentFamily;

		bool isComplete() {
			return graphicsFamily.has_value() && presentFamily.has_value();
		}
	};

	static QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice device, VkSurfaceKHR surface)
	{
		QueueFamilyIndices indices;

		uint32_t queueFamilyCount = 0;
		vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

		Array<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
		vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.Ptr());

		int i = 0;
		for (const auto& queueFamily : queueFamilies)
		{
			if (surface != VK_NULL_HANDLE)
			{
				VkBool32 presentSupport = false;
				vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &presentSupport);

				if (presentSupport) {
					indices.presentFamily = i;
				}
			}

			if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
				indices.graphicsFamily = i;


			if (indices.isComplete()) {
				break;
			}

			i++;
		}

		return indices;
	}

	static bool CheckDeviceExtensionSupport(VkPhysicalDevice device, Set<String> deviceExtensions)
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

	GraphicsContext_Vulkan::GraphicsContext_Vulkan()
		: GraphicsContext_Vulkan(GraphicsContextProperties_Vulkan())
	{
	}

	GraphicsContext_Vulkan::GraphicsContext_Vulkan(const GraphicsContextProperties_Vulkan& properties)
	{
		if (vulkanInstance == nullptr)
			vulkanInstance = std::make_unique<VulkanInstance>();

		initWindow = vulkanInstance->CreateWindow("", Vec2i(INT_MAX), Vec2u(0, 0));

		if (SDL_Vulkan_CreateSurface((SDL_Window*)initWindow.GetHandle(), vulkanInstance->GetHandle(), &initSurface) == SDL_FALSE)
			Debug::Logger::LogFatal("Blaze Engine", "SDL failed to create vulkan surface: \"" + (String)SDL_GetError() + "\"\n");

		//Choosing a physical device
		uint32_t deviceCount = 0;
		vkEnumeratePhysicalDevices(vulkanInstance->GetHandle(), &deviceCount, nullptr);

		if (deviceCount == 0)
			Debug::Logger::LogFatal("Blaze Engine", "Failed to find GPUs with Vulkan support!");

		Array<VkPhysicalDevice> devices(deviceCount);
		vkEnumeratePhysicalDevices(vulkanInstance->GetHandle(), &deviceCount, devices.Ptr());

		Set<String> extensions;
		extensions.Insert(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
		extensions.Insert(VK_KHR_TIMELINE_SEMAPHORE_EXTENSION_NAME);
		
		QueueFamilyIndices indices;
		for (const auto& device : devices)
		{
			indices = FindQueueFamilies(device, initSurface);

			if (!indices.isComplete())
				continue;

			if (!CheckDeviceExtensionSupport(device, extensions))
				continue;

			physicalDevice = device;
			break;
		}

		if (physicalDevice == VK_NULL_HANDLE)
		{
			Debug::Logger::LogFatal("Blaze Engine", "failed to find a suitable GPU!\n");
			return;
		}

		enabledExtensions = std::move(extensions);

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

		VkPhysicalDeviceFeatures deviceFeatures{ };		
		VkPhysicalDeviceVulkan12Features deviceFeatures12{ };
		deviceFeatures12.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES;
		deviceFeatures12.pNext = nullptr;
		deviceFeatures12.timelineSemaphore = true;


		Array<const char*> _enabledExtensions{ enabledExtensions.Count() };

		{
			auto src = enabledExtensions.FirstIterator();
			auto dst = _enabledExtensions.FirstIterator();
			auto end = enabledExtensions.BehindIterator();

			while (src != end)
			{
				*dst = src->Ptr();
				++src;
				++dst;
			}
		}

		VkDeviceCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;				
		createInfo.pNext = &deviceFeatures12;
		createInfo.pQueueCreateInfos = queueCreateInfos.Ptr();
		createInfo.queueCreateInfoCount = queueCreateInfos.Count();
		createInfo.pEnabledFeatures = &deviceFeatures;
		createInfo.enabledExtensionCount = _enabledExtensions.Count();
		createInfo.ppEnabledExtensionNames = _enabledExtensions.Ptr();
		createInfo.enabledLayerCount = 0;
#ifdef BLAZE_VULKAN_VALIDATION_LAYERS
		createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.Count());
		createInfo.ppEnabledLayerNames = validationLayers.Ptr();
#endif        

		if (vkCreateDevice(physicalDevice, &createInfo, nullptr, &device) != VK_SUCCESS)
			Debug::Logger::LogFatal("Blaze Engine", "failed to create logical device!\n");					

		vkGetDeviceQueue(device, graphicsQueueFamily, 0, &graphicsQueue);
		//Creating command pool
		{
			VkCommandPoolCreateInfo poolInfo{};
			poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
			poolInfo.flags = 0;
			poolInfo.queueFamilyIndex = graphicsQueueFamily;

			if (vkCreateCommandPool(device, &poolInfo, nullptr, &commandPool) != VK_SUCCESS)
				Debug::Logger::LogError("BlazeEngine", "Failed to create command pool!\n");
		}
	}
	GraphicsContext_Vulkan::~GraphicsContext_Vulkan()
	{
		if (device == NULL)
			return;		

		vkDestroyCommandPool(device, commandPool, nullptr);

		vkDestroyDevice(device, nullptr);

		vkDestroySurfaceKHR(vulkanInstance->GetHandle(), initSurface, nullptr);

		vulkanInstance->FreeWindow(initWindow);

		device = VK_NULL_HANDLE;
		physicalDevice = VK_NULL_HANDLE;
		graphicsQueueFamily = -1;
		presentQueueFamily = -1;
		initSurface = VK_NULL_HANDLE;
	}	

	void GraphicsContext_Vulkan::CreateWindowAndSurface(WindowSDL& window, VkSurfaceKHR& surface, StringView title, Vec2i pos, Vec2u size)
	{
		if (!initWindow.IsNullWindow())
		{
			window = std::move(initWindow);
			surface = initSurface;
			initSurface = VK_NULL_HANDLE;
		}
		else
		{
			window = vulkanInstance->CreateWindow(title, pos, size);
			if (SDL_Vulkan_CreateSurface((SDL_Window*)window.GetHandle(), vulkanInstance->GetHandle(), &surface) == SDL_FALSE)
				Debug::Logger::LogFatal("Blaze Engine", "SDL failed to create vulkan surface: \"" + (String)SDL_GetError() + "\"\n");
		}
	}

	void GraphicsContext_Vulkan::DestroyWindowAndSurface(WindowSDL& window, VkSurfaceKHR surface)
	{
		vulkanInstance->FreeWindow(window);
		vkDestroySurfaceKHR(vulkanInstance->GetHandle(), surface, nullptr);
	}

	void GraphicsContext_Vulkan::RunSingleTimeCommand(const std::function<void(VkCommandBuffer)>& func)
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

		func(commandBuffer);

		vkEndCommandBuffer(commandBuffer);

		VkSubmitInfo submitInfo{};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &commandBuffer;

		vkQueueSubmit(graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
		vkQueueWaitIdle(graphicsQueue);

		vkFreeCommandBuffers(device, commandPool, 1, &commandBuffer);
	}

	void GraphicsContext_Vulkan::TransitionImageLayout(VkImage image, VkImageLayout oldLayout, VkImageLayout newLayout, VkPipelineStageFlags srcStage, VkPipelineStageFlags dstStage, VkAccessFlags srcAccess, VkAccessFlags dstAccess)
	{
		RunSingleTimeCommand([&](VkCommandBuffer commandBuffer) {
			VkImageMemoryBarrier barrier{};
			barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
			barrier.oldLayout = oldLayout;
			barrier.newLayout = newLayout;
			barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
			barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
			barrier.image = image;
			barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			barrier.subresourceRange.baseMipLevel = 0;
			barrier.subresourceRange.levelCount = 1;
			barrier.subresourceRange.baseArrayLayer = 0;
			barrier.subresourceRange.layerCount = 1;
			barrier.srcAccessMask = srcAccess; // TODO
			barrier.dstAccessMask = dstAccess; // TODO

			vkCmdPipelineBarrier(
				commandBuffer,
				srcStage, dstStage,
				0,
				0, nullptr,
				0, nullptr,
				1, &barrier
			);
			});
	}
}