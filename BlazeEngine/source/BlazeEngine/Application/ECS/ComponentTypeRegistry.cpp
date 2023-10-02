#pragma once
#include "BlazeEngine/Application/ECS/ComponentTypeRegistry.h"

namespace Blaze::ECS
{
	//ComponentTypeData emptyComponentTypeData;
	 
	ComponentTypeData::ComponentTypeData(ComponentTypeRegistry* registry, StringView name, uint index,
		size_t size, ptrdiff_t baseOffset, Constructor constructoror, Destructor destructoror,
		size_t systemSize, ptrdiff_t systemBaseOffset, SystemConstructor systemConstructor, SystemDestructor systemDestructor,
		Set<StringView> typeTags)
		: registry(registry), typeName(name), index(index),
		size(size), baseOffset(baseOffset), constructor(constructoror), destructor(destructoror),
		systemSize(systemSize), systemBaseOffset(systemBaseOffset), systemConstructor(systemConstructor), systemDestructor(systemDestructor),
		typeTags(std::move(typeTags))
	{

	}

	ComponentTypeData::ComponentTypeData()		
		: registry(0), typeName(0), index(0),
		size(0), baseOffset(0), constructor(0), destructor(0),
		systemSize(0), systemBaseOffset(0), systemConstructor(0), systemDestructor(0), 
		typeTags()
	{
	}

	ComponentTypeData::ComponentTypeData(ComponentTypeData&& typeData) noexcept
		: registry(typeData.registry), typeName(typeData.typeName), index(typeData.index),
		size(typeData.size), baseOffset(typeData.baseOffset), constructor(typeData.constructor), destructor(typeData.destructor),
		systemSize(typeData.systemSize), systemBaseOffset(typeData.systemBaseOffset), systemConstructor(typeData.systemConstructor), systemDestructor(typeData.systemConstructor),
		typeTags(std::move(typeData.typeTags))
	{
		typeData.registry = nullptr;
		typeData.typeName = "";
		typeData.index = 0;
		typeData.size = 0;
		typeData.baseOffset = 0;
		typeData.constructor = nullptr;
		typeData.destructor = nullptr;
		typeData.systemSize = 0;
		typeData.systemBaseOffset = 0;
		typeData.systemConstructor = nullptr;
		typeData.systemDestructor = nullptr;		
	}

	ComponentTypeData& ComponentTypeData::operator=(ComponentTypeData&& o) noexcept
	{
		registry = o.registry;
		typeName = o.typeName;
		index = o.index;
		size = o.size;
		baseOffset = o.baseOffset;
		constructor = o.constructor;
		destructor = o.destructor;
		systemSize = o.systemSize;
		systemConstructor = o.systemConstructor;
		systemDestructor = o.systemDestructor;
		typeTags = std::move(o.typeTags);
		o.registry = nullptr;
		o.typeName = "";
		o.index = 0;
		o.size = 0;
		o.baseOffset = 0;
		o.constructor = nullptr;
		o.destructor = nullptr;
		o.systemSize = 0;
		o.systemConstructor = nullptr;
		o.systemDestructor = nullptr;		

		return *this;
	}

	bool ComponentTypeData::operator==(const ComponentTypeData& other) const
	{
		return this == &other || GetTypeName() == other.GetTypeName();
	}

	bool ComponentTypeData::operator!=(const ComponentTypeData& other) const
	{
		return this != &other && GetTypeName() != other.GetTypeName();
	}

	Result ComponentTypeRegistry::RegisterType(StringView name,
		size_t size, ptrdiff_t baseOffset, ComponentTypeData::Constructor constructor, ComponentTypeData::Destructor destructor,
		size_t systemSize, ptrdiff_t systemBaseOffset, ComponentTypeData::SystemConstructor systemConstructor, ComponentTypeData::SystemDestructor systemDestructor,
		Array<StringView> typeTags)
	{
		auto [it, inserted] = nameTable.Insert(StringView(name), 0);

		if (!inserted)
			return BLAZE_WARNING_RESULT("Blaze Engine", "Trying to register a type but there is a type with the same name already registered");		

		ComponentTypeData typeData = ComponentTypeData(this, name, typeCount, size, baseOffset, constructor, destructor, systemSize, systemBaseOffset, systemConstructor, systemDestructor, ArrayView<StringView>(typeTags));
		it->value = typeCount;

		auto* oldTypes = types;

		types = (ComponentTypeData*)Memory::Allocate((typeCount + 1) * sizeof(ComponentTypeData));

		for (uint i = 0; i < typeCount; ++i)
			new (types + i) ComponentTypeData(std::move(oldTypes[i]));

		new (types + typeCount) ComponentTypeData(std::move(typeData));
		typeCount++;

#pragma warning(suppress: 26800)
		Memory::Free(oldTypes);

		return Result();
	}	
	ComponentTypeRegistry::ComponentTypeRegistry()
		: types(nullptr), typeCount(0)
	{
	}
	ComponentTypeRegistry::ComponentTypeRegistry(const ComponentTypeRegistry& other)
		: typeCount(other.typeCount), nameTable(other.nameTable)
	{
		types = (ComponentTypeData*)Memory::Allocate(typeCount * sizeof(ComponentTypeData));			
		
		for (uint i = 0; i < typeCount; ++i)
		{
			auto& o = other.types[i];
			new (types + i) ComponentTypeData(
				this, o.typeName, i, 
				o.size, o.baseOffset, o.constructor, o.destructor, 
				o.systemSize, o.systemBaseOffset, o.systemConstructor, o.systemDestructor,
				o.typeTags
			);

		}
	}
	ComponentTypeRegistry::ComponentTypeRegistry(ComponentTypeRegistry&& other) noexcept		
		: types(other.types), typeCount(other.typeCount), nameTable(std::move(other.nameTable))
	{		
		for (uint i = 0; i < typeCount; ++i)
			types[i].registry = this;

		other.types = nullptr;
		other.typeCount = 0;		
	}
	ComponentTypeRegistry::~ComponentTypeRegistry()
	{
		Memory::Free(types);
	}
	bool ComponentTypeRegistry::GetComponentTypeData(StringView name, const ComponentTypeData*& typeData) const
	{
		auto it = nameTable.Find((String)name);

		if (it.IsNull())
		{
			typeData = nullptr;
			return false;
		}
		else
		{
			typeData = &types[it->value];
			return true;
		}
	}	

	bool ComponentTypeRegistry::HasComponentTypeData(StringView name) const
	{
		auto it = nameTable.Find((String)name);

		return !it.IsNull();			
	}

	ComponentTypeRegistry& ComponentTypeRegistry::operator=(const ComponentTypeRegistry& other)
	{
		Memory::Free(types);
		typeCount = other.typeCount;
		nameTable = other.nameTable;		

		types = (ComponentTypeData*)Memory::Allocate(typeCount * sizeof(ComponentTypeData));

		for (uint i = 0; i < typeCount; ++i)
		{
			auto& o = other.types[i];
			new (types + i) ComponentTypeData(
				this, o.typeName, i,
				o.size, o.baseOffset, o.constructor, o.destructor,
				o.systemSize, o.systemBaseOffset, o.systemConstructor, o.systemDestructor,
				o.typeTags
			);			
		}

		return *this;
	}
	ComponentTypeRegistry& ComponentTypeRegistry::operator=(ComponentTypeRegistry&& r) noexcept
	{
		Memory::Free(types);

		types = r.types;
		typeCount = r.typeCount;
		nameTable = std::move(r.nameTable);

		for (uint i = 0; i < typeCount; ++i)		
			types[i].registry = this;		

		r.types = nullptr;
		r.typeCount = 0;		 
		return *this;
	}	

	ArrayView<ComponentTypeData> ComponentTypeRegistry::GetAllTypesData() const
	{
		return { types, typeCount };
	}
}