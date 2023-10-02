#include "pch.h"
#include "BlazeEngine/Application/ECS/System.h"

#include "SystemCreationData.h"

namespace Blaze::ECS
{	
	SystemCreationData systemCreationData;

	System::System() :
		scene(systemCreationData.scene),
		typeData(systemCreationData.typeData)
	{		
	}
	void System::Created(Component*)
	{
	}
	void System::Destroyed(Component*)
	{
	}		
	bool System::GetTypeData(const ComponentTypeData*& typeData) const
	{
		if (this->typeData == nullptr)
		{
			typeData = nullptr;
			return false;
		}
		else
		{
			typeData = this->typeData;
			return true;
		}
	}

}
