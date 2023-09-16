#pragma once
#include "BlazeEngine/Application/ECS/Component.h"

namespace Blaze::UI::Components
{
	class UITransformComponent;

	class UISizeOverrideComponent : public ECS::Component
	{
	public:			
		virtual Vec2f ResolveSize(UITransformComponent*) = 0;		

		COMPONENT_ADD_TYPE_TAGS("UISizeOverrideComponent");
	};
}