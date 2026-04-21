#include "pch.h"
#include "BlazeEngine/Runtime/App/AppRuntimeThreadCreationData.h"

namespace Blaze
{
	AppRuntimeThreadCreationData::AppRuntimeThreadCreationData()
		: createObjectFunction(nullptr)
	{
	}
	void AppRuntimeThreadCreationData::Clear()
	{
		createObjectFunction = nullptr;
		arguments.Clear();
		layerCreationData.Clear();
	}
	AppRuntimeThreadCreationData&& AppRuntimeThreadCreationData::RegisterLayer(AppLayerCreationData&& layerCreationData)
	{
		this->layerCreationData.AddBack(std::move(layerCreationData));
		return std::move(*this);
	}
}