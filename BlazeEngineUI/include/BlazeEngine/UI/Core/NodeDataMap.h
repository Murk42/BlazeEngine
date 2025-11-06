#pragma once
#include "BlazeEngine/Core/Container/VirtualMap.h"
#include "BlazeEngine/Core/String/String.h"

namespace Blaze::UI
{
	class BLAZE_API NodeDataMap
	{
	public:
		VirtualMap<String> map;

		void SetTypeName(const String& name);
		String GetTypeName() const;
	};
}