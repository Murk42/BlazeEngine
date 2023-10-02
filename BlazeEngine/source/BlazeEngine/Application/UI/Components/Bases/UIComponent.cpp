#include "pch.h"
#include "BlazeEngine/Application/UI/UIComponent.h"
#include "BlazeEngine/Application/UI/Components/UIElementComponent.h"
#include "BlazeEngine/Application/UI/Components/UITransformComponent.h"
#include "BlazeEngine/Application/ECS/Entity.h"

namespace Blaze::UI
{
	Components::UIElementComponent* UIComponent::GetElementComponent()
	{
		if (GetEntity() == nullptr)
			return nullptr;
		
		auto element = GetEntity()->GetComponent<Components::UIElementComponent>();

		if (element == nullptr)
			return nullptr;

		return element;
	}
	Components::UITransformComponent* UIComponent::GetTransformComponent()
	{
		if (GetEntity() == nullptr)
			return nullptr;

		auto transform = GetEntity()->GetComponent<Components::UITransformComponent>();

		if (transform == nullptr)
			return nullptr;

		return transform;
	}
	UIComponent::UIComponent()
		: transform(GetTransformComponent()), element(GetElementComponent())
	{
	}
	UIScene* UIComponent::GetUIScene() 
	{ 
		return element->GetUIScene(); 
	}
	StringView UIComponent::GetName() 
	{ 
		return element->GetName(); 
	}
	StringView UIComponent::GetLayer() 
	{ 
		return element->GetLayer(); 
	}
}