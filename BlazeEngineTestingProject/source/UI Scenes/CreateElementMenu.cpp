#include "Globals.h"

#include "SceneListMenu.h"
#include "CreateLayerMenu.h"
#include "ElementPropertiesMenu.h"

namespace CreateElementMenu
{
	UI::Panel* bgPanel;
	UI::Panel* menuPanel;

	UI::TextButton* cancelButton;
	UI::TextButton* createButton;
	UI::Text* menuLabel;

	UI::Text* elementNameLabel;
	UI::TextField* elementName;
	UI::Text* elementNameInfo;
	UI::Text* elementTypeLabel;
	UI::DropdownSelection* elementType;
	UI::Text* elementLayerLabel;
	UI::DropdownSelection* elementLayer; 	

	UI::List* propertiesList;

	UI::ImageButton* newLayerButton;

	static void ActivateElementNameInfo(StringUTF8 text)
	{
		elementNameInfo->SetText(text);
		elementNameInfo->color = 0xff0000ff;
		if (!elementNameInfo->IsActive())
			propertiesList->AddElement(elementNameInfo, 1, elementNameLabel->GetSize().x + 5);

		elementNameInfo->SetActiveFlag(true);
	}
	static void DeactivateElementNameInfo()
	{
		if (elementNameInfo->IsActive())
			propertiesList->RemoveElement(1);

		elementNameInfo->SetActiveFlag(false);
	}

	static bool CheckName()
	{
		String name = (const char*)elementName->GetText().Buffer();
		if (name.Size() == 0)
		{
			ActivateElementNameInfo("Cant have an empty name");
			return false;
		}
		else if (!SceneListMenu::GetSelectedScene()->IsAvailableName(name))
		{
			ActivateElementNameInfo("Name already taken");
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

		uint typeIndicies[] = {
			globals->gameUIManager.GetElementTypeRegistry().GetElementTypeIndex<UI::Button>(),
			globals->gameUIManager.GetElementTypeRegistry().GetElementTypeIndex<UI::Text>(),
			globals->gameUIManager.GetElementTypeRegistry().GetElementTypeIndex<UI::Panel>(),
			globals->gameUIManager.GetElementTypeRegistry().GetElementTypeIndex<UI::Image>(),
		};

		String name = String((const char*)elementName->GetText().Buffer());

		uint typeIndex = typeIndicies[elementType->GetSelectedOptionIndex()];

		UI::UIScene* gameScene = SceneListMenu::GetSelectedScene();
		UI::UIElement* element = gameScene->CreateElement(name, typeIndex, (const char*)elementLayer->GetSelectedOption().value.Buffer());

		ElementPropertiesMenu::SelectElement(element);

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
		const char* layer = "createElementMenu";
		const char* menuLayer = "createElementMenu_menu";
		globals->uiManager.CreateLayer(layer);
		globals->uiManager.CreateLayer(menuLayer);
		 
		CREATE_ELEMENT(bgPanel			, layer);
		CREATE_ELEMENT(menuPanel		, layer);
		CREATE_ELEMENT(createButton		, layer);
		CREATE_ELEMENT(cancelButton		, layer);
		CREATE_ELEMENT(menuLabel		, layer);
		CREATE_ELEMENT(elementNameLabel	, layer);
		CREATE_ELEMENT(elementName		, layer);
		CREATE_ELEMENT(elementNameInfo	, layer);
		CREATE_ELEMENT(elementTypeLabel	, layer);
		CREATE_ELEMENT(elementType		, layer);
		CREATE_ELEMENT(elementLayerLabel, layer);
		CREATE_ELEMENT(elementLayer, layer);
		CREATE_ELEMENT(propertiesList	, layer);
		CREATE_ELEMENT(newLayerButton	, layer);

		bgPanel->UIElement::SetProperties({
			.name = (String)"createElementMenu_panel", .localAlign = Align::Center, .anchorAlign = Align::Center, .active = false
			});
		bgPanel->SetProperties(GetBGPanelProperties());
		globals->uiManager.AddViewportChangedFunction([&]() { bgPanel->SetSize((Vec2f)globals->uiManager.GetViewport().size); });

		menuPanel->UIElement::SetProperties({
			.size = Vec2f(500, 300), .anchor = bgPanel
			});
		menuPanel->SetProperties(GetMenuPanelProperties());

		cancelButton->UIElement::SetProperties({
			.pos = Vec2f(-10, 10), .size = Vec2f(70, 20), .anchor = menuPanel, .localAlign = Align::BottomRight, .anchorAlign = Align::BottomRight 
			});
		cancelButton->SetProperties(DefaultTextButtonProperties<14>("Cancel"));
		cancelButton->AddPressedFunction(CancelButton);

		createButton->UIElement::SetProperties({
			.pos = Vec2f(-10, 0), .size = Vec2f(70, 20), .anchor = cancelButton, .localAlign = Align::Right, .anchorAlign = Align::Left 
			});
		createButton->SetProperties(DefaultTextButtonProperties<14>("Create"));		
		createButton->AddPressedFunction(CreateButton);

		menuLabel->UIElement::SetProperties({ .pos = Vec2f(0, -10), .anchor = menuPanel, .localAlign = Align::Top, .anchorAlign = Align::Top });
		menuLabel->SetProperties(DefaultTextProperties<18>("New element"));		
		
		elementNameLabel->SetProperties(DefaultTextProperties<14>("Name:"));		

		elementName->UIElement::SetProperties({ 
			.pos = Vec2f(5, 0), .size = Vec2f(100, 20), .anchor = elementNameLabel, .localAlign = Align::Left, .anchorAlign = Align::Right 
			});
		elementName->SetProperties({ 
			.buttonProperties = DefaultTextButtonProperties<14>(),
			.emptyText = (StringUTF8)"Enter name"}
		);		
		elementName->AddValueEnteredFunction(NameEntered);

		elementNameInfo->UIElement::SetProperties({
			.pos = Vec2f(0, -5), .anchor = elementName, .localAlign = Align::TopLeft, .anchorAlign = Align::BottomLeft, .active = false
			});
		elementNameInfo->SetProperties(DefaultTextProperties<8>());
		elementNameInfo->SetProperties({ .fontSize = 10 });
		
		elementTypeLabel->SetProperties(DefaultTextProperties<14>("Type:"));		

		std::vector<StringUTF8> elementTypeOptions = { "Button", "Text", "Panel", "Image" };
		elementType->UIElement::SetProperties({  
			.pos = Vec2f(5, 0), .size = Vec2f(60, 20), .anchor = elementTypeLabel, .localAlign = Align::Left, .anchorAlign = Align::Right
			});
		elementType->SetProperties({
			.buttonProperties = DefaultTextButtonProperties<14>(),
			.menuProperties = {
				.optionProperties = DefaultTextButtonProperties<14>(),
				.panelProperties = GetMenuPanelProperties(),
				.options = elementTypeOptions, 
				.defaultOption = 0 },
			.menuLayer = (String)menuLayer 
			});
		//elementType->SetProperties({
		//	.menuProperties = {
		//		.optionProperties = {
		//			.textLocalAlign = Align::Left, .textAnchorAlign = Align::Left, .textOffset = Vec2f(5, 0) }}
		//	});
				
		elementLayerLabel->SetProperties(DefaultTextProperties<14>("Layer"));

		elementLayer->UIElement::SetProperties({
			.name = (String)"createElementMenu_elementLayer", .pos = Vec2f(5, 0), .size = Vec2f(200, 20), .anchor = elementLayerLabel, .localAlign = Align::Left, .anchorAlign = Align::Right
			});
		elementLayer->SetProperties({
			.buttonProperties = DefaultTextButtonProperties<14>(),
			.menuProperties = {
				.optionProperties = DefaultTextButtonProperties<14>(),
				.panelProperties = GetMenuPanelProperties(),
				.options = { },
				.defaultOption = -1 },
			.menuLayer = (String)menuLayer
			});
		elementLayer->SetProperties({
			.menuProperties = {
				.optionProperties = {
					.textLocalAlign = Align::Left, .textAnchorAlign = Align::Left, .textOffset = Vec2f(5, 0) }}
			});
		
		propertiesList->UIElement::SetProperties({
			.pos = Vec2f(10, -50), .anchor = menuPanel, .localAlign = Align::TopLeft, .anchorAlign = Align::TopLeft, .solid = false
			});
		propertiesList->SetProperties(UI::ListProperties({
			.elementsOffset = 3
			}));
		propertiesList->AddElement(elementNameLabel, -1, 0, 12, 7);
		propertiesList->AddElement(elementTypeLabel, -1, 0, 12, 7);
		propertiesList->AddElement(elementLayerLabel, -1, 0, 12, 7);

		newLayerButton->SetProperties({
			.pos = { 5, 0}, .size = { 20, 20}, .anchor = elementLayer, .localAlign = Align::Left,.anchorAlign = Align::Right,
			.borderWidth = 0,
			.texture = globals->resourceManager.GetResource<Graphics::Core::Texture2D>("plusIcon"), .sourceRect = { 0, 0, 20, 20}, .imageSize = { 20, 20},
			.imageMask = 0x00aa00ff,
			.pressed = (UI::UIEventFunction)[]() {
				CreateLayerMenu::Open();
				}
			});
	}
}