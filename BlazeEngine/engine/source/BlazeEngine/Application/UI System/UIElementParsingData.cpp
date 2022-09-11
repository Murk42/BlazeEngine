#include "BlazeEngine/Application/UI System/UIElementParsingData.h"

namespace Blaze::UI
{
	Result UIElementParsingData::AddMemberRaw(StringView name, UIElementMemberTypeData typeData, 
		std::function<void(UIScene*, void*, const void*)> setter, 
		std::function<void(UIScene*, const void*, void*)> getter)
	{
		UIElementMemberData data;
		data.name = name;
		data.typeData = typeData;
		data.setter = setter;
		data.getter = getter;		

		size_t index = members.size();

		if (nameMap.insert({ name, index }).second)
		{
			members.emplace_back(data);
			return Result();
		}
		else
			return Result(Log(LogType::Warning, BLAZE_FILE_NAME, BLAZE_FUNCTION_NAME, BLAZE_FILE_LINE,
				"Blaze Engine", "Adding a member with the same name twice. A member with the same name was already added"));
	}
	UIElementParsingData::UIElementParsingData()
	{ 
	}
	UIElementParsingData::UIElementParsingData(const UIElementParsingData& o)
		: members(o.members), nameMap(o.nameMap)
	{
	}
	UIElementParsingData::UIElementParsingData(UIElementParsingData&& o) noexcept
		: members(std::move(o.members)), nameMap(std::move(o.nameMap))
	{
	}
	UIElementParsingData::~UIElementParsingData()
	{
		members.clear();
		nameMap.clear();
	}

	int UIElementParsingData::GetMemberIndex(StringView name) const
	{
		auto it = nameMap.find(name);		
		if (it == nameMap.end())
			return -1;
		else 		
			return it->second;		
	}
	UIElementMemberData UIElementParsingData::GetMember(uint index) const
	{
		return members[index];
	}
	uint UIElementParsingData::GetMemberCount() const
	{
		return members.size();
	}
	UIElementParsingData& UIElementParsingData::operator=(const UIElementParsingData& o)
	{
		members = o.members;
		nameMap = o.nameMap;
		return *this;
	}
	UIElementParsingData& UIElementParsingData::operator=(UIElementParsingData&& o) noexcept
	{
		members = std::move(o.members);
		nameMap = std::move(o.nameMap);
		return *this;
	}
}