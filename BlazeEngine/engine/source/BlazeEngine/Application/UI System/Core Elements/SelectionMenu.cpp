#include "BlazeEngine/Application/UI System/Core Elements/SelectionMenu.h"

namespace Blaze::UI
{	
	SelectionMenu::SelectionMenu()
		: selected(0), optionCount(0), options(nullptr)
	{
		TieElement(&list);
		TieElement(&panel);

		panel.SetPos({ 0, -2 });
		panel.SetSize({ 150, 50 });
		panel.SetAnchor(this);
		panel.SetLocalAlignment(Align::TopLeft);
		panel.SetAnchorAlignment(Align::TopLeft);

		list.SetAnchor(&panel);
		list.SetPos({ 4, -4 });
		list.SetAnchorAlignment(Align::TopLeft);
		list.SetLocalAlignment(Align::TopLeft);
		list.SetElementOffset(2);

		list.UpdateSize();
		
		panel.sizeChanged += [&]() {
			for (int i = 0; i < optionCount; ++i)
				options[i].SetSize({ panel.GetSize().x - 8, (float)optionProperties.textProperties.fontSize});
		};
	}

	SelectionMenu::~SelectionMenu()
	{
		if (options)
			delete[] options;
	}

	void SelectionMenu::SetSize(Vec2f size)
	{
		panel.SetSize(size);
		UIElement::SetSize(size);
	}
	void SelectionMenu::SetDepth(float value)
	{
		panel.SetDepth(value);
		list.SetDepth(value);
		UIElement::SetDepth(value);
	}

	void SelectionMenu::SetOptionProperties(const TextButtonProperties& p)
	{		
		optionProperties = p;
		optionProperties.textProperties.text.edited = false;
		for (int i = 0; i < optionCount; ++i)		
			options[i].SetProperties(optionProperties);		
	}

	Result SelectionMenu::SelectOption(int index)
	{
		if (index < -1 || index >= optionCount)
			return BLAZE_ERROR_RESULT("Blaze Engine", "Index out of bounds");

		selected = index;		

		GetManager()->AddEventToQueue(optionSelected);

		return { };
	}
	ResultValue<StringUTF8> SelectionMenu::GetSelectedOption() const
	{
		if (selected == -1)
			return { StringUTF8(), BLAZE_ERROR_RESULT("Blaze Engine", "No option was selected") };
		else
			return { options[selected].GetText(), {} };
	}

	ResultValue<TextButton*> SelectionMenu::GetOptionElement(int index) const
	{
		if (selected == -1)
			return { nullptr, BLAZE_ERROR_RESULT("Blaze Engine", "Index out of bounds") };
		else
			return { &options[selected], {} };
	}

	void SelectionMenu::SetOptions(const std::vector<StringUTF8>& options)
	{

		StringUTF8 oldSelection;
		uint oldSelected = selected;
		if (selected >= 0 && selected < optionCount)
			oldSelection = this->options[selected].GetText();

		if (selected >= options.size())
			selected = -1;		

		if (options.size() == 0)
		{
			this->options = nullptr;
			panel.SetSize(Vec2f(150, 50));
			panel.SetAnchor(this);
			optionCount = 0;
			return;
		}

		delete[] this->options;
		this->options = new TextButton[options.size()];
		optionCount = options.size();

		list.SetElements({});

		for (int i = 0; i < options.size(); ++i)
		{
			if (GetManager() != nullptr)
				TieElement(&this->options[i]);

			this->options[i].SetSize(Vec2f(panel.GetSize().x - 8, optionProperties.textProperties.fontSize));
			this->options[i].SetClipElement(&panel);
			this->options[i].SetProperties(optionProperties);
			this->options[i].SetText(options[i]);
			this->options[i].AddPressedFunction([&, i]() { SelectOption(i); });

			list.AddElement(&this->options[i]);
		}

		SetSize({ 150, std::clamp<float>(list.GetSize().y + 8, 10, 150) });

		if (selected != oldSelected || selected != -1 && options[selected] != oldSelection)
			GetManager()->AddEventToQueue(optionSelected);
	}	
	void SelectionMenu::SetProperties(const SelectionMenuProperties& p)
	{
		panel.SetProperties(p.panelProperties);
		SetOptionProperties(p.optionProperties);
		if (p.options.edited) SetOptions(p.options);
		if (p.optionSelected.edited) AddOptionSelectedFunction(p.optionSelected);
		if (p.defaultOption.edited) SelectOption(p.defaultOption);
	}
	void SelectionMenuManager::Update(UIElement* el)
	{
		SelectionMenu* ptr = (SelectionMenu*)el;
		GetManager()->GetElementManager<UI::Panel>().Update(&ptr->panel);
		GetManager()->GetElementManager<UI::List>().Update(&ptr->list);

		for (int i = 0; i < ptr->optionCount; ++i)
			GetManager()->GetElementManager<UI::TextButton>().Update(&ptr->options[i]);
	}
	void SelectionMenuManager::Render(UIElement* el)
	{
		if (!el->IsActive())
			return;
		SelectionMenu* ptr = (SelectionMenu*)el;
		GetManager()->GetElementManager<UI::Panel>().Render(&ptr->panel);
		GetManager()->GetElementManager<UI::List>().Render(&ptr->list);

		for (int i = 0; i < ptr->optionCount; ++i)
			GetManager()->GetElementManager<UI::TextButton>().Render(&ptr->options[i]);
	}
}