#include "BlazeEngine/Application/ECS/Manager.h"
#include "BlazeEngine/Memory/MemoryManager.h"

namespace Blaze::ECS
{
	Manager::~Manager()
	{
		for (int i = 0; i < systems.Count(); ++i)
		{
			auto typeData = registry.GetComponentTypeData(i);
			typeData.destructSystem(systems[i]);
			Memory::Free(systems[i]);
		}
	}
	Result Manager::SetRegistry(const ComponentTypeRegistry& registry)
	{
		if (this->registry.GetComponentTypeCount() != 0)
			return BLAZE_ERROR_RESULT("BlazeEngine", "Cant change the element registry");

		this->registry = registry;

		systems.Resize(registry.GetComponentTypeCount());
		for (int i = 0; i < systems.Count(); ++i)
		{
			auto typeData = registry.GetComponentTypeData(i);
			void* ptr = Memory::Allocate(typeData.systemSize);
			typeData.constructSystem(ptr);
			systems[i] = (System*)ptr;
			systems[i]->manager = this;
			systems[i]->typeIndex = i;
		}

		return Result();
	}
	System* Manager::GetSystem(uint typeIndex) const
	{
		if (!registry.IsValidTypeIndex(typeIndex))
		{
			Logger::AddLog(BLAZE_ERROR_LOG("Blaze Engine", "Invalid type index"));
			return nullptr;
		}
		return systems[typeIndex];
	}


}