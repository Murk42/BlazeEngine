#include "BlazeEngine/Application/ECS/ComponentTypeRegistry.h"
#include "BlazeEngine/Logging/Logger.h"

namespace Blaze::ECS
{
	Result ComponentTypeRegistry::RegisterType(ComponentTypeData data)
	{
		if (nameTable.try_emplace(data.typeName, types.size()).second)
			types.emplace_back(std::move(data));

		return Result();
	}	
	ComponentTypeRegistry::ComponentTypeRegistry()
	{
	}
	ComponentTypeRegistry::ComponentTypeRegistry(const ComponentTypeRegistry& other)
		:types(other.types), nameTable(other.nameTable)
	{
	}
	ComponentTypeRegistry::ComponentTypeRegistry(ComponentTypeRegistry&& other) noexcept
		: types(std::move(other.types)), nameTable(std::move(other.nameTable))
	{
	}
	ComponentTypeRegistry::~ComponentTypeRegistry()
	{
	}
	int ComponentTypeRegistry::GetComponentTypeIndex(StringView name) const
	{
		auto it = nameTable.find((String)name);
		if (it == nameTable.end())
			return -1;
		else
			return it->second;
	}
	uint ComponentTypeRegistry::GetComponentTypeCount() const
	{
		return types.size();
	}
	ComponentTypeData ComponentTypeRegistry::GetComponentTypeData(uint index) const
	{
		if (index > types.size())
		{
			Logger::AddLog(LogType::Error, BLAZE_FILE_NAME, BLAZE_FUNCTION_NAME, BLAZE_FILE_LINE,
				"Blaze Engine", "Index is out of bounds. Index was: " + String::Convert(index));
			return { "" };
		}
		return types[index];
	}
	bool ComponentTypeRegistry::IsValidTypeIndex(uint index) const
	{
		return index < types.size();
	}	
	ComponentTypeRegistry& ComponentTypeRegistry::operator=(const ComponentTypeRegistry& r)
	{
		types = r.types;
		nameTable = r.nameTable;
		return *this;
	}
	ComponentTypeRegistry& ComponentTypeRegistry::operator=(ComponentTypeRegistry&& r) noexcept
	{
		types = std::move(r.types);
		nameTable = std::move(r.nameTable);
		return *this;
	}
}