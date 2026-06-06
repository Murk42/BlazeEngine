#pragma once
#include "BlazeEngine/UI/Common/TextInputBase.h"
#include <functional>

namespace Blaze::UI::Nodes 
{ 	
	class TextInput : public TextInputBase
	{
	public:
		TextInput();
		TextInput(Node& parent, const NodeTransform& transform);
		TextInput(Node& parent, const NodeTransform& transform, const TextStyle& textStyle, u8String emptyText = "Enter text", u8String text = "");
		~TextInput();		

		void SetText(u8String text);
		void SetEmptyText(u8String emptyText);
		void SetEmptyTextColor(ColorRGBAf color);

		void SetValueEnteredCallback(std::function<void(u8StringView)> callback);

		inline u8StringView GetText() const { return enteredText; }
		inline u8StringView GetEmptyText() const { return emptyText; }
		inline ColorRGBAf SetEmptyTextColor() const { return emptyTextColor; }
	private:
		std::function<void(u8StringView)> callback;

		u8String enteredText;

		u8String emptyText;
		ColorRGBAf emptyTextColor;

		void PressableFlagChanged() override;
		
		bool IsInsertable(UnicodeChar ch) const override;
		void TextEntered() override;
		void FilterRenderData(u8String& text, TextStyle& style) override;
		
		void SelectedStateChanged(const InputNode::SelectedStateChangedEvent& event);
		EVENT_MEMBER_FUNCTION(TextInput, SelectedStateChanged, selectedStateChangedEventDispatcher);
	};
}