#include "Globals.h"

#include "CreateElementMenu.h"
#include "CreateSceneMenu.h"

namespace SceneListMenu
{
	UI::TextButton* newSceneButton;
	UI::TextButton* newElementButton;
	UI::Panel* panel;
	UI::List* list;
	
	std::vector<UI::UIScene*> scenes;
	int selected = -1;
	
	UI::UIScene* GetSelectedScene()
	{
		if (selected == -1)
			return nullptr;

		return scenes[selected];
	}

	void SelectScene(UI::UIScene* scene)
	{
		uint index = 0;
		for (; index < scenes.size() && scenes[index] != scene; ++index);

		if (selected != -1)
			((UI::TextButton*)list->GetElements()[selected].ptr)->SetNormalColor(0x00000000);		

		selected = index;

		if (index != -1)
			((UI::TextButton*)list->GetElements()[index].ptr)->SetNormalColor(0x444444ff);
	}
	void AddScene(UI::UIScene* scene)
	{
		scenes.emplace_back(scene);
		uint index = scenes.size() - 1;

		UI::TextButton* elementText = mainScene->CreateElement<UI::TextButton>(list->GetLayerName()).value;

		String name = scene->GetName();
		if (name.Size() == 0)
			name = "Unnamed";

		elementText->UIElement::SetProperties({ 
			.size = Vec2f(panel->GetSize().x - 10, 18) 
			});
		elementText->SetProperties(DefaultTextButtonProperties<14>(StringUTF8(name)));
		elementText->SetProperties({
				.normalColor = (ColorRGBAf)0x00000000, 
				.hoveredColor = elementText->GetPressedColor(),
				.textLocalAlign = Align::Left, .textAnchorAlign = Align::Left, .textOffset = Vec2f(5, 0),
			});
		elementText->AddPressedFunction([scene]() { SelectScene(scene);  });

		list->AddElement(elementText);
		SelectScene(scene);
	}

	static void CreateScenePressed()
	{		
		CreateSceneMenu::Open();
	}

	static void CreateElementPressed()
	{
		if (selected == -1)
		{
			return;
		}

		CreateElementMenu::Open();
	}

	void Setup()
	{
		const char* layer = "SceneListMenu_main";
		uiManager.CreateLayer(layer);

		CREATE_ELEMENT(newSceneButton, "main");
		CREATE_ELEMENT(newElementButton, "main");
		CREATE_ELEMENT(panel, layer);
		CREATE_ELEMENT(list, layer);

		newSceneButton->UIElement::SetProperties({ 
			.pos = Vec2f(15, -15), .size = Vec2f(110, 30), .localAlign = Align::TopLeft, .anchorAlign = Align::TopLeft 
			});
		newSceneButton->SetProperties(DefaultTextButtonProperties<18>("New scene"));
		newSceneButton->AddPressedFunction(CreateScenePressed);

		newElementButton->UIElement::SetProperties({ 
			.pos = Vec2f(5, 0), .size = Vec2f(130, 30), .anchor = newSceneButton, .localAlign = Align::Left, .anchorAlign = Align::Right 
			});
		newElementButton->SetProperties(DefaultTextButtonProperties<18>("New element"));
		newElementButton->AddPressedFunction(CreateElementPressed);
		
		panel->UIElement::SetProperties({
			.pos = Vec2f(15, -50), .size = Vec2f(300, 500), .localAlign = Align::TopLeft, .anchorAlign = Align::TopLeft
			});
		panel->SetProperties(menuPanelProperties);		

		list->UIElement::SetProperties({ .name = (String)"scenesList", .pos = Vec2f(5, -5), .anchor = panel, .localAlign = Align::TopLeft, .anchorAlign = Align::TopLeft });
		list->SetProperties( { .elementsOffset = 2 });
	}
}