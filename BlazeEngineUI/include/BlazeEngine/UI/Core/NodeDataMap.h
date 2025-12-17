#pragma once
#include "BlazeEngine/Core/Container/VirtualMap.h"
#include "BlazeEngine/Core/String/String.h"

namespace Blaze::UI
{
	class BLAZE_API NodeDataMap
	{
	public:
		VirtualMap<String> map;

		void SetName(StringView name);
		StringView GetName() const;

		void SetTypeName(StringView typeName);
		StringView GetTypeName() const;
	};
}