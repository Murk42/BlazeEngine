#include "pch.h"
#include "BlazeEngineGraphics/UI/Nodes/Text.h"

namespace Blaze::UI::Nodes
{
	Text::Text() 
		: renderUnit(textContainer, this)
	{		
		dataMap.SetTypeName("Text");				
	}
	Text::~Text()
	{		
	}
	void Text::SetText(StringViewUTF8 textRenderUnit)
	{
		textContainer.SwapStrings(textRenderUnit);		
	}
	void Text::SetTextColor(ColorRGBAf color)
	{
		renderUnit.SetTextColor(color);
	}
	void Text::SetTextCharactersColor(const ArrayView<ColorRGBAf>& colors)
	{
		renderUnit.SetTextCharactersColor(colors);
	}
	void Text::SetFontStyle(const FontStyle& fontStyle)
	{
		renderUnit.SetFontStyle(fontStyle);
	}	
	void Text::SetLayoutOptions(TextLayoutOptions layoutOptions)
	{
		renderUnit.SetLayoutOptions(layoutOptions);		
	}
	void Text::SetCullingNode(Node* cullingNode)
	{
		renderUnit.SetCullingNode(cullingNode);		
	}	
	Graphics::RenderUnit* Text::GetRenderUnit(uint index)
	{
		if (index == 0)
			return &renderUnit;
		return nullptr;
	}	
}