#include "pch.h"
#include "BlazeEngine/Runtime/App/AppLayerCreationData.h"

namespace Blaze
{
	AppLayerCreationData::AppLayerCreationData()
		: constructFunction(nullptr)
	{
	}
	void AppLayerCreationData::Clear()
	{
		constructFunction = nullptr;
		arguments.Clear();
	}
	Handle<AppLayer> AppLayerCreationData::CreateLayer()
	{
		if (constructFunction != nullptr)
		{
			Handle<AppLayer> layer = constructFunction(*arguments);

			Clear();

			return layer;
		}
		return { };
	}
}
