#include "pch.h"
#include "BlazeEngine/UI/Core/NodeDataMap.h"

namespace Blaze::UI
{
	void NodeDataMap::SetName(StringView name)
	{
		map.Insert<String, true>("name", name);
	}
	StringView NodeDataMap::GetName() const
	{
		auto it = map.Find("name");

		if (!it.IsNull())
			if (const String* ptr = it.GetValue<String>())
				return *ptr;

		return StringView();
	}
	void NodeDataMap::SetTypeName(StringView name)
	{
		map.Insert<String, true>("type", name);
	}
	StringView NodeDataMap::GetTypeName() const
	{
		auto it = map.Find("type");

		if (!it.IsNull())
			if (const String* ptr = it.GetValue<String>())
				return *ptr;

		return StringView();
	}
}