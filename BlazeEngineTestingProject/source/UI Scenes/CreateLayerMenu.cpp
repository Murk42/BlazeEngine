#include "Globals.h"

#include "SceneListMenu.h"

namespace CreateLayerMenu
{
	UI::Panel* bgPanel;
	UI::Panel* menuPanel;
	UI::TextButton* cancelButton;
	UI::TextButton* createButton;
	UI::Text* menuLabel;
	UI::Text* layerNameLabel;
	UI::TextField* layerName;
	UI::Text* layerNameInfo;	

	static void ActivateLayerNameInfo(StringUTF8 text)
	{
		layerNameInfo->SetText(text);
		layerNameInfo->color = 0xff0000ff;
		layerNameLabel->SetActiveFlag(true);
	}
	static void DeactivateElementNameInfo()
	{
		layerNameInfo->SetActiveFlag(false);
	}

	static bool CheckName()
	{
		StringUTF8 name = layerName->GetText();
		if (name.CharacterCount() == 0)
		{
			ActivateLayerNameInfo("Cant have an empty name");
			return false;
		}
		else
		{
			DeactivateElementNameInfo();
			return true;
		}
	}

	static void CancelButton()
	{
		bgPanel->SetActiveFlag(false);
	}

	static void CreateButton()
	{		
		if (!CheckName())
			return;

		globals->gameUIManager.CreateLayer((const char*)layerName->GetText().Buffer());

		CancelButton();
	}

	static void NameEntered()
	{
		CheckName();
	}

	void Open()
	{
		bgPanel->SetActiveFlag(true);
	}
	void Setup()
	{
		const char* layer = "createLayerMenu";
		globals->uiManager.CreateLayer(layer);
		 
		CREATE_ELEMENT(bgPanel, layer);
		CREATE_ELEMENT(menuPanel		, layer);
		CREATE_ELEMENT(createButton		, layer);
		CREATE_ELEMENT(cancelButton		, layer);
		CREATE_ELEMENT(menuLabel		, layer);
		CREATE_ELEMENT(layerNameLabel	, layer);
		CREATE_ELEMENT(layerName		, layer);
		CREATE_ELEMENT(layerNameInfo	, layer);

		bgPanel->UIElement::SetProperties({
			.name = (String)"createLayerMenu_panel", .active = false
			});
		bgPanel->SetProperties(GetBGPanelProperties());
		globals->uiManager.AddViewportChangedFunction([&]() { bgPanel->SetSize((Vec2f)globals->uiManager.GetViewport().size); });

		menuPanel->UIElement::SetProperties({
			.size = Vec2f(300, 100), .anchor = bgPanel
			});
		menuPanel->SetProperties(GetMenuPanelProperties());		

		cancelButton->UIElement::SetProperties({
			.pos = Vec2f(75, 10), .size = Vec2f(70, 20), .anchor = menuPanel, .localAlign = Align::Bottom, .anchorAlign = Align::Bottom
			});
		cancelButton->SetProperties(DefaultTextButtonProperties<14>("Cancel"));		
		cancelButton->AddPressedFunction(CancelButton);

		createButton->UIElement::SetProperties({
			.pos = Vec2f(-10, 0), .size = Vec2f(70, 20), .anchor = cancelButton, .localAlign = Align::Right, .anchorAlign = Align::Left
			});
		createButton->SetProperties(DefaultTextButtonProperties<14>("Create"));		
		createButton->AddPressedFunction(CreateButton);

		menuLabel->UIElement::SetProperties({
			.pos = Vec2f(0, -10), .anchor = menuPanel, .localAlign = Align::Top, .anchorAlign = Align::Top
			});
		menuLabel->SetProperties(DefaultTextProperties<18>("New layer"));

		layerNameLabel->UIElement::SetProperties({
			.pos = Vec2f(5, -40), .anchor = menuPanel, .localAlign = Align::TopLeft, .anchorAlign = Align::TopLeft
			});
		layerNameLabel->SetProperties(DefaultTextProperties<14>("Name:"));

		layerName->UIElement::SetProperties({
			.pos = Vec2f(5, 0), .size = Vec2f(100, 20), .anchor = layerNameLabel, .localAlign = Align::Left, .anchorAlign = Align::Right
			});
		layerName->SetProperties({
			.buttonProperties = DefaultTextButtonProperties<14>(),
			});
		layerName->AddValueEnteredFunction(NameEntered);

		layerNameInfo->UIElement::SetProperties({
			.pos = Vec2f(0, -5), .anchor = layerName, .localAlign = Align::TopLeft, .anchorAlign = Align::BottomLeft, .active = false
			});
		layerNameInfo->SetProperties(DefaultTextProperties<8>());
		layerNameInfo->SetFontSize(10);		
	}
}