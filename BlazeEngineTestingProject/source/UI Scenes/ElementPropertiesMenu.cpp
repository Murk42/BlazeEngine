#include "Globals.h"

namespace ElementPropertiesMenu
{
	UI::Panel* panel;
	UI::Text* label;
	UI::Text* nameLabel;
	UI::TextField* name;
	UI::Text* nameInfo;
	UI::Text* posLabel;
	UI::TextField* posX;
	UI::TextField* posY;
	UI::Text* sizeLabel;
	UI::TextField* sizeX;
	UI::TextField* sizeY;
	UI::Text* elementAnchor;
	UI::Text* elementLocalAlignLabel;
	UI::DropdownSelection* elementLocalAlign;
	UI::Text* elementAnchorAlignLabel;
	UI::DropdownSelection* elementAnchorAlign;
	UI::List* list;

	UI::UIElement* selected;

	static void ActivateElementNameInfo(StringUTF8 text)
	{
		nameInfo->SetText(text);
		nameInfo->color = 0xff0000ff;		

		if (!nameInfo->IsActive())
			list->AddElement(nameInfo, 1, nameLabel->GetSize().x + 5);

		nameInfo->SetActiveFlag(true);
	}
	static void DeactivateElementNameInfo()
	{		
		if (nameInfo->IsActive())
			list->RemoveElement(1);
		nameInfo->SetActiveFlag(false);
	}

	void SelectElement(UI::UIElement* el)
	{
		selected = el;

		auto SetClickable = [&](bool c) {
			name->SetClickableFlag(c);
			posX->SetClickableFlag(c);
			posY->SetClickableFlag(c);
			sizeX->SetClickableFlag(c);
			sizeY->SetClickableFlag(c);
		};

		if (el == nullptr)
		{
			SetClickable(false);
			name->SetText("");
			posX->SetText("");
			posY->SetText("");
			sizeX->SetText("");
			sizeY->SetText("");
		
			return;
		}
		else
		{
			SetClickable(true);
			name->SetText((StringUTF8)el->GetName());
			posX->SetText((StringUTF8)String::Convert((int)el->GetPos().x));
			posY->SetText((StringUTF8)String::Convert((int)el->GetPos().y));
			sizeX->SetText((StringUTF8)String::Convert((int)el->GetSize().x));
			sizeY->SetText((StringUTF8)String::Convert((int)el->GetSize().y));
		}
	}

	static void RenameElement(String name)
	{
		if (selected->GetName() == name)
			return;

		bool valid = true;
		auto* scene = selected->GetScene();

		if (name.Size() == 0)
		{
			ActivateElementNameInfo("Cant have an empty name");
			valid = false;
		}
		else if (!scene->IsAvailableName(name))
		{
			ActivateElementNameInfo("Name already taken");
			valid = false;
		}
		
		if (valid)
		{
			DeactivateElementNameInfo();
			selected->SetName(name);
		}
		else
		{
			ElementPropertiesMenu::name->SetText((StringUTF8)selected->GetName());
		}
	}

	void Setup()
	{
		const char* layer = "elementPropertiesMenu";

		uiManager.CreateLayer(layer);

		CREATE_ELEMENT(panel, layer);
		CREATE_ELEMENT(label, layer);
		CREATE_ELEMENT(nameLabel, layer);
		CREATE_ELEMENT(name, layer);
		CREATE_ELEMENT(nameInfo, layer);
		CREATE_ELEMENT(posLabel, layer);
		CREATE_ELEMENT(posX, layer);
		CREATE_ELEMENT(posY, layer);
		CREATE_ELEMENT(sizeLabel, layer);
		CREATE_ELEMENT(sizeX, layer);
		CREATE_ELEMENT(sizeY, layer);
		CREATE_ELEMENT(list, layer);

		panel->UIElement::SetProperties({
			.pos = Vec2f(-15, -50), .size = Vec2f(300, 500), .localAlign = Align::TopRight, .anchorAlign = Align::TopRight
			});
		panel->SetProperties(menuPanelProperties);

		label->UIElement::SetProperties({
			.pos = Vec2f(0, -5), .anchor = panel, .localAlign = Align::Top, .anchorAlign = Align::Top
			});
		label->SetProperties({
			.text = (StringUTF8)"Element properties", .fontResolution = fontResolution18, .fontSize = 18
			});
		
		nameLabel->SetProperties({
			.text = (StringUTF8)"Name:", .fontResolution = fontResolution14, .fontSize = 14,
			});

		name->UIElement::SetProperties({
			.pos = Vec2f(5, 0), .size = Vec2f(100, 20), .anchor = nameLabel, .localAlign = Align::Left, .anchorAlign = Align::Right
			});
		name->SetProperties({
			.buttonProperties = DefaultTextButtonProperties<14>(),
			.emptyText = (StringUTF8)"",
			.valueEntered = (UI::UIEventFunction)[&]() {
				String name = (char*)ElementPropertiesMenu::name->GetText().Buffer();
				RenameElement(name);
			}
			});
		
		nameInfo->SetActiveFlag(false);
		nameInfo->SetProperties(DefaultTextProperties<8>());
		nameInfo->SetFontSize(10);
		
		posLabel->SetProperties({
			.text = (StringUTF8)"Pos:", .fontResolution = fontResolution14, .fontSize = 14,
			});

		posX->UIElement::SetProperties({
			.pos = Vec2f(5, 0), .size = Vec2f(50, 20), .anchor = posLabel, .localAlign = Align::Left, .anchorAlign = Align::Right,
			});
		posX->SetProperties({
			.buttonProperties = DefaultTextButtonProperties<14>(),
			.emptyText = (StringUTF8)"",
			.valueEntered = (UI::UIEventFunction)[&]() {
				float value;
				if (StringUTF8::ConvertTo<float>(posX->GetText(), value))
					selected->SetPos({ std::floor(value), selected->GetPos().y});
				else
					posX->SetText(StringUTF8::Convert((int)selected->GetPos().x));
			}
			});

		posY->UIElement::SetProperties({
			.pos = Vec2f(5, 0), .size = Vec2f(50, 20), .anchor = posX, .localAlign = Align::Left, .anchorAlign = Align::Right
			});
		posY->SetProperties({
			.buttonProperties = DefaultTextButtonProperties<14>(),
			.emptyText = (StringUTF8)"",
			.valueEntered = (UI::UIEventFunction)[&]() {
				float value;
				if (StringUTF8::ConvertTo<float>(posY->GetText(), value))
					selected->SetPos({ selected->GetPos().x, std::floor(value) });
				else
					posY->SetText(StringUTF8::Convert((int)selected->GetPos().y));
			}
			});
		
		sizeLabel->UIElement::SetProperties({
			.pos = Vec2f(5, 0), .anchor = posY, .localAlign = Align::Left, .anchorAlign = Align::Right
			});
		sizeLabel->SetProperties({
			.text = (StringUTF8)"Size:", .fontResolution = fontResolution14, .fontSize = 14,
			});		

		sizeX->UIElement::SetProperties({
			.pos = Vec2f(5, 0), .size = Vec2f(50, 20), .anchor = sizeLabel, .localAlign = Align::Left, .anchorAlign = Align::Right,
			});
		sizeX->SetProperties({
			.buttonProperties = DefaultTextButtonProperties<14>(),
			.emptyText = (StringUTF8)"",
			.valueEntered = (UI::UIEventFunction)[&]() {
				float value;
				if (StringUTF8::ConvertTo<float>(sizeX->GetText(), value))
					selected->SetSize({ std::floor(value), selected->GetSize().y});
				else
					sizeX->SetText(StringUTF8::Convert((int)selected->GetSize().x));
			}
			});

		sizeY->UIElement::SetProperties({
			.pos = Vec2f(5, 0), .size = Vec2f(50, 20), .anchor = sizeX, .localAlign = Align::Left, .anchorAlign = Align::Right,
			});
		sizeY->SetProperties({
			.buttonProperties = DefaultTextButtonProperties<14>(),
			.emptyText = (StringUTF8)"",
			.valueEntered = (UI::UIEventFunction)[&]() {
				float value;
				if (StringUTF8::ConvertTo<float>(sizeY->GetText(), value))
					selected->SetSize({ selected->GetSize().x, std::floor(value) });
				else
					sizeY->SetText(StringUTF8::Convert((int)selected->GetSize().y));
			}
			});

		list->UIElement::SetProperties({
			.pos = Vec2f(5, -30), .anchor = panel, .localAlign = Align::TopLeft, .anchorAlign = Align::TopLeft
			});
		list->SetProperties({
			.elementsOffset = 3,
			});
		list->SetElements({
			{.ptr = nameLabel, .topSpacing = 12, .bottomSpacing = 7 },
			{.ptr = posLabel, .topSpacing = 12, .bottomSpacing = 7}
			});

		SelectElement(nullptr);
	}
}