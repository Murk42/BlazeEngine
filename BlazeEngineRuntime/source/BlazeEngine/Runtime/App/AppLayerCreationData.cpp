#include "pch.h"
#include "BlazeEngine/Runtime/App/AppLayerCreationData.h"

namespace Blaze
{
	AppLayerCreationData::AppLayerCreationData()
		: constructFunction(nullptr)
	{
	}
	bool AppLayerCreationData::CreateLayer(Array<Handle<AppLayer>>& layerStack) const
	{
		if (constructFunction != nullptr && !arguments.Empty())
		{
			constructFunction(layerStack, *arguments);
			return true;
		}
		else
			return false;
	}
}
