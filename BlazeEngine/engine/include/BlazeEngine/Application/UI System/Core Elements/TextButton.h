#pragma once
#include "BlazeEngine/Application/UI System/UIElement.h"
#include "BlazeEngine/Application/UI System/Core Elements/Panel.h"
#include "BlazeEngine/Application/UI System/Core Elements/Text.h"
#include "BlazeEngine/Application/UI System/Core Elements/Button.h"

namespace Blaze::UI
{
	class TextButtonManager;	
	
	struct TextButtonProperties
	{
		PanelProperties panelProperties;
		TextProperties textProperties;
		ButtonProperties buttonProperties;

		UIElementProperty<ColorRGBAf> normalColor;
		UIElementProperty<ColorRGBAf> hoveredColor;
		UIElementProperty<ColorRGBAf> pressedColor;

		UIElementProperty<Align> textLocalAlign;
		UIElementProperty<Align> textAnchorAlign;
		UIElementProperty<Vec2f> textOffset;
	};

	class BLAZE_API TextButton : public UIElement
	{
		Panel panel;
		Text text;
		Button button;
		ColorRGBAf normalColor;
		ColorRGBAf pressedColor;
		ColorRGBAf hoveredColor;		
	public:
		TextButton();
		~TextButton();
		
		void SetSize(Vec2f size) override;
		
		void SetActiveFlag(bool active) override;		
		void SetClickableFlag(bool clickable) { button.clickable = clickable; }
		void SetTriggerableFlag(bool triggerable) { button.triggerable = triggerable; }
		void SetDepth(float value) override;

		void SetNormalColor(ColorRGBAf color);
		void SetPressedColor(ColorRGBAf color);
		void SetHoveredColor(ColorRGBAf color);
		void SetBorderColor(ColorRGBAf color) { panel.borderColor = color; }
		void SetBorderWidth(float width) { panel.borderWidth = width; }
		void SetCornerRadius(float radius) { panel.cornerRadius = radius; }

		void SetText(StringViewUTF8 text) { this->text.SetText(text); }
		void SetFontResolution(FontResolution* fontResolution) { text.SetFontResolution(fontResolution); }
		void SetFontSize(size_t fontSize) { text.SetFontSize(fontSize); }
		void SetTextColor(ColorRGBAf color) { text.color = color; }
		void SetTextAlignment(Align local, Align anchor, Vec2f offset);
		void SetTextLocalAlignment(Align align);
		void SetTextAnchorAlignment(Align align);
		void SetTextOffset(Vec2f offset);

		void AddPressedFunction(const UIEventFunction& event) { button.pressed += event; }
		void AddReleasedFunction(const UIEventFunction& event) { button.released += event; }
		void AddEnteredFunction(const UIEventFunction& event) { button.entered += event; }
		void AddLeftFunction(const UIEventFunction& event) { button.left += event; }

		ColorRGBAf GetNormalColor() const { return normalColor; }
		ColorRGBAf GetHoveredColor() const { return hoveredColor; }
		ColorRGBAf GetPressedColor() const { return pressedColor; }
		ColorRGBAf GetBorderColor() const { return panel.borderColor; }
		float GetBorderWidth() const { return panel.borderWidth; }
		float GetCornerRadius() const { return panel.cornerRadius; }

		StringUTF8 GetText() const { return text.GetText(); }
		FontResolution* GetFontResolution() const { return text.GetFontResolution(); }
		size_t GetFontSize() const { return text.GetFontSize(); }
		ColorRGBAf GetTextColor() const { return text.color; }

		Text& GetTextElement() { return text; }
		const Text& GetTextElement() const { return text; }
		Panel& GetPanelElement() { return panel; }
		const Panel& GetPanelElement() const { return panel; }

		bool IsTriggerable() const { return button.triggerable; }
		bool IsClickable() const { return button.clickable; }
				
		void SetProperties(const TextButtonProperties&);

		static constexpr const char* typeName = "TextButton";
		using ManagerType = TextButtonManager;

		friend class TextButtonManager;
	};

	class BLAZE_API TextButtonManager : UIElementManager<TextButton>
	{
	public:
		void Render(UIElement*) override;
		void Update(UIElement*) override;
	};
}