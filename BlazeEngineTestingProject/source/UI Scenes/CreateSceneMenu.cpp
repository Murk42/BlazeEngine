#include "Globals.h"

#include "SceneListMenu.h"

namespace CreateSceneMenu
{
	UI::Panel* bgPanel;
	UI::Panel* menuPanel;
	UI::TextButton* cancelButton;
	UI::TextButton* createButton;
	UI::Text* menuLabel;
	UI::Text* sceneNameLabel;
	UI::TextField* sceneName;

	static void CancelButton()
	{
		bgPanel->SetActiveFlag(false);
	}

	static void CreateButton()
	{
		String name = String((const char*)sceneName->GetText().Buffer());

		UI::UIScene* scene;
		if (name.Size() > 0)
			scene = gameResourceStorage.CreateResource<UI::UIScene>(name);
		else
			scene = gameResourceStorage.CreateResource<UI::UIScene>();
		SetupUIScene(scene, &gameUIManager);
		
		SceneListMenu::AddScene(scene);
		SceneListMenu::SelectScene(scene);

		CancelButton();
	}	

	void Open()
	{
		bgPanel->SetActiveFlag(true);
	}
	void Setup()
	{
		const char* layer = "createSceneMenu";
		uiManager.CreateLayer(layer);		

		CREATE_ELEMENT(bgPanel			, layer);
		CREATE_ELEMENT(menuPanel		, layer);
		CREATE_ELEMENT(menuLabel		, layer);
		CREATE_ELEMENT(cancelButton		, layer);
		CREATE_ELEMENT(createButton		, layer);
		CREATE_ELEMENT(sceneNameLabel	, layer);
		CREATE_ELEMENT(sceneName		, layer);

		bgPanel->UIElement::SetProperties({
			.name = (String)"createSceneMenu_panel", .localAlign = Align::Center, .anchorAlign = Align::Center, .active = false 
			});
		bgPanel->SetProperties(bgPanelProperties);
		uiManager.AddViewportChangedFunction([&]() { bgPanel->SetSize((Vec2f)uiManager.GetViewport().size); });

		menuPanel->UIElement::SetProperties({
			.size = Vec2f(300, 100), .anchor = bgPanel
			});
		menuPanel->SetProperties({menuPanelProperties});		

		cancelButton->UIElement::SetProperties({
			.pos = Vec2f(-5, 5), .size = Vec2f(70, 20), .anchor = menuPanel, .localAlign = Align::BottomRight, .anchorAlign = Align::BottomRight
			});
		cancelButton->SetProperties(DefaultTextButtonProperties<14>("Cancel"));		
		cancelButton->AddPressedFunction(CancelButton);
		
		createButton->UIElement::SetProperties({
			.pos = Vec2f(-5, 0), .size = Vec2f(70, 20), .anchor = cancelButton, .localAlign = Align::Right, .anchorAlign = Align::Left
			});		
		createButton->SetProperties(DefaultTextButtonProperties<14>("Create"));		
		createButton->AddPressedFunction(CreateButton);

		menuLabel->UIElement::SetProperties({
			.pos = Vec2f(0, -10), .anchor = menuPanel, .localAlign = Align::Top, .anchorAlign = Align::Top
			});
		menuLabel->SetProperties(DefaultTextProperties<18>("New scene"));		

		sceneNameLabel->UIElement::SetProperties({
			.pos = Vec2f(5, -40), .anchor = menuPanel, .localAlign = Align::TopLeft, .anchorAlign = Align::TopLeft
			});
		sceneNameLabel->SetProperties(DefaultTextProperties<14>("Name:"));		
	
		sceneName->UIElement::SetProperties({
			.pos = Vec2f(5, 0), .size = Vec2f(200, 20), .anchor = sceneNameLabel, .localAlign = Align::Left, .anchorAlign = Align::Right
			});
		sceneName->SetProperties({
			.buttonProperties = DefaultTextButtonProperties<14>(),
			.emptyText = (StringUTF8)"Enter scene name"
			});
	}
}