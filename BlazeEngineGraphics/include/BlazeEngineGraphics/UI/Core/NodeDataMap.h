#pragma once
#include "BlazeEngineGraphics/BlazeEngineGraphicsDefines.h"
#include "BlazeEngineCore/BlazeEngineCore.h"

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