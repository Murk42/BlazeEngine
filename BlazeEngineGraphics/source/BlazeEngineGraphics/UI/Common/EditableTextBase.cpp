#include "pch.h"
#include "BlazeEngineGraphics/UI/Common/EditableTextBase.h"
#include "BlazeEngineGraphics/UI/Common/TextSelection.h"
#include "BlazeEngineGraphics/UI/Common/TextCursor.h"
#include "BlazeEngineCore/Debug/Logger.h"
#include "BlazeEngine/Input/Input.h"
#include "BlazeEngineGraphics/UI/Input/InputManager.h"

namespace Blaze::UI
{
	EditableTextBase::EditableTextBase(
		TextRenderUnitBase& textRenderUnit, 
		TextSelectionRenderUnitBase& textSelectionRenderUnit, 
		TextCursorRenderUnitBase& textCursorRenderUnit
	) :
		SelectableTextBase(textRenderUnit, textSelectionRenderUnit), textCursorRenderUnit(textCursorRenderUnit),
		multilineInput(true), textChanged(false)
	{				
		selectedStateChangedEventDispatcher.AddHandler<&EditableTextBase::SelectedStateChangedEvent>(*this);
		keyDownEventDispatcher.AddHandler < &EditableTextBase::KeyDownEvent>(*this);
		textInputEventDispatcher.AddHandler < &EditableTextBase::TextInputEvent>(*this);		
	}
	EditableTextBase::~EditableTextBase()
	{		
		selectedStateChangedEventDispatcher.RemoveHandler<&EditableTextBase::SelectedStateChangedEvent>(*this);
		keyDownEventDispatcher.RemoveHandler < &EditableTextBase::KeyDownEvent>(*this);
		textInputEventDispatcher.RemoveHandler < &EditableTextBase::TextInputEvent>(*this);		
	}
	void EditableTextBase::SetMultilineInput(bool multiline)
	{
		this->multilineInput = multiline;
	}	
	void EditableTextBase::SelectedStateChangedEvent(const InputNode::SelectedStateChangedEvent& event)
	{
		if (IsSelected())
		{
			textCursorRenderUnit.cursor.SetCursorPos(textSelectionRenderUnit.selection.textContainer.CharacterCount());

			textCursorRenderUnit.ShowCursor();
		}
		else
		{
			if (textChanged)
				textEnteredEventDispatcher.Call({ *this, textSelectionRenderUnit.selection.textContainer });

			textCursorRenderUnit.HideCursor();
		}
	}
	void EditableTextBase::KeyDownEvent(const InputNode::KeyDownEvent& event)
	{
		if (!IsPressable())
			return;

		switch (event.key)
		{		
		case Keyboard::Key::ENTER: {
			if (multilineInput)
				ReplaceText(textSelectionRenderUnit.selection.GetBeginning(), textSelectionRenderUnit.selection.GetSize(), "\n");
			else
				Unselect();
			break;
		}
		case Keyboard::Key::BACKSPACE: {
			if (textSelectionRenderUnit.selection.GetBeginning() != 0)
				EraseText(textSelectionRenderUnit.selection.GetBeginning() - 1, std::max<uintMem>(textSelectionRenderUnit.selection.GetSize(), 1));
			break;
		}
		case Keyboard::Key::DELETE: {
			if (textSelectionRenderUnit.selection.GetBeginning() != textSelectionRenderUnit.selection.textContainer.CharacterCount())				
				EraseText(textSelectionRenderUnit.selection.GetBeginning(), std::max<uintMem>(textSelectionRenderUnit.selection.GetSize(), 1));
			break;
		}
		case Keyboard::Key::LEFT: {
			if (bool(event.modifier & Keyboard::KeyModifier::SHIFT) && !textSelectionRenderUnit.selection.Empty())
				break;

			const uintMem pos = textCursorRenderUnit.cursor.GetCursorPos();			

			if (bool(event.modifier & Keyboard::KeyModifier::SHIFT))
			{
				if (pos != 0)
					textSelectionRenderUnit.selection.SetTail(pos - 1);
			}
			else if (!textSelectionRenderUnit.selection.Empty())
				textSelectionRenderUnit.selection.Set(pos, pos);
			else
			{
				if (pos != 0)
					textCursorRenderUnit.cursor.SetCursorPos(pos - 1);
			}
			break;
		}
		case Keyboard::Key::RIGHT: {
			if (bool(event.modifier & Keyboard::KeyModifier::SHIFT) && !textSelectionRenderUnit.selection.Empty())
				break;

			const uintMem pos = textCursorRenderUnit.cursor.GetCursorPos();
			
			if (bool(event.modifier & Keyboard::KeyModifier::SHIFT))
			{
				if (pos != textSelectionRenderUnit.selection.textContainer.CharacterCount())
					textSelectionRenderUnit.selection.SetTail(pos + 1);
			}
			else if (!textSelectionRenderUnit.selection.Empty())
				textSelectionRenderUnit.selection.Set(pos, pos);
			else
			{
				if (pos != textSelectionRenderUnit.selection.textContainer.CharacterCount())
					textCursorRenderUnit.cursor.SetCursorPos(pos + 1);
			}
			break;
		}
		case Keyboard::Key::UP: {
			if (bool(event.modifier & Keyboard::KeyModifier::SHIFT) && !textSelectionRenderUnit.selection.Empty())
				break;

			const uintMem pos = textCursorRenderUnit.cursor.GetCursorPos();

			if (pos == 0)
				break;

			const uintMem lineIndex = textRenderUnit.GetCharacterData()[pos].lineIndex;

			if (lineIndex == 0)
				break;

			const Vec2f position = textRenderUnit.GetCharacterSeparationPosition(pos);
			const uintMem characterIndex = textRenderUnit.GetClosestCharacterSeparationIndexInLine(position, lineIndex - 1);
			if (bool(event.modifier & Keyboard::KeyModifier::SHIFT))
				textSelectionRenderUnit.selection.SetTail(characterIndex);
			else
				textCursorRenderUnit.cursor.SetCursorPos(characterIndex);

			break;
		}
		case Keyboard::Key::DOWN: {
			if (bool(event.modifier & Keyboard::KeyModifier::SHIFT) && !textSelectionRenderUnit.selection.Empty())
				break;

			const uintMem pos = textCursorRenderUnit.cursor.GetCursorPos();

			if (pos >= textCursorRenderUnit.cursor.selection.textContainer.CharacterCount() - 1)
				break;

			const uintMem lineIndex = textRenderUnit.GetCharacterData()[pos].lineIndex;

			if (lineIndex == textRenderUnit.GetLineData().Count() - 1)
				break;

			const Vec2f position = textRenderUnit.GetCharacterSeparationPosition(pos);
			const uintMem characterIndex = textRenderUnit.GetClosestCharacterSeparationIndexInLine(position, lineIndex + 1);
			if (bool(event.modifier & Keyboard::KeyModifier::SHIFT))
				textSelectionRenderUnit.selection.SetTail(characterIndex);
			else
				textCursorRenderUnit.cursor.SetCursorPos(characterIndex);

			break;
		}
		case Keyboard::Key::HOME: {
			if (bool(event.modifier & Keyboard::KeyModifier::SHIFT) && !textSelectionRenderUnit.selection.Empty())
				break;

			const uintMem pos = textCursorRenderUnit.cursor.GetCursorPos();
			const auto& line = textRenderUnit.GetLineData()[textRenderUnit.GetCharacterData()[pos].lineIndex];
			if (bool(event.modifier & Keyboard::KeyModifier::SHIFT))
				textSelectionRenderUnit.selection.SetTail(line.firstCharacterIndex);
			else
				textCursorRenderUnit.cursor.SetCursorPos(line.firstCharacterIndex);

			break;
		}
		case Keyboard::Key::END: {
			if (bool(event.modifier & Keyboard::KeyModifier::SHIFT) && !textSelectionRenderUnit.selection.Empty())
				break;

			const uintMem pos = textCursorRenderUnit.cursor.GetCursorPos();
			const auto& line = textRenderUnit.GetLineData()[textRenderUnit.GetCharacterData()[pos].lineIndex];
			if (bool(event.modifier & Keyboard::KeyModifier::SHIFT))
				textSelectionRenderUnit.selection.SetTail(line.firstCharacterIndex + line.characterCount);
			else
				textCursorRenderUnit.cursor.SetCursorPos(line.firstCharacterIndex + line.characterCount);

			break;
		}
		case Keyboard::Key::V: {
			if (bool(event.modifier & Keyboard::KeyModifier::CTRL))			
				ReplaceText(textSelectionRenderUnit.selection.GetBeginning(), textSelectionRenderUnit.selection.GetSize(), Input::GetClipboardText());
			break;
		}
		}
	}
	void EditableTextBase::TextInputEvent(const InputNode::TextInputEvent& event)
	{
		if (!IsPressable())
			return;

		ReplaceText(textSelectionRenderUnit.selection.GetBeginning(), textSelectionRenderUnit.selection.GetSize(), event.string);
	}	
	void EditableTextBase::ReplaceText(uintMem index, uintMem count, StringViewUTF8 string)
	{
		textSelectionRenderUnit.selection.textContainer.EraseSubString(index, count);
		textSelectionRenderUnit.selection.textContainer.Insert(index, string);

		textChanged = true;
	}
	void EditableTextBase::EraseText(uintMem index, uintMem count)
	{
		textSelectionRenderUnit.selection.textContainer.EraseSubString(index, count);

		textChanged = true;
	}	
}