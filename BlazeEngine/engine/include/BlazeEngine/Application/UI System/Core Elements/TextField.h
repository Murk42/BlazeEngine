#pragma once
#include "BlazeEngine/Application/UI System/UIElement.h"
#include "BlazeEngine/Application/UI System/Core Elements/TextButton.h"
#include "BlazeEngine/Event/EventManager.h"

namespace Blaze::UI
{
	class TextFieldManager;

	struct TextFieldProperties
	{
		TextButtonProperties buttonProperties;

		UIElementProperty<StringUTF8> text;
		UIElementProperty<StringUTF8> emptyText;
		UIElementProperty<std::function<bool(StringUTF8)>> validateTextFunction;		
		UIElementProperty<UIEventFunction> valueEntered;
	};

	class BLAZE_API TextField : public UIElement,
		public EventHandler<Event::TextInput>		
	{
		TextButton button;
		Panel cursor;

		StringUTF8 editingText;
		StringUTF8 text;
		StringUTF8 emptyText;

		uint cursorPos;

		bool focused;
		TimePoint focusTime;
		TimePoint backspaceTime;
		uint backspaceCount;
		
		void OnEvent(Event::TextInput) override;

		void UpdateText();
		void UpdateCursor();

		void Erase();
		void Insert(StringViewUTF8);
		void MoveLeft();
		void MoveRight();		
		
		UIEvent valueEntered;
		std::function<bool(StringUTF8)> validateTextFunction;
	public:
		TextField();

		void SetSize(Vec2f size) override;
		void SetActiveFlag(bool active) override;		
		void SetClickableFlag(bool clickable) { button.SetClickableFlag(clickable); }
		void SetTriggerableFlag(bool triggerable) { button.SetTriggerableFlag(triggerable); }
		void SetDepth(float value) override;		

		void SetBorderWidth(float width);
		void SetCornerRadius(float radius);

		void SetNormalColor(ColorRGBAf color);
		void SetHoveredColor(ColorRGBAf color);
		void SetPressedColor(ColorRGBAf color);
		void SetBorderColor(ColorRGBAf color);

		void SetFontResolution(FontResolution* fontResolution);
		void SetFontSize(size_t fontSize);
		void SetText(StringViewUTF8 text);
		void SetEmptyText(StringViewUTF8 text);
		
		void SetValidateTextFunction(const std::function<bool(StringUTF8)>& function) { validateTextFunction = function; }
		void AddValueEnteredFunction(const UIEventFunction& event) { valueEntered += event; }
		void AddPressedFunction(const UIEventFunction& event) { button.AddPressedFunction(event); }
		void AddReleasedFunction(const UIEventFunction& event) { button.AddReleasedFunction(event); }
		void AddEnteredFunction(const UIEventFunction& event) { button.AddEnteredFunction(event); }
		void AddLeftFunction(const UIEventFunction& event) { button.AddLeftFunction(event); }

		bool Focus();
		void ReturnFocus();

		bool IsFocused() const { return focused; }
		StringUTF8 GetText() const { return text; }
		StringUTF8 GetEmptyText() const { return emptyText; }
		FontResolution* GetFontResolution() const { return button.GetFontResolution(); }
		size_t GetFontSize() const { return button.GetFontSize(); }
		float GetBorderWidth() const { return button.GetBorderWidth(); }
		float GetCornerRadius() const { return button.GetCornerRadius(); }
		Vec2f GetCursorSize() const { return cursor.GetSize(); }

		bool IsTriggerable() const { return button.IsTriggerable(); }
		bool IsClickable() const { return button.IsClickable(); }

		void SetProperties(const TextFieldProperties&);

		static constexpr const char* typeName = "TextField";
		using ManagerType = TextFieldManager;

		friend TextFieldManager;
	};

	class BLAZE_API TextFieldManager : UIElementManager<TextField>
	{
	public:
		void Update(UIElement* element) override;
		void Render(UIElement* element) override;
	};
}