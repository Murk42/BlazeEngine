#include "BlazeEngine/Application/UI System/Core Elements/DropdownSelection.h"

namespace Blaze::UI
{
	void DropdownSelection::AttachedToManager()
	{
		auto* manager = GetManager();
		auto layer = GetLayerName();

		StringView menuLayer;

		if (this->menuLayer.Size() == 0)
			menuLayer = layer;
		else
			menuLayer = this->menuLayer;

		manager->AddElement(&menu, menuLayer);
		manager->AddElement(&hoverTestButton, menuLayer);
	}
	void DropdownSelection::DetachedFromManager()
	{
		auto* manager = GetManager();

		manager->RemoveElement(&menu);
		manager->RemoveElement(&hoverTestButton);
	}
	DropdownSelection::DropdownSelection()
		: open(false)
	{
		TieElement(&button);

		button.SetAnchor(this);
		button.SetAnchorAlignment(Align::TopLeft);
		button.SetLocalAlignment(Align::TopLeft);
		button.sizeChanged += [&]() {
			hoverTestButton.SetSize({
				std::max(menu.GetSize().x, button.GetSize().x),
				menu.GetSize().y + button.GetSize().y + std::abs(menu.GetPos().y)
				});			

			Vec2f panelSize = button.GetSize();
			this->UIElement::SetSize(panelSize);
		};
		button.SetBorderWidth(0);
		button.SetCornerRadius(3);

		button.SetTextAlignment(Align::Left, Align::Left, Vec2f(5, 0));
		button.AddPressedFunction([&]() {
			SetOpenFlag(!open);
			});

		hoverTestButton.SetAnchor(this);
		hoverTestButton.SetAnchorAlignment(Align::TopLeft);
		hoverTestButton.SetLocalAlignment(Align::TopLeft);
		hoverTestButton.SetSolidFlag(false);
		hoverTestButton.clickable = false;
		hoverTestButton.triggerable = true;
		hoverTestButton.left += [&]() {
				SetOpenFlag(false);
			};		
		
		menu.SetAnchor(&button);
		menu.SetAnchorAlignment(Align::BottomLeft);
		menu.SetLocalAlignment(Align::TopLeft);
		menu.SetPos({ 0, -1 });
		menu.SetBorderWidth(0);
		menu.SetCornerRadius(3);
		menu.sizeChanged += [&]() {
			hoverTestButton.SetSize({
				std::max(menu.GetSize().x, button.GetSize().x),
				menu.GetSize().y + button.GetSize().y + std::abs(menu.GetPos().y)
				});
		};
		menu.AddOptionSelectedFunction((UIEvent)[&]() {
			button.SetText(menu.GetSelectedOption().value);
			SetOpenFlag(false);
			});

		SetOpenFlag(false);

		hoverTestButton.SetSize({
				std::max(menu.GetSize().x, button.GetSize().x),
				menu.GetSize().y + button.GetSize().y + std::abs(menu.GetPos().y)
			});		
	}
	void DropdownSelection::SetMenuLayer(StringView layer)
	{
		this->menuLayer = layer;
		if (GetManager() != nullptr)
		{
			menu.ChangeLayer(layer);
			hoverTestButton.ChangeLayer(layer);
		}
	}
	void DropdownSelection::SetSize(Vec2f size)
	{
		button.SetSize(size);
		UIElement::SetSize(size);
	}
	void DropdownSelection::SetActiveFlag(bool active)
	{		
		button.SetActiveFlag(active);
		UIElement::SetActiveFlag(active);
	}
	void DropdownSelection::SetDepth(float value)
	{		
		button.SetDepth(value);
		menu.SetDepth(value);
	}

	void DropdownSelection::SelectOption(uint index)
	{
		menu.SelectOption(index);
	}
	void DropdownSelection::SetOptionProperties(const TextButtonProperties& optionProperties)
	{
		menu.SetOptionProperties(optionProperties);
	}
	void DropdownSelection::SetOptions(const std::vector<StringUTF8>& options)
	{
		menu.SetOptions(options);		
	}

	void DropdownSelection::SetOpenFlag(bool open)
	{
		this->open = open;
		menu.SetActiveFlag(open);
		hoverTestButton.SetActiveFlag(open);
	}

	void DropdownSelection::SetFontResolution(FontResolution* fontResolution)
	{
		button.SetFontResolution(fontResolution);		
	}
	void DropdownSelection::SetFontSize(float fontSize)
	{
		button.SetFontSize(fontSize);		
	}

	void DropdownSelection::SetProperties(const DropdownSelectionProperties& p)
	{
		button.SetProperties(p.buttonProperties);
		menu.SetProperties(p.menuProperties);
		if (p.menuLayer.edited) SetMenuLayer(p.menuLayer.value);
	}

	void DropdownSelectionManager::Update(UIElement* ptr)
	{
		DropdownSelection* el = (DropdownSelection*)ptr;

		//GetManager()->GetElementManager<Button>().Update(&el->hoverTestButton);
		GetManager()->GetElementManager<TextButton>().Update(&el->button);
	}
	void DropdownSelectionManager::Render(UIElement* ptr)
	{
		DropdownSelection* el = (DropdownSelection*)ptr;
		//GetManager()->GetElementManager<Button>().Render(& el->hoverTestButton);
		GetManager()->GetElementManager<TextButton>().Render(&el->button);
	}
}