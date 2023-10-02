#pragma once
#include "BlazeEngine/Application/UI/UIScene.h"

namespace Blaze::UI
{	
	namespace Components
	{
		class UITransformComponent;
		class UIElementComponent;
	}
	
	struct UIElementCreationData
	{				
		UIScene* scene = nullptr;
		DualList<UIScene::Layer>::Iterator layerIt;
		DualList<UIScene::Layer::Element>::Iterator layerElementIt;
		String name;		

		Components::UIElementComponent** nameToElementIt;
	};
	
	extern List<UIElementCreationData> uiElementCreationData;
	extern UIElementCreationData* currentUIElementCreationData;
}