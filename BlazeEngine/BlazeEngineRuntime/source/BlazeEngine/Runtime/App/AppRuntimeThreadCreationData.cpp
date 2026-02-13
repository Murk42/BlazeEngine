#include "pch.h"
#include "BlazeEngine/Runtime/App/AppRuntimeThreadCreationData.h"

namespace Blaze
{
	AppRuntimeThreadCreationData::AppRuntimeThreadCreationData()
		: threadFunction(nullptr)
	{
	}
	void AppRuntimeThreadCreationData::Clear()
	{
		threadFunction = nullptr;
		runtimeThreadArguments.Clear();
		layerCreationData.Clear();
	}
	bool AppRuntimeThreadCreationData::InstantiateOnThread(Thread& thread)
	{
		ThreadArguments arguments{ *this };

		arguments.threadInitialized.clear();

		if (!thread.Run(threadFunction, &arguments))
			return false;

		arguments.threadInitialized.wait(false);

		Clear();

		return true;
	}
	AppRuntimeThreadCreationData&& AppRuntimeThreadCreationData::RegisterLayer(AppLayerCreationData&& layerCreationData)
	{
		this->layerCreationData.AddBack(std::move(layerCreationData));
		return std::move(*this);
	}
}