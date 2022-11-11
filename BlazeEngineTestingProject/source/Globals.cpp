#include "Globals.h"

Globals* globals;

void SetupUIManager(UI::UIManager* manager)
{
	UI::UIElementTypeRegistry registry;
	registry.RegisterCoreTypes();
	

	manager->SetElementTypeRegistry(registry);
}
void SetupUIScene(UI::UIScene* scene, UI::UIManager* manager)
{
	scene->SetManager(manager);
	scene->SetResourceManager(&globals->resourceManager);
}

UI::PanelProperties GetMenuPanelProperties()
{
	return { .borderWidth = 0.5f, .cornerRadius = 6, .fillColor = (ColorRGBAf)0x333333ff, .borderColor = (ColorRGBAf)0x404040ff };
}

UI::PanelProperties GetButtonPanelProperties()
{
	return { .borderWidth = 0, .cornerRadius = 3, .fillColor = (ColorRGBAf)0x555555ff, };
}

UI::PanelProperties GetBGPanelProperties()
{
	return { .borderWidth = 0, .cornerRadius = 0 , .fillColor = (ColorRGBAf)0x202020cc, };
}
