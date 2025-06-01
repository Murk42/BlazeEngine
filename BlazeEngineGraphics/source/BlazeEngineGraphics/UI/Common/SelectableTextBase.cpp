#include "pch.h"
#include "BlazeEngineCore/Debug/Logger.h"
#include "BlazeEngineGraphics/UI/Common/SelectableTextBase.h"
#include "BlazeEngineGraphics/UI/Common/TextSelection.h"
#include "BlazeEngine/Input/Input.h"

namespace Blaze::UI
{	
	SelectableTextBase::SelectableTextBase(				
		TextRenderUnitBase& textRenderUnit,
		TextSelectionRenderUnitBase& textSelectionRenderUnit) :
		textRenderUnit(textRenderUnit), textSelectionRenderUnit(textSelectionRenderUnit)
	{			
		SetMouseMotionReportPolicy(MouseMotionReportPolicy::ReportWhileSelected);

		selectedStateChangedEventDispatcher.AddHandler<&SelectableTextBase::SelectedStateChangedEvent>(*this);
		keyDownEventDispatcher.AddHandler<&SelectableTextBase::KeyDownEvent>(*this);
		mouseMotionEventDispatcher.AddHandler<&SelectableTextBase::MouseMotionEvent>(*this);		
		mouseButtonDownEventDispatcher.AddHandler<&SelectableTextBase::MouseButtonDownEvent>(*this);		
	}
	SelectableTextBase::~SelectableTextBase()
	{
		selectedStateChangedEventDispatcher.RemoveHandler<&SelectableTextBase::SelectedStateChangedEvent>(*this);
		keyDownEventDispatcher.RemoveHandler<&SelectableTextBase::KeyDownEvent>(*this);
		mouseButtonDownEventDispatcher.RemoveHandler<&SelectableTextBase::MouseButtonDownEvent>(*this);
		mouseMotionEventDispatcher.RemoveHandler<&SelectableTextBase::MouseMotionEvent>(*this);				
	}
	int SelectableTextBase::HitTest(Vec2f screenPosition)
	{
		if (!InputNode::HitTest(screenPosition))
			return 0;

		Vec2f localSpacePos = textRenderUnit.GetFinalTransform().TransformFromFinalToLocalTransformSpace(screenPosition);
		uintMem lineIndex;

		if (textRenderUnit.GetLineIndexUnderPosition(localSpacePos, lineIndex))
			return 2;
		return 1;
	}
	void SelectableTextBase::SelectedStateChangedEvent(const InputNode::SelectedStateChangedEvent& event)
	{
		if (!IsSelected())
		{
			uintMem characterCount = textSelectionRenderUnit.selection.textContainer.CharacterCount();
			textSelectionRenderUnit.selection.Set(characterCount, characterCount);
		}		
	}
	void SelectableTextBase::KeyDownEvent(const InputNode::KeyDownEvent& event)
	{
		switch (event.key)
		{
		case Keyboard::Key::ESCAPE: {
			Unselect();
			break;
		}				
		case Keyboard::Key::LEFT: {
			if (textSelectionRenderUnit.selection.Empty() || !bool(event.modifier & Keyboard::KeyModifier::SHIFT))
				break;

			const uintMem tail = textSelectionRenderUnit.selection.GetTail();

			if (tail == 0)
				break;

			const uintMem newTail = tail - 1;			
			textSelectionRenderUnit.selection.SetTail(newTail);			

			break;
		}
		case Keyboard::Key::RIGHT: {
			if (textSelectionRenderUnit.selection.Empty() || !bool(event.modifier & Keyboard::KeyModifier::SHIFT))
				break;

			const uintMem tail = textSelectionRenderUnit.selection.GetTail();

			if (tail == textSelectionRenderUnit.selection.textContainer.CharacterCount())
				break;

			const uintMem newTail = tail + 1;
			textSelectionRenderUnit.selection.SetTail(newTail);			

			break;
		}
		case Keyboard::Key::UP: {
			if (textSelectionRenderUnit.selection.Empty() || !bool(event.modifier & Keyboard::KeyModifier::SHIFT))
				break;

			const uintMem tail = textSelectionRenderUnit.selection.GetTail();

			if (tail == 0)
				break;
			
			const uintMem lineIndex = textRenderUnit.GetCharacterData()[tail].lineIndex;

			if (lineIndex == 0)
				break;

			const Vec2f position = textRenderUnit.GetCharacterSeparationPosition(tail);
			const uintMem characterIndex = textRenderUnit.GetClosestCharacterSeparationIndexInLine(position, lineIndex - 1);
			textSelectionRenderUnit.selection.SetTail(characterIndex);			

			break;
		}
		case Keyboard::Key::DOWN: {
			if (textSelectionRenderUnit.selection.Empty() || !bool(event.modifier & Keyboard::KeyModifier::SHIFT))
				break;

			const uintMem tail = textSelectionRenderUnit.selection.GetTail();

			if (tail >= textSelectionRenderUnit.selection.textContainer.CharacterCount() - 1)
				break;

			const uintMem lineIndex = textRenderUnit.GetCharacterData()[tail].lineIndex;

			if (lineIndex == textRenderUnit.GetLineData().Count() - 1)
				break;

			const Vec2f position = textRenderUnit.GetCharacterSeparationPosition(tail);
			const uintMem characterIndex = textRenderUnit.GetClosestCharacterSeparationIndexInLine(position, lineIndex + 1);
			textSelectionRenderUnit.selection.SetTail(characterIndex);			

			break;
		}
		case Keyboard::Key::HOME: {
			if (textSelectionRenderUnit.selection.Empty() || !bool(event.modifier & Keyboard::KeyModifier::SHIFT))
				break;

			const uintMem tail = textSelectionRenderUnit.selection.GetTail();
			const auto& line = textRenderUnit.GetLineData()[textRenderUnit.GetCharacterData()[tail].lineIndex];
			textSelectionRenderUnit.selection.SetTail(line.firstCharacterIndex);			

			break;
		}
		case Keyboard::Key::END: {
			if (textSelectionRenderUnit.selection.Empty() || !bool(event.modifier & Keyboard::KeyModifier::SHIFT))
				break;

			const uintMem tail = textSelectionRenderUnit.selection.GetTail();			
			const auto& line = textRenderUnit.GetLineData()[textRenderUnit.GetCharacterData()[tail].lineIndex];
			textSelectionRenderUnit.selection.SetTail(line.firstCharacterIndex + line.characterCount);			

			break;
		}
		case Keyboard::Key::C: {
			if (bool(event.modifier & Keyboard::KeyModifier::CTRL) && !textSelectionRenderUnit.selection.Empty())
				Input::SetClipboardText(textSelectionRenderUnit.selection.textContainer.GetSubString(textSelectionRenderUnit.selection.GetBeginning(), textSelectionRenderUnit.selection.GetEnd()));
			break;
		}		
		}
	}	
	void SelectableTextBase::MouseButtonDownEvent(const InputNode::MouseButtonDownEvent& event)
	{
		if (!IsPressed())
			return;

		uintMem combo = ((uintMem)event.combo - 1) % 3 + 1;

		Vec2f localSpacePos = textRenderUnit.GetFinalTransform().TransformFromFinalToLocalTransformSpace(event.screenPos);
		uintMem characterSeparationIndex = textRenderUnit.GetClosestCharacterSeparationIndex(localSpacePos);
		uintMem characterIndex = characterSeparationIndex != 0 && characterSeparationIndex == textSelectionRenderUnit.selection.textContainer.CharacterCount() ? characterSeparationIndex - 1 : characterSeparationIndex;

		switch (combo)
		{
		case 1:
			textSelectionRenderUnit.selection.Set(characterSeparationIndex, characterSeparationIndex);
			break;		
		case 2:
			textSelectionRenderUnit.selection.FocusWord(characterIndex);
			break;		
		case 3:
			textSelectionRenderUnit.selection.FocusLine(characterIndex);
			break;		
		}
	}
	void SelectableTextBase::MouseMotionEvent(const InputNode::MouseMotionEvent& event)
	{				
		if (!IsPressed())
			return;

		Vec2f localSpacePos = textRenderUnit.GetFinalTransform().TransformFromFinalToLocalTransformSpace(event.screenPos);
		uintMem characterIndex = textRenderUnit.GetClosestCharacterSeparationIndex(localSpacePos);

		textSelectionRenderUnit.selection.SetTail(characterIndex);
	}		
}