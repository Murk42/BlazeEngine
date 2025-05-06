#include "pch.h"
#include "BlazeEngineGraphics/UI/Core/NodeDataMap.h"

namespace Blaze::UI
{
	void NodeDataMap::SetTypeName(const String& name) 
	{
		map.Insert<String, true>("type", name); 
	}
	String NodeDataMap::GetTypeName() const
	{
		auto it = map.Find("type");
		if (!it.IsNull())
			if (const String* ptr = it.GetValue<String>())
				return *ptr;
		return String();
	}
}