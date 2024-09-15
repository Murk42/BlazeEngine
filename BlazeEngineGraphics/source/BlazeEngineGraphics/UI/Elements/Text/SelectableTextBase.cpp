#include "pch.h"
#include "BlazeEngineGraphics/UI/Elements/Text/SelectableTextBase.h"
#include "BlazeEngineGraphics/UI/Input/UIInputManager.h"
#include "BlazeEngine/Input/Input.h"

namespace Blaze::UI
{	
	SelectableTextBase::SelectableTextBase(
		UIGraphics::TextRenderUnitBase& textRenderUnit,
		UIGraphics::TextSelectionRenderUnitBase& textSelectionRenderUnit,
		UIGraphics::TextCursorRenderUnitBase& textCursorRenderUnit
		) :
		textRenderUnit(textRenderUnit), textSelectionRenderUnit(textSelectionRenderUnit), textCursorRenderUnit(textCursorRenderUnit),
		grabbedSelectionBegin(0), grabbedSelectionEnd(0), mouseDown(false)
	{
	}
	bool SelectableTextBase::HitTest(Vec2f screenPosition)
	{
		if (!UIInputNode::HitTest(screenPosition))
			return false;

		Vec2f localSpacePos = textRenderUnit.GetFinalTransform().TransformFromFinalToLocalTransformSpace(screenPosition);
		uintMem lineIndex;

		if (!textRenderUnit.GetLineIndexUnderPosition(localSpacePos, lineIndex))
			return false;
		return true;
	}
	void SelectableTextBase::OnEvent(DeselectedEvent event)
	{
		textSelectionRenderUnit.ClearSelection();
	}
	void SelectableTextBase::OnEvent(KeyPressedEvent event)
	{
		switch (event.key)
		{
		case Key::Escape: {
			event.inputManager->SelectNode(nullptr);
			break;
		}				
		case Key::Left: {

			if (
				!textSelectionRenderUnit.IsSelectionEmpty() && !mouseDown &&
				(GetScreen()->GetWindow()->GetLastKeyState(Key::LShift).down || GetScreen()->GetWindow()->GetLastKeyState(Key::RShift).down)
				)
			{
				if (textCursorRenderUnit.IsCursorAtBeggining())
					break;

				textCursorRenderUnit.RetreatCursor();

				uintMem cursorNextCharacterIndex = textCursorRenderUnit.GetIndexOfCharacterAfterCursor();
				if (textSelectionRenderUnit.GetSelectionBegin() == cursorNextCharacterIndex + 1)
					textSelectionRenderUnit.SetSelectionBegin(cursorNextCharacterIndex);
				else if (textSelectionRenderUnit.GetSelectionEnd() == cursorNextCharacterIndex + 1)
					textSelectionRenderUnit.SetSelectionEnd(cursorNextCharacterIndex);
				else
					Debug::Logger::LogWarning("Blaze Engine Warning", "Cursor is inside selection. An internal error.");
			}
			break;
		}
		case Key::Right: {
			if (
				!textSelectionRenderUnit.IsSelectionEmpty() && !mouseDown &&
				(GetScreen()->GetWindow()->GetLastKeyState(Key::LShift).down || GetScreen()->GetWindow()->GetLastKeyState(Key::RShift).down)
				)
			{
				if (textCursorRenderUnit.IsCursorAtEnd())
					break;

				textCursorRenderUnit.AdvanceCursor();

				uint cursorNextCharacterIndex = textCursorRenderUnit.GetIndexOfCharacterAfterCursor();
				if (textSelectionRenderUnit.GetSelectionBegin() == cursorNextCharacterIndex - 1)
					textSelectionRenderUnit.SetSelectionBegin(cursorNextCharacterIndex);
				else if (textSelectionRenderUnit.GetSelectionEnd() == cursorNextCharacterIndex - 1)
					textSelectionRenderUnit.SetSelectionEnd(cursorNextCharacterIndex);
				else
					Debug::Logger::LogWarning("Blaze Engine Warning", "Cursor is inside selection. An internal error.");
			}
	
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

			if (
				!textSelectionRenderUnit.IsSelectionEmpty() && !mouseDown &&
				GetScreen()->GetWindow()->GetLastKeyState(Key::LShift).down || GetScreen()->GetWindow()->GetLastKeyState(Key::RShift).down
				)
			{
				uintMem indexOfCharacterAfterCursor = textCursorRenderUnit.GetIndexOfCharacterAfterCursor();

				if (textSelectionRenderUnit.GetSelectionBegin() == indexOfCharacterAfterCursor)
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

			if (
				!textSelectionRenderUnit.IsSelectionEmpty() && !mouseDown &&
				GetScreen()->GetWindow()->GetLastKeyState(Key::LShift).down || GetScreen()->GetWindow()->GetLastKeyState(Key::RShift).down
				)
			{
				uintMem indexOfCharacterAfterCursor = textCursorRenderUnit.GetIndexOfCharacterAfterCursor();

				if (textSelectionRenderUnit.GetSelectionBegin() == indexOfCharacterAfterCursor)
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
			break;
		}
		case Key::Home: {
			if (
				!textSelectionRenderUnit.IsSelectionEmpty() && !mouseDown &&
				GetScreen()->GetWindow()->GetLastKeyState(Key::LShift).down || GetScreen()->GetWindow()->GetLastKeyState(Key::RShift).down
				)
			{
				uintMem indexOfCharacterAfterCursor = textCursorRenderUnit.GetIndexOfCharacterAfterCursor();
				auto& characterData = textRenderUnit.GetCharacterData();
				auto& line = textRenderUnit.GetLineData()[characterData[indexOfCharacterAfterCursor].lineIndex];

				if (textSelectionRenderUnit.GetSelectionEnd() == indexOfCharacterAfterCursor)
					textSelectionRenderUnit.SetSelectionEnd(textSelectionRenderUnit.GetSelectionBegin());

				textSelectionRenderUnit.SetSelectionBegin(line.firstCharacterIndex);
				textCursorRenderUnit.SetCursorPosBeforeCharacter(textSelectionRenderUnit.GetSelectionBegin());
			}

			break;
		}
		case Key::End: {
			if (
				!textSelectionRenderUnit.IsSelectionEmpty() && !mouseDown &&
				GetScreen()->GetWindow()->GetLastKeyState(Key::LShift).down || GetScreen()->GetWindow()->GetLastKeyState(Key::RShift).down
				)
			{
				uintMem indexOfCharacterAfterCursor = textCursorRenderUnit.GetIndexOfCharacterAfterCursor();
				auto& characterData = textRenderUnit.GetCharacterData();
				auto& line = textRenderUnit.GetLineData()[characterData[indexOfCharacterAfterCursor].lineIndex];

				if (textSelectionRenderUnit.GetSelectionBegin() == indexOfCharacterAfterCursor)
					textSelectionRenderUnit.SetSelectionBegin(textSelectionRenderUnit.GetSelectionEnd());

				textSelectionRenderUnit.SetSelectionEnd(line.firstCharacterIndex + line.characterCount);

				textCursorRenderUnit.SetCursorPosBeforeCharacter(textSelectionRenderUnit.GetSelectionEnd());
			}
			break;
		}
		case Key::C: {
			if ((GetScreen()->GetWindow()->GetLastKeyState(Key::LCtrl).down || GetScreen()->GetWindow()->GetLastKeyState(Key::RCtrl).down) && !textSelectionRenderUnit.IsSelectionEmpty())
				Input::SetClipboardText(GetTextSubString(textSelectionRenderUnit.GetSelectionBegin(), textSelectionRenderUnit.GetSelectionEnd()));
			break;
		}		
		}
	}	
	void SelectableTextBase::OnEvent(MousePressedEvent event)
	{
		Vec2f localSpacePos = textRenderUnit.GetFinalTransform().TransformFromFinalToLocalTransformSpace(event.pos);
		uintMem lineIndex;

		if (!textRenderUnit.GetLineIndexUnderPosition(localSpacePos, lineIndex))
			return;
		
		event.inputManager->SelectNode(this);

		if (event.combo == 3)
		{
			if (!textRenderUnit.GetCharacterData().Empty())
			{								
				uintMem characterIndex = textRenderUnit.GetClosestCharacterIndexInLine(localSpacePos, lineIndex);

				textRenderUnit.FindLine(characterIndex, grabbedSelectionBegin, grabbedSelectionEnd);

				textCursorRenderUnit.SetCursorPosBeforeCharacter(grabbedSelectionEnd);
			}
		}
		else if (event.combo == 2)
		{					
			uintMem characterIndex = textRenderUnit.GetClosestCharacterIndexInLine(localSpacePos, lineIndex);

			textRenderUnit.FindWord(characterIndex, grabbedSelectionBegin, grabbedSelectionEnd);
			textCursorRenderUnit.SetCursorPosBeforeCharacter(grabbedSelectionEnd);
		}
		else if (event.combo == 1)
		{						
			uintMem characterIndex = textRenderUnit.GetClosestCharacterSeparationIndex(localSpacePos);
			textCursorRenderUnit.SetCursorPosBeforeCharacter(characterIndex);

			grabbedSelectionEnd = characterIndex;
			grabbedSelectionBegin = grabbedSelectionEnd;
		}

		textSelectionRenderUnit.SetSelection(grabbedSelectionBegin, grabbedSelectionEnd);

		mouseDown = true;
	}
	void SelectableTextBase::OnEvent(MouseMotionEvent event)
	{
		Vec2f localSpacePos = textRenderUnit.GetFinalTransform().TransformFromFinalToLocalTransformSpace(event.pos);
		uintMem lineIndex;

		textRenderUnit.GetLineIndexUnderPosition(localSpacePos, lineIndex);			

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
	void SelectableTextBase::OnEvent(MouseReleasedEvent event)
	{
		mouseDown = false;
	}
	void SelectableTextBase::OnEvent(MouseEnterEvent event)
	{		
		Input::SetCursorType(Input::CursorType::IBeam);
	}
	void SelectableTextBase::OnEvent(MouseExitEvent event)
	{		
		Input::SetCursorType(Input::CursorType::Arrow);
	}	
	void SelectableTextBase::SelectAroundGrabbedSelection(uintMem characterIndex)
	{
		uintMem begin = std::min(characterIndex, grabbedSelectionBegin);
		uintMem end = std::max(characterIndex, grabbedSelectionEnd);

		textSelectionRenderUnit.SetSelection(begin, end);
	}	
}