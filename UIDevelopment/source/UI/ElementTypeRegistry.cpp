#include "ElementTypeRegistry.h"
#include "Element.h"

namespace Blaze::UI2
{
	Result ElementTypeRegistry::RegisterType(StringView name, 
		size_t size, void(*construct)(void*), void(*destruct)(void*),
		size_t managerSize, void(*constructManager)(void*), void(*destructManager)(void*))
	{
		ElementTypeData data;
		data.name = name;
		data.size = size;
		data.construct = construct;
		data.destruct = destruct;
		data.managerSize = managerSize;
		data.constructManager = constructManager;
		data.destructManager = destructManager;

		if (nameTable.try_emplace(name, types.size()).second)
			types.emplace_back(std::move(data));

		return Result();
	}
	Result ElementTypeRegistry::OverrideManager(StringView name, size_t size, void(*construct)(void*), void(*destruct)(void*))
	{
		auto& data = types[nameTable[name]];
		data.managerSize = size;
		data.constructManager = construct;
		data.destructManager = destruct;
		return Result();
	}
	ElementTypeRegistry::ElementTypeRegistry()
	{
	}
	ElementTypeRegistry::ElementTypeRegistry(const ElementTypeRegistry& other)
		:types(other.types), nameTable(other.nameTable)
	{
	}
	ElementTypeRegistry::~ElementTypeRegistry()
	{
		nameTable.clear();
	}
	void ElementTypeRegistry::RegisterCoreTypes()
	{
		RegisterType<Element>();
		RegisterType<Panel>();
	}
	int ElementTypeRegistry::GetElementTypeIndex(StringView name) const
	{
		auto it = nameTable.find((String)name);
		if (it == nameTable.end())
			return -1;
		else
			return it->second;
	}
	uint ElementTypeRegistry::GetElementTypeCount() const
	{
		return types.size();
	}
	ElementTypeData ElementTypeRegistry::GetElementTypeData(uint index) const
	{
		if (index > types.size())
		{
			Logger::AddLog(LogType::Error, BLAZE_FILE_NAME, BLAZE_FUNCTION_NAME, BLAZE_FILE_LINE,
				"Blaze Engine", "Index is out of bounds. Index was: " + String::Convert(index));
			return { "" };
		}
		return types[index];
	}
	bool ElementTypeRegistry::IsValidTypeIndex(uint index) const
	{
		return index < types.size();
	}
	ElementTypeRegistry ElementTypeRegistry::CoreRegistry()
	{
		ElementTypeRegistry registry;
		registry.RegisterCoreTypes();
		return registry;
	}
}