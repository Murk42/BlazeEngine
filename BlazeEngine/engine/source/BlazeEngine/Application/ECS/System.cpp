#include "BlazeEngine/Application/ECS/System.h"

#include "SystemConstructData.h"

namespace Blaze::ECS
{	
	SystemConstructData systemConstructData;

	System::System() :
		scene(systemConstructData.scene),
		typeData(*systemConstructData.typeData)		
	{
		if (systemConstructData.state == 1)
		{
			systemConstructData.state = 2;
		}
		else if (systemConstructData.state == 0)
		{			
			Logger::ProcessLog(BLAZE_WARNING_LOG("Blaze Engine",
				"Creating a system outside of a manager. It may not function properly. Or there was an internal engine error."));
		}
		else
		{
			Logger::ProcessLog(BLAZE_WARNING_LOG("Blaze Engine",
				"Internal engine error. systemConstructData.state was changed inappropriately"));
		}
	}
	void System::Created(Component*)
	{
	}
	void System::Destroyed(Component*)
	{
	}
	void System::Update(const ComponentContainer&)
	{
	}
	void System::Update(Component* component)
	{
	}	
}
