#include "pch.h"
#include "BlazeEngineGraphics/UI/Common/EditableTextBase.h"
#include "BlazeEngineGraphics/UI/Input/UIInputManager.h"
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
		SetBlocksMouseEventsFlag(true);
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
	void EditableTextBase::OnEvent(SelectedEvent event)
	{
		textCursorRenderUnit.SetCursorPosToEnd();
	}
	void EditableTextBase::OnEvent(DeselectedEvent event)
	{
		textSelectionRenderUnit.ClearSelection();
	}
	void EditableTextBase::OnEvent(KeyPressedEvent event)
	{
		switch (event.key)
		{
		case Key::Escape: {
			event.inputManager->SelectNode(nullptr);
			break;
		}
		case Key::Enter: {
			if (multilineInput)
				InsertAtCursor("\n");
			else
				event.inputManager->SelectNode(nullptr);
			break;
		}
		case Key::Backspace: {
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
		case Key::Delete: {
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
		case Key::Left: {

			if (GetScreen()->GetWindow()->GetLastKeyState(Key::LShift).down || GetScreen()->GetWindow()->GetLastKeyState(Key::RShift).down)
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
		case Key::Right: {
			if (GetScreen()->GetWindow()->GetLastKeyState(Key::LShift).down || GetScreen()->GetWindow()->GetLastKeyState(Key::RShift).down)
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
		case Key::Up: {
			auto& characterData = textRenderUnit.GetCharacterData();
			uintMem characterIndex = textCursorRenderUnit.GetIndexOfCharacterAfterCursor();
			uintMem lineIndex = characterData[characterIndex].lineIndex;

			if (lineIndex == 0)
				break;

			Vec2f position = textRenderUnit.GetCharacterSeparationPosition(characterIndex);		
			characterIndex = textRenderUnit.GetClosestCharacterSeparationIndexInLine(position, lineIndex - 1);

			if (GetScreen()->GetWindow()->GetLastKeyState(Key::LShift).down || GetScreen()->GetWindow()->GetLastKeyState(Key::RShift).down)
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
		case Key::Down: {
			auto& characterData = textRenderUnit.GetCharacterData();
			uintMem characterIndex = textCursorRenderUnit.GetIndexOfCharacterAfterCursor();
			uintMem lineIndex = characterData[characterIndex].lineIndex;

			if (lineIndex == textRenderUnit.GetLineData().Count() - 1)
				break;

			Vec2f position = textRenderUnit.GetCharacterSeparationPosition(characterIndex);
			characterIndex = textRenderUnit.GetClosestCharacterSeparationIndexInLine(position, lineIndex + 1);

			if (GetScreen()->GetWindow()->GetLastKeyState(Key::LShift).down || GetScreen()->GetWindow()->GetLastKeyState(Key::RShift).down)
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
		case Key::Home: {
			if (GetScreen()->GetWindow()->GetLastKeyState(Key::LShift).down || GetScreen()->GetWindow()->GetLastKeyState(Key::RShift).down)
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
		case Key::End: {

			if (GetScreen()->GetWindow()->GetLastKeyState(Key::LShift).down || GetScreen()->GetWindow()->GetLastKeyState(Key::RShift).down)
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
		case Key::C: {
			if ((GetScreen()->GetWindow()->GetLastKeyState(Key::LCtrl).down || GetScreen()->GetWindow()->GetLastKeyState(Key::RCtrl).down) && !textSelectionRenderUnit.IsSelectionEmpty())
				Input::SetClipboardText(GetTextSubString(textSelectionRenderUnit.GetSelectionBegin(), textSelectionRenderUnit.GetSelectionEnd()));
			break;
		}
		case Key::V: {
			if (GetScreen()->GetWindow()->GetLastKeyState(Key::LCtrl).down || GetScreen()->GetWindow()->GetLastKeyState(Key::RCtrl).down)
				InsertAtCursor(Input::GetClipboardText());
			break;
		}
		}
	}
	void EditableTextBase::OnEvent(TextInputEvent event)
	{
		InsertAtCursor(event.input);
	}
	void EditableTextBase::OnEvent(MousePressedEvent event)
	{
		event.inputManager->SelectNode(this);

		if (event.combo == 3)
		{
			if (!textRenderUnit.GetCharacterData().Empty())
			{
				Vec2f localSpacePos = textRenderUnit.GetFinalTransform().TransformFromFinalToLocalTransformSpace(event.pos);
				uintMem characterIndex = textRenderUnit.GetClosestCharacterIndex(localSpacePos);

				textRenderUnit.FindLine(characterIndex, grabbedSelectionBegin, grabbedSelectionEnd);

				textCursorRenderUnit.SetCursorPosBeforeCharacter(grabbedSelectionEnd);
			}
		}
		else if (event.combo == 2)
		{
			Vec2f localSpacePos = textRenderUnit.GetFinalTransform().TransformFromFinalToLocalTransformSpace(event.pos);			
			uintMem characterIndex = textRenderUnit.GetClosestCharacterIndex(localSpacePos);

			textRenderUnit.FindWord(characterIndex, grabbedSelectionBegin, grabbedSelectionEnd);
			textCursorRenderUnit.SetCursorPosBeforeCharacter(grabbedSelectionEnd);
		}
		else if (event.combo == 1)
		{
			Vec2f localSpacePos = textRenderUnit.GetFinalTransform().TransformFromFinalToLocalTransformSpace(event.pos);
			uintMem characterIndex = textRenderUnit.GetClosestCharacterSeparationIndex(localSpacePos);			
			textCursorRenderUnit.SetCursorPosBeforeCharacter(characterIndex);

			grabbedSelectionEnd = characterIndex;
			grabbedSelectionBegin = grabbedSelectionEnd;
		}

		textSelectionRenderUnit.SetSelection(grabbedSelectionBegin, grabbedSelectionEnd);

		mouseDown = true;
	}
	void EditableTextBase::OnEvent(MouseMotionEvent event)
	{
		if (mouseDown)
		{
			Vec2f localSpacePos = textRenderUnit.GetFinalTransform().TransformFromFinalToLocalTransformSpace(event.pos);
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
	void EditableTextBase::OnEvent(MouseReleasedEvent event)
	{
		mouseDown = false;
	}
	void EditableTextBase::OnEvent(MouseEnterEvent event)
	{
		Input::SetCursorType(Input::CursorType::IBeam);
	}
	void EditableTextBase::OnEvent(MouseExitEvent event)
	{
		Input::SetCursorType(Input::CursorType::Arrow);
	}
}