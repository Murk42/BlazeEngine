#include "pch.h"
#include "BlazeEngineGraphics/UI/Nodes/SelectableText.h"

namespace Blaze::UI::Nodes
{
	SelectableText::SelectableText() :
		SelectableTextBase(textRenderUnit, textSelectionRenderUnit, textCursorRenderUnit), 
		textRenderUnit(this), textSelectionRenderUnit(textRenderUnit), textCursorRenderUnit(textRenderUnit)
	{
		dataMap.SetTypeName("Text");
	}
	StringUTF8 SelectableText::GetTextSubString(uintMem begin, uintMem end)
	{
		return textRenderUnit.GetText().SubString(begin, end);
	}
	Graphics::RenderUnit* SelectableText::GetRenderUnit(uint index)
	{
		switch (index)
		{
		case 0:
			return &textRenderUnit;			
		case 1:
			return &textSelectionRenderUnit;			
		default:
			return nullptr;			
		}
	}
}