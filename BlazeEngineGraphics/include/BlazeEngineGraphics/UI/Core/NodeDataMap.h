#pragma once
#include "BlazeEngineCore/DataStructures/VirtualMap.h"
#include "BlazeEngineCore/DataStructures/String.h"
#include "BlazeEngineGraphics/BlazeEngineGraphicsDefines.h"

namespace Blaze::UI
{
	class BLAZE_GRAPHICS_API NodeDataMap
	{
	public:
		VirtualMap<String> map;

		void SetTypeName(const String& name);
		String GetTypeName() const;
	};
}