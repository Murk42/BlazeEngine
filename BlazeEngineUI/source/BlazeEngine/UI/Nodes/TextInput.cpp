#include "pch.h"
#include "BlazeEngine/UI/Nodes/TextInput.h"

namespace Blaze::UI::Nodes
{
	TextInput::TextInput()
	{
	}
	TextInput::TextInput(Node& parent, const NodeTransform& transform)
		: TextInputBase(parent, transform), emptyTextColor(0x404040ff_rgba)
	{		
	}
	TextInput::TextInput(Node& parent, const NodeTransform& transform, const TextStyle& textStyle, u8String emptyText, u8String text)
		: TextInputBase(parent, transform, textStyle), emptyTextColor(0x404040ff_rgba)
	{
		SetEmptyText(emptyText);
		SetText(text);
	}
	TextInput::~TextInput()
	{
	}
	void TextInput::SetText(u8String text)
	{		
		SetInputedText(text);

		TextEntered();
	}
	void TextInput::SetEmptyText(u8String emptyText)
	{
		this->emptyText = emptyText;
		InvalidateRenderedText();
	}
	void TextInput::SetEmptyTextColor(ColorRGBAf color)
	{
		this->emptyTextColor = color;
		InvalidateRenderedText();
	}
	void TextInput::SetValueEnteredCallback(std::function<void(u8StringView)> callback)
	{
		this->callback = callback;
	}
	void TextInput::PressableFlagChanged()
	{
		TextInputBase::PressableFlagChanged();
		InvalidateRenderedText();
	}
	bool TextInput::IsInsertable(UnicodeChar ch) const
	{
		return ch.Value() != U'\n';
	}
	void TextInput::TextEntered()
	{
		enteredText = GetInputedText();	

		if (callback)
			callback(enteredText);

		InvalidateRenderedText();
	}
	void TextInput::FilterRenderData(u8String& text, TextStyle& style)
	{	
		if (IsPressable())
		{
			if (GetInputedText().Empty() && !IsSelected())
			{
				style.color = emptyTextColor;
				text = emptyText;
			}
		}
		else
		{
			style.color = 0x606060ff_rgba;

			if (GetInputedText().Empty())
				text = emptyText;
		}
	}
	void TextInput::SelectedStateChanged(const InputNode::SelectedStateChangedEvent& event)
	{
		InvalidateRenderedText();
	}
}