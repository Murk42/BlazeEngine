#include "pch.h"
#include "BlazeEngine/Core/Resource/ResourceManager.h"

namespace Blaze
{
	ResourceManager::ResourceManager()
	{
	}
	void ResourceManager::HandleResourceLoadedCallbacks()
	{
		Array<ResourceLoadedCallbackData> oldResourceLoadedCallbacks;

		{
			std::lock_guard lg{ mutex };
			oldResourceLoadedCallbacks = std::move(resourceLoadedCallbacks);
		}

		for (auto& callback : oldResourceLoadedCallbacks)
			callback.function(callback.resource, callback.resourceLoadedCallback, callback.resourceLoadedCallbackUserData);
	}
}
