#include "pch.h"
#include "BlazeEngine/UI/Text/TextStyle.h"

namespace Blaze::UI
{
	TextStyle::TextStyle()
		: fontSize(0), underline(false), strikethrough(0), baselineOffset(0)
	{
	}
	TextStyle::TextStyle(ResourceRef<FontFace> fontFace, float fontSize, ColorRGBA color, bool underline, bool strikethrough, float baselineOffset)
		: fontFace(fontFace), fontSize(fontSize), color(color), underline(underline), strikethrough(strikethrough), baselineOffset(baselineOffset)
	{
	}
	void TextStyle::SetFontFace(Resource<FontFace> newFontFace)
	{
		fontFace = newFontFace;
		textStyleChangedEventDispatcher.Call({
			.valueChangedType = TextStyleChangedEvent::ValueChangedType::FontFace,
			.textStyle = *this
			});
	}
	void TextStyle::SetFontSize(float newFontSize)
	{
		fontSize = newFontSize;
		textStyleChangedEventDispatcher.Call({
			.valueChangedType = TextStyleChangedEvent::ValueChangedType::FontSize,
			.textStyle = *this
			});
	}
	void TextStyle::SetColor(ColorRGBA newColor)
	{
		color = newColor;
		textStyleChangedEventDispatcher.Call({
			.valueChangedType = TextStyleChangedEvent::ValueChangedType::Color,
			.textStyle = *this
			});
	}
	void TextStyle::SetUnderline(bool newUnderline)
	{
		underline = newUnderline;
		textStyleChangedEventDispatcher.Call({
			.valueChangedType = TextStyleChangedEvent::ValueChangedType::Underline,
			.textStyle = *this
			});
	}
	void TextStyle::SetStrikethrough(bool newStrikethrough)
	{
		strikethrough = newStrikethrough;
		textStyleChangedEventDispatcher.Call({
			.valueChangedType = TextStyleChangedEvent::ValueChangedType::Strikethrough,
			.textStyle = *this
			});
	}
	void TextStyle::SetBaselineOffset(float newBaselineOffset)
	{
		baselineOffset = newBaselineOffset;
		textStyleChangedEventDispatcher.Call({
			.valueChangedType = TextStyleChangedEvent::ValueChangedType::BaselineOffset,
			.textStyle = *this
			});
	}
}