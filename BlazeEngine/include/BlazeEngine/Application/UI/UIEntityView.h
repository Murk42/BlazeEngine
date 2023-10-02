#pragma once
#include "BlazeEngine/Application/ECS/EntityView.h"

namespace Blaze::UI
{
	namespace Components
	{
		class UIElementComponent;
		class UITransformComponent;
	}

	template<typename ... Cs> requires (ECS::IsComponent<Cs> && ...)
	class UIEntityView : public ECS::EntityView<Components::UITransformComponent, Components::UIElementComponent, Cs...>
	{
		using Base = ECS::EntityView<Components::UITransformComponent, Components::UIElementComponent, Cs...>;
	public:		
		Components::UITransformComponent* transform;
		Components::UIElementComponent* element;

		UIEntityView()
			: Base()
		{
		}
		UIEntityView(const UIEntityView& other)
			: Base(other)
		{
			transform = (Components::UITransformComponent*)Base::GetEntity()->GetComponent(0);
			element = (Components::UIElementComponent*)Base::GetEntity()->GetComponent(1);
		}
		UIEntityView(ECS::Entity* entity)
			: Base(entity)
		{
			transform = (Components::UITransformComponent*)Base::GetEntity()->GetComponent(0);
			element = (Components::UIElementComponent*)Base::GetEntity()->GetComponent(1);
		}

		UIEntityView& operator=(const UIEntityView& other)
		{
			Base::operator=(other);
			transform = (Components::UITransformComponent*)Base::GetEntity()->GetComponent(0);
			element = (Components::UIElementComponent*)Base::GetEntity()->GetComponent(1);
		}

		template<uint I> requires (I < sizeof...(Cs))
		inline auto GetComponent()
		{
			constexpr uint _I = I + 2;
			return Base::template GetComponent<_I>();
		}
	};
}