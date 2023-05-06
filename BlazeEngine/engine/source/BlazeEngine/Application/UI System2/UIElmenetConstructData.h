#pragma once
#include "BlazeEngine/Application/UI System2/UIScene.h"

namespace Blaze::UI2
{	
	class UIElementTransform;

	struct UIElementConstructData
	{		
		UIElementTransform* transform = nullptr;
		UIScene* scene = nullptr;
		std::list<UIScene::Layer>::iterator layerIt;
		std::list<UIElement*>::iterator layerElementIt;
		String name;

		uint state = 0;
	};

	extern UIElementConstructData uiElementConstructData;
}