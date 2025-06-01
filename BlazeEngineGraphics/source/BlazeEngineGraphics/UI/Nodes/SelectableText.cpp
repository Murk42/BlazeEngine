#include "pch.h"
#include "BlazeEngineGraphics/UI/Nodes/SelectableText.h"
#include "BlazeEngine/Input/Input.h"

namespace Blaze::UI::Nodes
{
	SelectableText::SelectableText() :
		SelectableTextBase(textRenderUnit, textSelectionRenderUnit), 
		selection(textContainer), textRenderUnit(textContainer, this), textSelectionRenderUnit(selection, textRenderUnit)
	{
		dataMap.SetTypeName("Text");
		mouseHitStatusChangedEventDispatcher.AddHandler<&SelectableText::MouseHitStatusChangedEvent>(*this);
	}
	SelectableText::~SelectableText()
	{
		mouseHitStatusChangedEventDispatcher.RemoveHandler<&SelectableText::MouseHitStatusChangedEvent>(*this);
	}
	void SelectableText::SetText(StringViewUTF8 text)
	{
		textContainer.SwapStrings(text);
	}
	Graphics::RenderUnit* SelectableText::GetRenderUnit(uint index)
	{		
		switch (index)
		{
		case 0: return &textRenderUnit;
		case 1: return &textSelectionRenderUnit;
		default: return nullptr;
		}
	}
	void SelectableText::MouseHitStatusChangedEvent(const InputNode::MouseHitStatusChangedEvent& event)
	{
		if (GetMouseHitStatus() == 2)
			Input::SetCursorType(Input::CursorType::Text);
		else
			Input::SetCursorType(Input::CursorType::Default); //This updates the cursor every frame. Fix this
	}
}