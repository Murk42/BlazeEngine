#include "pch.h"
#include "BlazeEngine/Application/ECS/ComponentTypeRegistry.h"
#include "BlazeEngineCore/Debug/Logger.h"

namespace Blaze::ECS
{
	//ComponentTypeData emptyComponentTypeData;
	 
	ComponentTypeData::ComponentTypeData(ComponentTypeRegistry* registry, StringView name, uintMem index,
		uintMem size, ptrdiff_t baseOffset, Constructor constructoror, Destructor destructoror,
		uintMem systemSize, ptrdiff_t systemBaseOffset, SystemConstructor systemConstructor, SystemDestructor systemDestructor,
		Set<StringView> typeTags)
		: registry(registry), typeName(name), index(index),
		size(size), baseOffset(baseOffset), constructor(constructoror), destructor(destructoror),
		systemSize(systemSize), systemBaseOffset(systemBaseOffset), systemConstructor(systemConstructor), systemDestructor(systemDestructor),
		typeTags(std::move(typeTags))
	{

	}

	//ComponentTypeData::ComponentTypeData()		
	//	: registry(0), typeName(0), index(0),
	//	size(0), baseOffset(0), constructor(0), destructor(0),
	//	systemSize(0), systemBaseOffset(0), systemConstructor(0), systemDestructor(0), 
	//	typeTags()
	//{
	//}
	//
	//ComponentTypeData::ComponentTypeData(ComponentTypeData&& typeData) noexcept
	//	: registry(typeData.registry), typeName(typeData.typeName), index(typeData.index),
	//	size(typeData.size), baseOffset(typeData.baseOffset), constructor(typeData.constructor), destructor(typeData.destructor),
	//	systemSize(typeData.systemSize), systemBaseOffset(typeData.systemBaseOffset), systemConstructor(typeData.systemConstructor), systemDestructor(typeData.systemConstructor),
	//	typeTags(std::move(typeData.typeTags))
	//{
	//	typeData.registry = nullptr;
	//	typeData.typeName = "";
	//	typeData.index = 0;
	//	typeData.size = 0;
	//	typeData.baseOffset = 0;
	//	typeData.constructor = nullptr;
	//	typeData.destructor = nullptr;
	//	typeData.systemSize = 0;
	//	typeData.systemBaseOffset = 0;
	//	typeData.systemConstructor = nullptr;
	//	typeData.systemDestructor = nullptr;		
	//}
	//
	//ComponentTypeData& ComponentTypeData::operator=(ComponentTypeData&& o) noexcept
	//{
	//	registry = o.registry;
	//	typeName = o.typeName;
	//	index = o.index;
	//	size = o.size;
	//	baseOffset = o.baseOffset;
	//	constructor = o.constructor;
	//	destructor = o.destructor;
	//	systemSize = o.systemSize;
	//	systemConstructor = o.systemConstructor;
	//	systemDestructor = o.systemDestructor;
	//	typeTags = std::move(o.typeTags);
	//	o.registry = nullptr;
	//	o.typeName = "";
	//	o.index = 0;
	//	o.size = 0;
	//	o.baseOffset = 0;
	//	o.constructor = nullptr;
	//	o.destructor = nullptr;
	//	o.systemSize = 0;
	//	o.systemConstructor = nullptr;
	//	o.systemDestructor = nullptr;		
	//
	//	return *this;
	//}

	bool ComponentTypeData::operator==(const ComponentTypeData& other) const
	{
		return this == &other || GetTypeName() == other.GetTypeName();
	}

	bool ComponentTypeData::operator!=(const ComponentTypeData& other) const
	{
		return this != &other && GetTypeName() != other.GetTypeName();
	}

	void ComponentTypeRegistry::ReserveTypeCount(uint count)
	{						
		types.ReserveExactly(count);				
	}

	void ComponentTypeRegistry::AddType(StringView name,
		uintMem size, ptrdiff_t baseOffset, ComponentTypeData::Constructor constructor, ComponentTypeData::Destructor destructor,
		uintMem systemSize, ptrdiff_t systemBaseOffset, ComponentTypeData::SystemConstructor systemConstructor, ComponentTypeData::SystemDestructor systemDestructor,
		Array<StringView> typeTags)
	{
		auto [it, inserted] = nameTable.Insert(StringView(name), nullptr);

		if (!inserted)
		{
			BLAZE_LOG_WARNING("Trying to register a type but there is a type with the same name already registered");
			return;
		}

		types.TryAddBack(this, name, types.Count(), size, baseOffset, constructor, destructor, systemSize, systemBaseOffset, systemConstructor, systemDestructor, ArrayView<StringView>(typeTags));		

		it->value = &types.Last();				
	}	
	ComponentTypeRegistry::ComponentTypeRegistry()		
	{
	}
	ComponentTypeRegistry::ComponentTypeRegistry(const ComponentTypeRegistry& other)
		: nameTable(other.nameTable), types(other.types.Count(), [&](ComponentTypeData* it, uint index) {
		auto& o = other.types[index];
		std::construct_at(it, this, o.typeName, index,
			o.size, o.baseOffset, o.constructor, o.destructor,
			o.systemSize, o.systemBaseOffset, o.systemConstructor, o.systemDestructor,
			o.typeTags);
			})
	{					
	}
	ComponentTypeRegistry::ComponentTypeRegistry(ComponentTypeRegistry&& other) noexcept		
		: types(std::move(other.types)), nameTable(std::move(other.nameTable))
	{						
		for (auto& type : types)
			type.registry = this;
	}
	ComponentTypeRegistry::~ComponentTypeRegistry()
	{		
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
			typeData = it->value;
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
		types.Clear();

		types = std::move(Array<ComponentTypeData>(other.types.Count(), [&](ComponentTypeData* it, uintMem index) {
				auto& o = other.types[index];
				std::construct_at(it, this, o.typeName, index,
					o.size, o.baseOffset, o.constructor, o.destructor,
					o.systemSize, o.systemBaseOffset, o.systemConstructor, o.systemDestructor,
					o.typeTags);
			}));		
		
		for (auto& name : other.nameTable)
			nameTable.Insert(name.key, types.Ptr() + (name.value - other.types.Ptr()));

		return *this;
	}
	ComponentTypeRegistry& ComponentTypeRegistry::operator=(ComponentTypeRegistry&& r) noexcept
	{
		types = std::move(r.types);
		nameTable = std::move(r.nameTable);

		for (auto& type : types)		
			type.registry = this;		
		
		return *this;
	}	

	ArrayView<ComponentTypeData> ComponentTypeRegistry::GetAllTypesData() const
	{
		return types;
	}
}