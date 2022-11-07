#pragma once
#include "BlazeEngine/Core/EngineCore.h"
#include "BlazeEngine/Application/UI System/UIElement.h"
#include "BlazeEngine/Application/UI System/Core Elements/List.h"
#include "BlazeEngine/Application/UI System/Core Elements/TextButton.h"
#include "BlazeEngine/Core/ResultValue.h"

namespace Blaze::UI
{
	class SelectionMenuManager;

	struct SelectionMenuProperties
	{		
		TextButtonProperties optionProperties;
		PanelProperties panelProperties;
		
		UIElementProperty<std::vector<StringUTF8>> options;
		UIElementProperty<int> defaultOption;
		UIElementProperty<UIEvent> optionSelected;
	};

	class BLAZE_API SelectionMenu : public UIElement
	{		
		List list;
		Panel panel;
		
		uint optionCount;
		TextButton* options;
		int selected;

		UIEvent optionSelected;

		TextButtonProperties optionProperties;		
	public:
		SelectionMenu();
		~SelectionMenu();

		void SetSize(Vec2f size) override;
		void SetDepth(float value) override;

		void SetBorderColor(ColorRGBAf color) { panel.borderColor = color; }
		void SetBorderWidth(float width) { panel.borderWidth = width; }
		void SetCornerRadius(float radius) { panel.cornerRadius = radius; }

		void SetOptionProperties(const TextButtonProperties& p);

		void AddOptionSelectedFunction(const UIEvent& event) { optionSelected += event; }

		Result SelectOption(int index);
		int GetSelectedOptionIndex() const { return selected; }
		ResultValue<StringUTF8> GetSelectedOption() const;
		ResultValue<TextButton*> GetOptionElement(int index) const;
		uint GetOptionCount() const { return optionCount; }
		void SetOptions(const std::vector<StringUTF8>& options);

		void SetProperties(const SelectionMenuProperties&);

		static constexpr const char* typeName = "SelectionMenu";
		using ManagerType = SelectionMenuManager;

		friend class SelectionMenuManager;
	};

	class BLAZE_API SelectionMenuManager : UIElementManager<SelectionMenu>
	{
	public:
		void Update(UIElement*) override;
		void Render(UIElement*) override;
	};
}