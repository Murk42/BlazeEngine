#include "pch.h"
#include "BlazeEngine/Core/Resource/ResourceManager.h"

namespace Blaze
{
	ResourceManager::ResourceManager()
	{
		threadPool.AllocateThreads(1);
	}
	void ResourceManager::HandleResourceLoadedCallbacks()
	{
		Array<ResourceLoadedCallbackData> resourceLoadedCallbacks;

		{
			std::lock_guard lg{ mutex };
			resourceLoadedCallbacks = std::move(this->resourceLoadedCallbacks);
		}

		for (auto& callback : resourceLoadedCallbacks)
			callback.function(callback.resource, callback.resourceLoadedCallback, callback.resourceLoadedCallbackUserData);
	}
}
