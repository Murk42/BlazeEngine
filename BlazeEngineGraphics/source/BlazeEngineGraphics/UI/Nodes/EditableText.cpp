#include "pch.h"
#include "BlazeEngineGraphics/UI/Nodes/EditableText.h"
#include "BlazeEngineGraphics/UI/Input/InputManager.h"
#include "BlazeEngine/Input/Input.h"

namespace Blaze::UI::Nodes
{			
	EditableText::EditableText() :
		EditableTextBase(textRenderUnit, textSelectionRenderUnit, textCursorRenderUnit),
		textRenderUnit(textContainerReference, this), cursor(selection), selection(textContainer), textSelectionRenderUnit(selection, textRenderUnit), textCursorRenderUnit(cursor, textRenderUnit),
		emptyColor(0x555555ff), selectedColor(0xf5f5f5ff), unselectedColor(0xd0d0d0ff), emptyTextContainer("Enter text"), textContainerReference(emptyTextContainer)
	{
		textRenderUnit.SetTextColor(emptyColor);

		mouseHitStatusChangedEventDispatcher.AddHandler<&EditableText::MouseHitStatusChangedEvent>(*this);
		pressableFlagChangedEventDispatcher.AddHandler<&EditableText::PressableFlagChangedEvent>(*this);
		selectedStateChangedEventDispatcher.AddHandler<&EditableText::SelectedStateChangedEvent>(*this);
	}
	EditableText::~EditableText()
	{
		mouseHitStatusChangedEventDispatcher.RemoveHandler<&EditableText::MouseHitStatusChangedEvent>(*this);
		pressableFlagChangedEventDispatcher.RemoveHandler<&EditableText::PressableFlagChangedEvent>(*this);
		selectedStateChangedEventDispatcher.RemoveHandler<&EditableText::SelectedStateChangedEvent>(*this);
	}
	void EditableText::SetText(StringViewUTF8 newText)
	{
		auto old = textContainer.SwapStrings(newText);		
		
		UpdateDisplayedText();
	}
	void EditableText::SetEmptyText(StringViewUTF8 newEmptyText)
	{
		emptyTextContainer.SetString(newEmptyText);
	}	
	Graphics::RenderUnit* EditableText::GetRenderUnit(uint index)
	{		
		switch (index)
		{		
		case 0: return &textRenderUnit;
		case 1: return &textSelectionRenderUnit;
		case 2: return &textCursorRenderUnit;
		default: return nullptr;
		}
	}
	void EditableText::MouseHitStatusChangedEvent(const InputNode::MouseHitStatusChangedEvent& event)
	{
		UpdateCursorType();
	}
	void EditableText::PressableFlagChangedEvent(const ButtonBase::PressableFlagChangedEvent& event)
	{
		UpdateCursorType();
	}
	void EditableText::SelectedStateChangedEvent(const InputNode::SelectedStateChangedEvent& event)
	{
		UpdateDisplayedText();
	}
	void EditableText::UpdateCursorType()
	{
		if (GetMouseHitStatus() == 0)
			Input::SetCursorType(Input::CursorType::Default);
		else if (IsPressable())
			Input::SetCursorType(Input::CursorType::Text);
		else
			Input::SetCursorType(Input::CursorType::NotAllowed);
	}
	void EditableText::UpdateDisplayedText()
	{
		if (IsSelected())
		{
			textContainerReference.SetContainer(textContainer);
			textRenderUnit.SetTextColor(selectedColor);
		}
		else
		{
			if (textContainer.CharacterCount() == 0)
			{
				textContainerReference.SetContainer(emptyTextContainer);
				textRenderUnit.SetTextColor(emptyColor);
			}
			else
				textRenderUnit.SetTextColor(unselectedColor);
		}
	}
}