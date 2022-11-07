#pragma once
#include <queue>

extern ResourceSystem::ResourceManager resourceManager;
extern ResourceSystem::ResourceStorage resourceStorage;
extern UI::UIManager uiManager;
extern UI::UIScene* mainScene;

extern ResourceSystem::ResourceManager gameResourceManager;
extern ResourceSystem::ResourceStorage gameResourceStorage;
extern UI::UIManager gameUIManager;

extern FontResolution* fontResolution18;
extern FontResolution* fontResolution14;
extern FontResolution* fontResolution8;

void SetupUIManager(UI::UIManager* manager);
void SetupUIScene(UI::UIScene* scene, UI::UIManager* manager);

#define CREATE_ELEMENT(element, layer) element = mainScene->CreateElement<std::remove_pointer_t<decltype(element)>>(layer).value;
#define CREATE_NAMED_ELEMENT(element, name, layer) element = mainScene->CreateElement<std::remove_pointer_t<decltype(element)>>(name, layer).value;

template<size_t s>
constexpr FontResolution* GetFontResolution()
{
	if constexpr (s == 14)
		return fontResolution14;
	else if constexpr (s == 18)
		return fontResolution18;
	else if constexpr (s == 8)
		return fontResolution8;
}

const UI::PanelProperties menuPanelProperties = { .borderWidth = 0.5f, .cornerRadius = 6, .fillColor = (ColorRGBAf)0x333333ff, .borderColor = (ColorRGBAf)0x404040ff};
const UI::PanelProperties buttonPanelProperties = { .borderWidth = 0, .cornerRadius = 3, .fillColor = (ColorRGBAf)0x555555ff, };
const UI::PanelProperties bgPanelProperties = { .borderWidth = 0, .cornerRadius = 0 , .fillColor = (ColorRGBAf)0x202020cc, };

template<size_t f>
UI::TextProperties DefaultTextProperties(StringUTF8 text = "")
{		
	return { .text = text, .fontResolution = GetFontResolution<f>(), .fontSize = f};
}
template<size_t f>
UI::TextButtonProperties DefaultTextButtonProperties(StringUTF8 text = "")
{
	return {
		.panelProperties = buttonPanelProperties,
		.textProperties = DefaultTextProperties<f>(text),
		.normalColor = 0x555555ff,
		.hoveredColor = 0x444444ff,
		.pressedColor = 0x393939ff,
	};
}