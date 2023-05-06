#pragma once
#include "BlazeEngine/Application/ECS/Component.h"
#include "BlazeEngine/Application/UI System2/UIScene.h"
#include "BlazeEngine/Application/UI System2/UIElementTransform.h"

namespace Blaze::UI2
{
	class UIElementTransform;

	class UIElementSystem;

#define UIELEMENT(name, system) COMPONENT(name, system)

	class BLAZE_API UIElement : public ECS::Component
	{
		std::list<UIScene::Layer>::iterator layerIt;
		std::list<UIElement*>::iterator layerElementIt;

		UIScene* scene;
		String name;
	public:
		UIElementTransform* const transform;

		UIElement();

		inline UIScene* GetScene() const { return scene; }
		inline StringView GetName() const { return name; }

		COMPONENT(UIElement, UIElementSystem);

		friend class UIScene;
	};

	class BLAZE_API UIElementSystem : public ECS::System
	{				
	public:
		void Update(ECS::Component*) override;		
	};
}