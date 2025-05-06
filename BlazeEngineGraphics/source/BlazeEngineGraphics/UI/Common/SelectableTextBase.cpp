#include "pch.h"
#include "BlazeEngineGraphics/UI/Common/SelectableTextBase.h"
#include "BlazeEngineGraphics/UI/Input/InputManager.h"
#include "BlazeEngine/Input/Input.h"

namespace Blaze::UI
{	
	SelectableTextBase::SelectableTextBase(
		TextRenderUnitBase& textRenderUnit,
		TextSelectionRenderUnitBase& textSelectionRenderUnit,
		TextCursorRenderUnitBase& textCursorRenderUnit
		) :
		textRenderUnit(textRenderUnit), textSelectionRenderUnit(textSelectionRenderUnit), textCursorRenderUnit(textCursorRenderUnit),
		grabbedSelectionBegin(0), grabbedSelectionEnd(0), mouseDown(false)
	{		
	}
	int SelectableTextBase::HitTest(Vec2f screenPosition)
	{
		if (!InputNode::HitTest(screenPosition))
			return false;

		Vec2f localSpacePos = textRenderUnit.GetFinalTransform().TransformFromFinalToLocalTransformSpace(screenPosition);
		uintMem lineIndex;

		if (!textRenderUnit.GetLineIndexUnderPosition(localSpacePos, lineIndex))
			return false;
		return true;
	}
	void SelectableTextBase::OnEvent(const DeselectedEvent& event)
	{
		textSelectionRenderUnit.ClearSelection();
	}
	void SelectableTextBase::OnEvent(const KeyDownEvent& event)
	{
		switch (event.key)
		{
		case Keyboard::Key::ESCAPE: {
			event.inputManager.SelectNode(nullptr);
			break;
		}				
		case Keyboard::Key::LEFT: {

			if (bool(event.modifier & Keyboard::KeyModifier::SHIFT) && !textSelectionRenderUnit.IsSelectionEmpty())
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
		case Keyboard::Key::RIGHT: {
			if (bool(event.modifier & Keyboard::KeyModifier::SHIFT) && !textSelectionRenderUnit.IsSelectionEmpty() && !mouseDown)
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
		case Keyboard::Key::UP: {
			auto& characterData = textRenderUnit.GetCharacterData();
			uintMem characterIndex = textCursorRenderUnit.GetIndexOfCharacterAfterCursor();
			uintMem lineIndex = characterData[characterIndex].lineIndex;

			if (lineIndex == 0)
				break;

			Vec2f position = textRenderUnit.GetCharacterSeparationPosition(characterIndex);
			characterIndex = textRenderUnit.GetClosestCharacterSeparationIndexInLine(position, lineIndex - 1);

			if (bool(event.modifier & Keyboard::KeyModifier::SHIFT) && !textSelectionRenderUnit.IsSelectionEmpty() && !mouseDown)
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
		case Keyboard::Key::DOWN: {
			auto& characterData = textRenderUnit.GetCharacterData();
			uintMem characterIndex = textCursorRenderUnit.GetIndexOfCharacterAfterCursor();
			uintMem lineIndex = characterData[characterIndex].lineIndex;

			if (lineIndex == textRenderUnit.GetLineData().Count() - 1)
				break;

			Vec2f position = textRenderUnit.GetCharacterSeparationPosition(characterIndex);
			characterIndex = textRenderUnit.GetClosestCharacterSeparationIndexInLine(position, lineIndex + 1);

			if (bool(event.modifier & Keyboard::KeyModifier::SHIFT) && !textSelectionRenderUnit.IsSelectionEmpty() && !mouseDown)
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
		case Keyboard::Key::HOME: {
			if (bool(event.modifier & Keyboard::KeyModifier::SHIFT) && !textSelectionRenderUnit.IsSelectionEmpty() && !mouseDown)
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
		case Keyboard::Key::END: {
			if (bool(event.modifier & Keyboard::KeyModifier::SHIFT) && !textSelectionRenderUnit.IsSelectionEmpty() && !mouseDown)				
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
		case Keyboard::Key::C: {
			if (bool(event.modifier & Keyboard::KeyModifier::CTRL) && !textSelectionRenderUnit.IsSelectionEmpty())
				Input::SetClipboardText(GetTextSubString(textSelectionRenderUnit.GetSelectionBegin(), textSelectionRenderUnit.GetSelectionEnd()));
			break;
		}		
		}
	}	
	void SelectableTextBase::OnEvent(const MouseButtonDownEvent& event)
	{
		uintMem combo = (event.combo - 1) % 3 + 1;

		Vec2f localSpacePos = textRenderUnit.GetFinalTransform().TransformFromFinalToLocalTransformSpace(event.screenPos);
		uintMem lineIndex;

		if (!textRenderUnit.GetLineIndexUnderPosition(localSpacePos, lineIndex))
			return;
		
		event.inputManager.SelectNode(this);

		switch (combo)
		{
		case 1: {
			uintMem characterIndex = textRenderUnit.GetClosestCharacterSeparationIndex(localSpacePos);
			textCursorRenderUnit.SetCursorPosBeforeCharacter(characterIndex);

			grabbedSelectionEnd = characterIndex;
			grabbedSelectionBegin = grabbedSelectionEnd;
			break;
		}
		case 2: {
			uintMem characterIndex = textRenderUnit.GetClosestCharacterIndexInLine(localSpacePos, lineIndex);

			textRenderUnit.FindWord(characterIndex, grabbedSelectionBegin, grabbedSelectionEnd);
			textCursorRenderUnit.SetCursorPosBeforeCharacter(grabbedSelectionEnd);
			break;
		}
		case 3: {
			if (!textRenderUnit.GetCharacterData().Empty())
			{
				uintMem characterIndex = textRenderUnit.GetClosestCharacterIndexInLine(localSpacePos, lineIndex);

				textRenderUnit.FindLine(characterIndex, grabbedSelectionBegin, grabbedSelectionEnd);

				textCursorRenderUnit.SetCursorPosBeforeCharacter(grabbedSelectionEnd);
			}
			break;
		}
		}						
		
		textSelectionRenderUnit.SetSelection(grabbedSelectionBegin, grabbedSelectionEnd);

		mouseDown = true;
	}
	void SelectableTextBase::OnEvent(const MouseMotionEvent& event)
	{
		Vec2f localSpacePos = textRenderUnit.GetFinalTransform().TransformFromFinalToLocalTransformSpace(event.screenPos);
		uintMem lineIndex;

		textRenderUnit.GetLineIndexUnderPosition(localSpacePos, lineIndex);			

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
	void SelectableTextBase::OnEvent(const MouseButtonUpEvent& event)
	{
		mouseDown = false;
	}
	void SelectableTextBase::OnEvent(const MouseEnterEvent& event)
	{		
		Input::SetCursorType(Input::CursorType::IBeam);
	}
	void SelectableTextBase::OnEvent(const MouseExitEvent& event)
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