#pragma once
#include "BlazeEngine/Application/ECS/Component.h"

namespace Blaze::UI
{		
	class UIScene;

	namespace Components
	{
		class UIElementComponent;
		class UITransformComponent;
	}


	class BLAZE_API UIComponent : public ECS::Component
	{
		Components::UIElementComponent* GetElementComponent();
		Components::UITransformComponent* GetTransformComponent();
	public:
		Components::UIElementComponent* const element;
		Components::UITransformComponent* const transform;

		UIComponent();

		UIScene* GetUIScene();
		StringView GetName();
		StringView GetLayer();

		COMPONENT_ADD_TYPE_TAGS("UIComponent");

		friend class UIScene;
	};
}