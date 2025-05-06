#include "pch.h"
#include "BlazeEngineGraphics/UI/Common/EditableTextBase.h"
#include "BlazeEngineGraphics/UI/Input/InputManager.h"
#include "BlazeEngine/Input/Input.h"

namespace Blaze::UI
{
	EditableTextBase::EditableTextBase(
		TextRenderUnitBase& textRenderUnit, 
		TextSelectionRenderUnitBase& textSelectionRenderUnit, 
		TextCursorRenderUnitBase& textCursorRenderUnit
	) :
		textRenderUnit(textRenderUnit), textSelectionRenderUnit(textSelectionRenderUnit), textCursorRenderUnit(textCursorRenderUnit),
		grabbedSelectionBegin(0), grabbedSelectionEnd(0), mouseDown(false), multilineInput(true)
	{		
	}
	void EditableTextBase::SetMultilineInput(bool multiline)
	{
		this->multilineInput = multiline;
	}		
	void EditableTextBase::SelectAroundGrabbedSelection(uintMem characterIndex)
	{
		uintMem begin = std::min(characterIndex, grabbedSelectionBegin);
		uintMem end = std::max(characterIndex, grabbedSelectionEnd);

		textSelectionRenderUnit.SetSelection(begin, end);
	}
	void EditableTextBase::InsertAtCursor(StringViewUTF8 string)
	{
		if (!textSelectionRenderUnit.IsSelectionEmpty())
		{
			EraseTextSubString(textSelectionRenderUnit.GetSelectionBegin(), textSelectionRenderUnit.GetSelectionEnd());
			textCursorRenderUnit.SetCursorPosBeforeCharacter(textSelectionRenderUnit.GetSelectionBegin());

			textSelectionRenderUnit.ClearSelection();
		}

		if (textCursorRenderUnit.IsCursorAtEnd()) 
			InsertStringIntoText(textRenderUnit.GetCharacterData().Count() - 1, string);
		else
			InsertStringIntoText(textCursorRenderUnit.GetIndexOfCharacterAfterCursor(), string);

		for (uintMem i = 0; i < string.CharacterCount(); ++i)
			textCursorRenderUnit.AdvanceCursor();

		grabbedSelectionBegin = grabbedSelectionEnd = textCursorRenderUnit.GetIndexOfCharacterAfterCursor();
	}
	void EditableTextBase::OnEvent(const SelectedEvent& event)
	{
		textCursorRenderUnit.SetCursorPosToEnd();
	}
	void EditableTextBase::OnEvent(const DeselectedEvent& event)
	{
		textSelectionRenderUnit.ClearSelection();
	}
	void EditableTextBase::OnEvent(const KeyDownEvent& event)
	{
		switch (event.key)
		{
		case Keyboard::Key::ESCAPE: {
			event.inputManager.SelectNode(nullptr);
			break;
		}
		case Keyboard::Key::ENTER: {
			if (multilineInput)
				InsertAtCursor("\n");
			else
				event.inputManager.SelectNode(nullptr);
			break;
		}
		case Keyboard::Key::BACKSPACE: {
			if (textSelectionRenderUnit.IsSelectionEmpty())
			{
				if (textCursorRenderUnit.IsCursorAtBeggining())
					break;

				textCursorRenderUnit.RetreatCursor();
				grabbedSelectionBegin = grabbedSelectionEnd = textCursorRenderUnit.GetIndexOfCharacterAfterCursor();

				EraseTextSubString(textCursorRenderUnit.GetIndexOfCharacterAfterCursor(), 1);
			}
			else
			{
				textCursorRenderUnit.SetCursorPosBeforeCharacter(textSelectionRenderUnit.GetSelectionBegin());
				grabbedSelectionBegin = grabbedSelectionEnd = textCursorRenderUnit.GetIndexOfCharacterAfterCursor();

				uint selectionLength = textSelectionRenderUnit.GetSelectionEnd() - textSelectionRenderUnit.GetSelectionBegin();
				EraseTextSubString(textSelectionRenderUnit.GetSelectionBegin(), selectionLength);

				textSelectionRenderUnit.ClearSelection();
			}
			break;
		}
		case Keyboard::Key::DELETE: {
			if (textSelectionRenderUnit.IsSelectionEmpty())
			{
				if (textCursorRenderUnit.IsCursorAtEnd())
					break;

				EraseTextSubString(textCursorRenderUnit.GetIndexOfCharacterAfterCursor(), 1);
			}
			else
			{
				EraseTextSubString(textSelectionRenderUnit.GetSelectionBegin(), textSelectionRenderUnit.GetSelectionEnd() - textSelectionRenderUnit.GetSelectionBegin());

				textSelectionRenderUnit.ClearSelection();
			}
			break;
		}
		case Keyboard::Key::LEFT: {

			if (bool(event.modifier & Keyboard::KeyModifier::SHIFT))
			{
				if (textCursorRenderUnit.IsCursorAtBeggining())
					break;

				textCursorRenderUnit.RetreatCursor();

				uintMem cursorNextCharacterIndex = textCursorRenderUnit.GetIndexOfCharacterAfterCursor();
				if (textSelectionRenderUnit.IsSelectionEmpty())
					textSelectionRenderUnit.SetSelection(cursorNextCharacterIndex, cursorNextCharacterIndex + 1);
				else if (textSelectionRenderUnit.GetSelectionBegin() == cursorNextCharacterIndex + 1)
					textSelectionRenderUnit.SetSelectionBegin(cursorNextCharacterIndex);
				else if (textSelectionRenderUnit.GetSelectionEnd() == cursorNextCharacterIndex + 1)
					textSelectionRenderUnit.SetSelectionEnd(cursorNextCharacterIndex);
				else
					Debug::Logger::LogWarning("Blaze Engine Warning", "Cursor is inside selection. An internal error.");
			}
			else
			{
				if (!textSelectionRenderUnit.IsSelectionEmpty())
					textCursorRenderUnit.SetCursorPosBeforeCharacter(textSelectionRenderUnit.GetSelectionBegin());
				else if (!textCursorRenderUnit.IsCursorAtBeggining())
					textCursorRenderUnit.RetreatCursor();

				grabbedSelectionBegin = grabbedSelectionEnd = textCursorRenderUnit.GetIndexOfCharacterAfterCursor();

				textSelectionRenderUnit.ClearSelection();
			}

			if (mouseDown && !textSelectionRenderUnit.IsSelectionEmpty())
				mouseDown = false;
			break;
		}
		case Keyboard::Key::RIGHT: {
			if (bool(event.modifier & Keyboard::KeyModifier::SHIFT))
			{
				if (textCursorRenderUnit.IsCursorAtEnd())
					break;

				textCursorRenderUnit.AdvanceCursor();

				uint cursorNextCharacterIndex = textCursorRenderUnit.GetIndexOfCharacterAfterCursor();
				if (textSelectionRenderUnit.IsSelectionEmpty())
					textSelectionRenderUnit.SetSelection(cursorNextCharacterIndex - 1, cursorNextCharacterIndex);
				else if (textSelectionRenderUnit.GetSelectionBegin() == cursorNextCharacterIndex - 1)
					textSelectionRenderUnit.SetSelectionBegin(cursorNextCharacterIndex);
				else if (textSelectionRenderUnit.GetSelectionEnd() == cursorNextCharacterIndex - 1)
					textSelectionRenderUnit.SetSelectionEnd(cursorNextCharacterIndex);
				else
					Debug::Logger::LogWarning("Blaze Engine Warning", "Cursor is inside selection. An internal error.");
			}
			else
			{
				if (!textSelectionRenderUnit.IsSelectionEmpty())
					textCursorRenderUnit.SetCursorPosBeforeCharacter(textSelectionRenderUnit.GetSelectionEnd());
				else if (!textCursorRenderUnit.IsCursorAtEnd())
					textCursorRenderUnit.AdvanceCursor();

				grabbedSelectionBegin = grabbedSelectionEnd = textCursorRenderUnit.GetIndexOfCharacterAfterCursor();

				textSelectionRenderUnit.ClearSelection();
			}

			if (mouseDown && !textSelectionRenderUnit.IsSelectionEmpty())
				mouseDown = false;
			break;
		}
		case Keyboard::Key::UP: {
			auto& characterData = textRenderUnit.GetCharacterData();
			uintMem characterIndex = textCursorRenderUnit.GetIndexOfCharacterAfterCursor();
			uintMem lineIndex = characterData[characterIndex].lineIndex;

			if (lineIndex == 0)
				break;

			Vec2f position = textRenderUnit.GetCharacterSeparationPosition(characterIndex);		
			characterIndex = textRenderUnit.GetClosestCharacterSeparationIndexInLine(position, lineIndex - 1);

			if (bool(event.modifier & Keyboard::KeyModifier::SHIFT))
			{
				uintMem indexOfCharacterAfterCursor = textCursorRenderUnit.GetIndexOfCharacterAfterCursor();

				if (textSelectionRenderUnit.IsSelectionEmpty())				
					textSelectionRenderUnit.SetSelection(characterIndex, indexOfCharacterAfterCursor);					
				else if (textSelectionRenderUnit.GetSelectionBegin() == indexOfCharacterAfterCursor)				
					textSelectionRenderUnit.SetSelectionBegin(characterIndex);					
				else if (textSelectionRenderUnit.GetSelectionEnd() == indexOfCharacterAfterCursor)
					if (textSelectionRenderUnit.GetSelectionBegin() > characterIndex)					
						textSelectionRenderUnit.SetSelection(characterIndex, textSelectionRenderUnit.GetSelectionBegin());						
					else											
						textSelectionRenderUnit.SetSelectionEnd(characterIndex);
				else
					Debug::Logger::LogWarning("Blaze Engine Warning", "Cursor is inside selection. An internal error.");				

				textCursorRenderUnit.SetCursorPosBeforeCharacter(characterIndex);
			}
			else
			{
				textSelectionRenderUnit.ClearSelection();
				textCursorRenderUnit.SetCursorPosBeforeCharacter(characterIndex);
				grabbedSelectionBegin = grabbedSelectionEnd = textCursorRenderUnit.GetIndexOfCharacterAfterCursor();
			}


			break;
		}
		case Keyboard::Key::DOWN: {
			auto& characterData = textRenderUnit.GetCharacterData();
			uintMem characterIndex = textCursorRenderUnit.GetIndexOfCharacterAfterCursor();
			uintMem lineIndex = characterData[characterIndex].lineIndex;

			if (lineIndex == textRenderUnit.GetLineData().Count() - 1)
				break;

			Vec2f position = textRenderUnit.GetCharacterSeparationPosition(characterIndex);
			characterIndex = textRenderUnit.GetClosestCharacterSeparationIndexInLine(position, lineIndex + 1);

			if (bool(event.modifier & Keyboard::KeyModifier::SHIFT))
			{
				uintMem indexOfCharacterAfterCursor = textCursorRenderUnit.GetIndexOfCharacterAfterCursor();

				if (textSelectionRenderUnit.IsSelectionEmpty())
					textSelectionRenderUnit.SetSelection(indexOfCharacterAfterCursor, characterIndex);
				else if (textSelectionRenderUnit.GetSelectionBegin() == indexOfCharacterAfterCursor)
					if (textSelectionRenderUnit.GetSelectionEnd() < characterIndex)
						textSelectionRenderUnit.SetSelection(textSelectionRenderUnit.GetSelectionEnd(), characterIndex);
					else
						textSelectionRenderUnit.SetSelectionBegin(characterIndex);

				else if (textSelectionRenderUnit.GetSelectionEnd() == indexOfCharacterAfterCursor)
					textSelectionRenderUnit.SetSelectionEnd(characterIndex);
				else					
					Debug::Logger::LogWarning("Blaze Engine Warning", "Cursor is inside selection. An internal error.");				

				textCursorRenderUnit.SetCursorPosBeforeCharacter(characterIndex);
			}
			else
			{
				textSelectionRenderUnit.ClearSelection();
				textCursorRenderUnit.SetCursorPosBeforeCharacter(characterIndex);
				grabbedSelectionBegin = grabbedSelectionEnd = textCursorRenderUnit.GetIndexOfCharacterAfterCursor();
			}
			
			break;
		}
		case Keyboard::Key::HOME: {
			if (bool(event.modifier & Keyboard::KeyModifier::SHIFT))
			{
				uintMem indexOfCharacterAfterCursor = textCursorRenderUnit.GetIndexOfCharacterAfterCursor();
				auto& characterData = textRenderUnit.GetCharacterData();
				auto& line = textRenderUnit.GetLineData()[characterData[indexOfCharacterAfterCursor].lineIndex];
				
				if (textSelectionRenderUnit.IsSelectionEmpty())
					textSelectionRenderUnit.SetSelection(line.firstCharacterIndex, indexOfCharacterAfterCursor);
				else
					SelectAroundGrabbedSelection(line.firstCharacterIndex);
				//{
				//	if (textSelectionRenderUnit.GetSelectionEnd() == indexOfCharacterAfterCursor)
				//		if (textSelectionRenderUnit.GetSelectionBegin() > line.firstCharacterIndex)
				//		{
				//			textSelectionRenderUnit.SetSelectionEnd(textSelectionRenderUnit.GetSelectionBegin());
				//			textSelectionRenderUnit.GetSelectionBegin()
				//		}
				//		else
				//			textSelectionRenderUnit.SetSelectionEnd(line.firstCharacterIndex);
				//
				//	textSelectionRenderUnit.SetSelectionBegin(line.firstCharacterIndex);
				//}

				textCursorRenderUnit.SetCursorPosBeforeCharacter(line.firstCharacterIndex);
			}
			else
			{
				textSelectionRenderUnit.ClearSelection();
				textCursorRenderUnit.SetCursorPosToBeginningOfLine(textCursorRenderUnit.GetIndexOfCharacterAfterCursor());
				grabbedSelectionBegin = grabbedSelectionEnd = textCursorRenderUnit.GetIndexOfCharacterAfterCursor();
			}

			break;
		}
		case Keyboard::Key::END: {

			if (bool(event.modifier & Keyboard::KeyModifier::SHIFT))
			{
				uintMem indexOfCharacterAfterCursor = textCursorRenderUnit.GetIndexOfCharacterAfterCursor();
				auto& characterData = textRenderUnit.GetCharacterData();
				auto& line = textRenderUnit.GetLineData()[characterData[indexOfCharacterAfterCursor].lineIndex];

				if (textSelectionRenderUnit.IsSelectionEmpty())
					textSelectionRenderUnit.SetSelection(indexOfCharacterAfterCursor, line.firstCharacterIndex + line.characterCount);
				else
					SelectAroundGrabbedSelection(line.firstCharacterIndex + line.characterCount);
				//{
				//	if (textSelectionRenderUnit.GetSelectionBegin() == indexOfCharacterAfterCursor)
				//		textSelectionRenderUnit.SetSelectionBegin(textSelectionRenderUnit.GetSelectionEnd());
				//
				//	textSelectionRenderUnit.SetSelectionEnd(line.firstCharacterIndex + line.characterCount);
				//}

				textCursorRenderUnit.SetCursorPosBeforeCharacter(line.firstCharacterIndex + line.characterCount);
			}
			else
			{
				textSelectionRenderUnit.ClearSelection();				
				textCursorRenderUnit.SetCursorPosToEndOfLine(textCursorRenderUnit.GetIndexOfCharacterAfterCursor());				
				grabbedSelectionBegin = grabbedSelectionEnd = textCursorRenderUnit.GetIndexOfCharacterAfterCursor();
			}

			break;
		}
		case Keyboard::Key::C: {
			if (bool(event.modifier & Keyboard::KeyModifier::CTRL) && !textSelectionRenderUnit.IsSelectionEmpty())
				Input::SetClipboardText(GetTextSubString(textSelectionRenderUnit.GetSelectionBegin(), textSelectionRenderUnit.GetSelectionEnd()));
			break;
		}
		case Keyboard::Key::V: {
			if (bool(event.modifier & Keyboard::KeyModifier::CTRL))			
				InsertAtCursor(Input::GetClipboardText());
			break;
		}
		}
	}
	void EditableTextBase::OnEvent(const TextInputEvent& event)
	{
		InsertAtCursor(event.string);
	}
	void EditableTextBase::OnEvent(const MouseButtonDownEvent& event)
	{
		uintMem combo = (event.combo - 1) % 3 + 1;

		event.inputManager.SelectNode(this);

		switch (combo)
		{
		case 1: {
			Vec2f localSpacePos = textRenderUnit.GetFinalTransform().TransformFromFinalToLocalTransformSpace(event.screenPos);
			uintMem characterIndex = textRenderUnit.GetClosestCharacterSeparationIndex(localSpacePos);
			textCursorRenderUnit.SetCursorPosBeforeCharacter(characterIndex);

			grabbedSelectionEnd = characterIndex;
			grabbedSelectionBegin = grabbedSelectionEnd;
			break;
		}
		case 2: {
			Vec2f localSpacePos = textRenderUnit.GetFinalTransform().TransformFromFinalToLocalTransformSpace(event.screenPos);
			uintMem characterIndex = textRenderUnit.GetClosestCharacterIndex(localSpacePos);

			textRenderUnit.FindWord(characterIndex, grabbedSelectionBegin, grabbedSelectionEnd);
			textCursorRenderUnit.SetCursorPosBeforeCharacter(grabbedSelectionEnd);
			break;
		}
		case 3: {
			if (!textRenderUnit.GetCharacterData().Empty())
			{
				Vec2f localSpacePos = textRenderUnit.GetFinalTransform().TransformFromFinalToLocalTransformSpace(event.screenPos);
				uintMem characterIndex = textRenderUnit.GetClosestCharacterIndex(localSpacePos);

				textRenderUnit.FindLine(characterIndex, grabbedSelectionBegin, grabbedSelectionEnd);

				textCursorRenderUnit.SetCursorPosBeforeCharacter(grabbedSelectionEnd);
			}
			break;
		}
		}

		textSelectionRenderUnit.SetSelection(grabbedSelectionBegin, grabbedSelectionEnd);

		mouseDown = true;
	}
	void EditableTextBase::OnEvent(const MouseMotionEvent& event)
	{
		if (mouseDown)
		{
			Vec2f localSpacePos = textRenderUnit.GetFinalTransform().TransformFromFinalToLocalTransformSpace(event.screenPos);
			uintMem characterIndex = textRenderUnit.GetClosestCharacterSeparationIndex(localSpacePos);

			if (characterIndex > grabbedSelectionBegin && characterIndex < grabbedSelectionEnd)
				textCursorRenderUnit.SetCursorPosBeforeCharacter(grabbedSelectionEnd);
			else
			{
				textCursorRenderUnit.SetCursorPosBeforeCharacter(characterIndex);

				if (textCursorRenderUnit.IsCursorAtEnd())
					SelectAroundGrabbedSelection(textRenderUnit.GetCharacterData().Count());
				else
					SelectAroundGrabbedSelection(textCursorRenderUnit.GetIndexOfCharacterAfterCursor());
			}
		}
	}
	void EditableTextBase::OnEvent(const MouseButtonUpEvent& event)
	{
		mouseDown = false;
	}
	void EditableTextBase::OnEvent(const MouseEnterEvent& event)
	{
		Input::SetCursorType(Input::CursorType::IBeam);
	}
	void EditableTextBase::OnEvent(const MouseExitEvent& event)
	{
		Input::SetCursorType(Input::CursorType::Arrow);
	}
}