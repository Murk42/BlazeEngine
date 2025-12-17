#include "pch.h"
#include "BlazeEngine/Runtime/App/AppRuntimeThreadCreationData.h"

namespace Blaze
{
	AppRuntimeThreadCreationData::AppRuntimeThreadCreationData()
		: threadFunction(nullptr)
	{
	}
	AppRuntimeThreadCreationData::AppRuntimeThreadCreationData(AppRuntimeThreadCreationData&& other) noexcept :
		threadFunction(std::exchange(other.threadFunction, nullptr)), runtimeThreadArguments(std::move(other.runtimeThreadArguments)), layerCreationData(std::move(other.layerCreationData))
	{
	}
	AppRuntimeThreadCreationData::~AppRuntimeThreadCreationData()
	{
		Clear();
	}
	void AppRuntimeThreadCreationData::Clear()
	{
		threadFunction = nullptr;
		runtimeThreadArguments.Clear();
		layerCreationData.Clear();
	}
	void AppRuntimeThreadCreationData::InstantiateOnThread(Thread& thread)
	{
		ThreadArguments arguments{ *this };

		arguments.threadInitialized.clear();

		thread.Run(threadFunction, &arguments);

		if (thread.IsRunning())
			arguments.threadInitialized.wait(false);

		Clear();
	}
	AppRuntimeThreadCreationData&& AppRuntimeThreadCreationData::RegisterLayer(AppLayerCreationData&& layerCreationData)
	{
		this->layerCreationData.AddBack(std::move(layerCreationData));
		return std::move(*this);
	}
	AppRuntimeThreadCreationData& AppRuntimeThreadCreationData::operator=(AppRuntimeThreadCreationData&& other) noexcept
	{
		Clear();

		threadFunction = std::exchange(other.threadFunction, nullptr);
		runtimeThreadArguments = std::move(other.runtimeThreadArguments);
		layerCreationData = std::move(other.layerCreationData);

		return *this;
	}
}