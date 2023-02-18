#include "NameReferenceComponent.h"

namespace Blaze::ECS
{
	void NameReferenceComponent::SetName(String name)
	{
		auto* system = GetEntity()->GetScene()->GetManager()->GetSystem(GetTypeIndex())->Cast<NameReferenceComponent>();
		auto it = system->map.find(name);

		if (it != system->map.end())
			system->map.erase(it);
		
		system->map.insert({ name, this });
	}
	NameReferenceComponent* NameReferenceComponentSystem::GetComponent(StringView name) const
	{
		auto it = map.find(name);
		if (it != map.end())
			return it->second;
		else
			return nullptr;
	}
	bool NameReferenceComponentSystem::SetProperty(Component* component, StringView name, StringView value)
	{
		auto* nrc = component->Cast<NameReferenceComponent>();
		if (name == "name")
		{
			String t;
			StringParsing::RemoveQuotes(value, t);
			nrc->SetName(t);
		}
		else
			return false;

		return true;
	}
}