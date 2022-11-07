#pragma once
#include "BlazeEngine/Core/EngineCore.h"
#include "BlazeEngine/Application/UI System/UIElement.h"
#include "BlazeEngine/Application/UI System/Core Elements/TextButton.h"
#include "BlazeEngine/Application/UI System/Core Elements/SelectionMenu.h"

namespace Blaze::UI
{
	class DropdownSelectionManager;

	struct DropdownSelectionProperties
	{
		TextButtonProperties buttonProperties;
		SelectionMenuProperties menuProperties;

		UIElementProperty<String> menuLayer;
	};

	class BLAZE_API DropdownSelection : public UIElement
	{
		TextButton button;
		Button hoverTestButton;
		SelectionMenu menu;
		
		bool open;

		String menuLayer;

		void AttachedToManager() override;
		void DetachedFromManager() override;
	public:
		DropdownSelection();

#pragma region
		void SetSize(Vec2f size) override;
		void SetActiveFlag(bool active) override;
		void SetDepth(float value) override;
#pragma endregion Element setters
#pragma region
		void SetMenuLayer(StringView layer);
		void SelectOption(uint index);
		void SetOptionProperties(const TextButtonProperties& optionProperties);
		void SetOptions(const std::vector<StringUTF8>& options);
#pragma endregion Menu setters
#pragma region
		void SetFontResolution(FontResolution* fontResolution);
		void SetFontSize(float fontSize);
		void SetOpenFlag(bool open);
#pragma endregion Specific setters
#pragma region
		void AddOptionSelectedFunction(const UIEvent& event) { menu.AddOptionSelectedFunction(event); }
#pragma endregion Event setters
	
#pragma region
		int GetSelectedOptionIndex() const { return menu.GetSelectedOptionIndex(); }
		ResultValue<StringUTF8> GetSelectedOption() const { return menu.GetSelectedOption(); }
		ResultValue<TextButton*> GetOptionElement(int index) const { return menu.GetOptionElement(index); }
		uint GetOptionCount() const { return menu.GetOptionCount(); }
#pragma endregion Menu getters
#pragma region 
		FontResolution* GetFontResolution() const { return button.GetFontResolution(); }
		float GetFontSize() const { return button.GetFontSize(); }
		bool IsOpen() const { return open; }
#pragma endregion Specific getters


		void SetProperties(const DropdownSelectionProperties&);

		static constexpr const char* typeName = "DropdownSelection";
		using ManagerType = DropdownSelectionManager;
		friend class DropdownSelectionManager;
	};

	class BLAZE_API DropdownSelectionManager : UIElementManager<DropdownSelection>
	{
	public:
		void Update(UIElement*) override;
		void Render(UIElement*) override;
	};
}