#pragma once
#include "BlazeEngine/BlazeEngine.h"
using namespace Blaze;

namespace SceneListMenu
{
	void AddScene(UI::UIScene* scene);
	UI::UIScene* GetSelectedScene();
	void SelectScene(UI::UIScene* selected);

	void Setup();	
}