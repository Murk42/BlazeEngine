#include "pch.h"
#include "BlazeEngine/Resources/Font/TextLayouter.h"
#include "BlazeEngineGraphics/UI/Nodes/Text.h"

namespace Blaze::UI
{
	Text::Text() :
		renderUnit(this)
	{		
		dataMap.SetTypeName("Text");				
	}
	Text::~Text()
	{		
	}
	void Text::SetText(StringUTF8 textRenderUnit)
	{
		renderUnit.SetText(textRenderUnit);	
	}
	void Text::SetTextColor(ColorRGBAf color)
	{
		renderUnit.SetTextColor(color);
	}
	void Text::SetTextCharactersColor(const ArrayView<ColorRGBAf>& colors)
	{
		renderUnit.SetTextCharactersColor(colors);
	}
	void Text::SetFont(Font& font)
	{
		renderUnit.SetFont(font);				
	}
	void Text::SetFontHeight(uint pixelFontHeight)
	{		
		renderUnit.SetFontHeight(pixelFontHeight);
	}
	void Text::SetLayoutOptions(TextLayoutOptions layoutOptions)
	{
		renderUnit.SetLayoutOptions(layoutOptions);		
	}
	void Text::SetCullingNode(UI::Node* cullingNode)
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