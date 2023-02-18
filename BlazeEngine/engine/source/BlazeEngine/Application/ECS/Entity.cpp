#include "BlazeEngine/Application/ECS/Entity.h"
#include "BlazeEngine/Application/ECS/Scene.h"

namespace Blaze::ECS
{
	const ComponentTypeRegistry& Entity::GetRegistry() const
	{
		return scene->GetManager()->GetRegistry();
	}
	bool Entity::HasComponent(uint typeIndex) const
	{
		return components.HasComponent(typeIndex);
	}
	Component* Entity::GetComponent(uint typeIndex) const
	{
		return components.GetComponent(typeIndex);
	}
}