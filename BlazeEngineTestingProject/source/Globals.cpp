#include "Globals.h"

ResourceSystem::ResourceManager resourceManager;
ResourceSystem::ResourceStorage resourceStorage;
UI::UIManager uiManager;
UI::UIScene* mainScene;

ResourceSystem::ResourceManager gameResourceManager;
ResourceSystem::ResourceStorage gameResourceStorage;
UI::UIManager gameUIManager;

FontResolution* fontResolution18;
FontResolution* fontResolution14;
FontResolution* fontResolution8;

void SetupUIManager(UI::UIManager* manager)
{
	UI::UIElementTypeRegistry registry;
	registry.RegisterCoreTypes();
	

	manager->SetElementTypeRegistry(registry);
}
void SetupUIScene(UI::UIScene* scene, UI::UIManager* manager)
{
	scene->SetManager(manager);
	scene->SetResourceManager(&resourceManager);
}