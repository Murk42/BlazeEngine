#include "BlazeEngine/Application/ECS/System.h"
#include "BlazeEngine/Application/ECS/Manager.h"

namespace Blaze::ECS
{
	const ComponentTypeRegistry& System::GetRegistry() const
	{
		return manager->GetRegistry();
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
	void System::Render(const ComponentContainer&)
	{
	}
	bool System::SetProperty(Component*, StringView name, StringView value)
	{
		return false;
	}
}
