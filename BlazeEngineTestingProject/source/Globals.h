#pragma once
#include <queue>

struct Globals {
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
};
extern Globals* globals;

void SetupUIManager(UI::UIManager* manager);
void SetupUIScene(UI::UIScene* scene, UI::UIManager* manager);

#define CREATE_ELEMENT(element, layer) element = globals->mainScene->CreateElement<std::remove_pointer_t<decltype(element)>>(layer).value;
#define CREATE_NAMED_ELEMENT(element, name, layer) element = globals->mainScene->CreateElement<std::remove_pointer_t<decltype(element)>>(name, layer).value;

template<size_t s>
constexpr FontResolution* GetFontResolution()
{
	if constexpr (s == 14)
		return globals->fontResolution14;
	else if constexpr (s == 18)
		return globals->fontResolution18;
	else if constexpr (s == 8)
		return globals->fontResolution8;
}

UI::PanelProperties GetMenuPanelProperties();
UI::PanelProperties GetButtonPanelProperties();
UI::PanelProperties GetBGPanelProperties();

template<size_t f>
UI::TextProperties DefaultTextProperties(StringUTF8 text = "")
{		
	return { .text = text, .fontResolution = GetFontResolution<f>(), .fontSize = f};
}
template<size_t f>
UI::TextButtonProperties DefaultTextButtonProperties(StringUTF8 text = "")
{
	return {
		.panelProperties = GetButtonPanelProperties(),
		.textProperties = DefaultTextProperties<f>(text),
		.normalColor = 0x555555ff,
		.hoveredColor = 0x444444ff,
		.pressedColor = 0x393939ff,
	};
}