#include "pch.h"
#include "BlazeEngineGraphics/UI/Nodes/EditableText.h"
#include "BlazeEngineGraphics/UI/Input/InputManager.h"
#include "BlazeEngine/Input/Input.h"

namespace Blaze::UI::Nodes
{			
	EditableText::EditableText() :
		EditableTextBase(textRenderUnit, textSelectionRenderUnit, textCursorRenderUnit),
		textRenderUnit(this), textSelectionRenderUnit(textRenderUnit), textCursorRenderUnit(textRenderUnit),
		selected(false), emptyColor(0x555555ff), selectedColor(0xf5f5f5ff), unselectedColor(0xd0d0d0ff), emptyText("Enter text")
	{
		UpdateTextRenderUnit();
	}
	void EditableText::SetText(StringViewUTF8 text)
	{
		this->text = text;

		UpdateTextRenderUnit();
	}
	void EditableText::SetEmptyText(StringViewUTF8 text)
	{
		emptyText = text;

		UpdateTextRenderUnit();
	}
	Graphics::RenderUnit* EditableText::GetRenderUnit(uint index)
	{
		switch (index)
		{
		case 0:
			return &textRenderUnit;
		case 1:
			return &textSelectionRenderUnit;
		case 2:
			return &textCursorRenderUnit;
		default:
			return nullptr;
		}
	}
	void EditableText::EraseTextSubString(uintMem begin, uintMem count)
	{
		text.EraseSubString(begin, count);
		textRenderUnit.SetText(text);
		textEdited = true;
		textChangedEventDispatcher.Call({ this, text });
	}
	void EditableText::InsertStringIntoText(uintMem index, StringViewUTF8 string)
	{
		text.InsertString(index, string);
		textRenderUnit.SetText(text);
		textEdited = true;
		textChangedEventDispatcher.Call({ this, text });
	}
	StringUTF8 EditableText::GetTextSubString(uintMem begin, uintMem end)
	{
		return text.SubString(begin, end - begin);
	}
	void EditableText::UpdateTextRenderUnit()
	{
		if (selected)
		{
			//Set focused text
			textRenderUnit.SetText(text);
			textRenderUnit.SetTextColor(selectedColor);
		}
		else if (text.Empty())
		{			
			//Set empty text
			textRenderUnit.SetText(emptyText);			
			textSelectionRenderUnit.ClearSelection();			
			textRenderUnit.SetTextColor(emptyColor);
		}
		else
		{
			//Set unfocused text		
			textRenderUnit.SetText(text);
			textRenderUnit.SetTextColor(unselectedColor);
		}
	}
	void EditableText::OnEvent(const SelectedEvent& event)
	{
		selected = true;
		textCursorRenderUnit.ShowCursor();
		UpdateTextRenderUnit();

		EditableTextBase::OnEvent(event);
	}
	void EditableText::OnEvent(const DeselectedEvent& event)
	{
		selected = false;
		EditableTextBase::OnEvent(event);

		textCursorRenderUnit.HideCursor();
		UpdateTextRenderUnit();

		if (textEdited)
			textEnteredEventDispatcher.Call({ this, text });
	}
}