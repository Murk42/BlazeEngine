#pragma once
#include "BlazeEngine/Application/ECS/Component.h"

namespace Blaze::UI::Components
{
	class UITransformComponent;

	class UIPosOverrideComponent : public ECS::Component
	{
	public:
		virtual Vec2f ResolvePos(UITransformComponent*) = 0;

		COMPONENT_ADD_TYPE_TAGS("UIPosOverrideComponent");
	};
}