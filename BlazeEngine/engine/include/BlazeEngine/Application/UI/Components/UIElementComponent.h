#pragma once
#include "BlazeEngine/Application/ECS/Component.h"
#include "BlazeEngine/Application/UI/UIScene.h"

namespace Blaze::UI
{
	class UIComponent;
}
namespace Blaze::UI::Components
{	

	class BLAZE_API UIElementComponent : public ECS::Component
	{
		DualList<UI::UIScene::Layer>::Iterator layerIt;
		DualList<UI::UIScene::Layer::Element>::Iterator layerElementIt;

		UIScene* scene;
		String name;
		
		Array<UIComponent*> uiComponents;		
	public:
		UIElementComponent();
		UIElementComponent(const UIElementComponent&);
		UIElementComponent(UIElementComponent&&) noexcept;

		inline UIScene* GetUIScene() const { return scene; }
		inline StringView GetName() const { return name; }		
		inline StringView GetLayer() const { return layerIt->name; }
		
		COMPONENT(UIElementComponent, ECS::System);		

		friend class UIScene;
	};
}