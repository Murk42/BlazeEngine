#pragma once
#include "RegionSplitter.h"

//class Slider : UI::InputNode, Graphics::RenderObject
//{
//public:
//	struct SliderValueUpdatedEvent
//	{
//		Slider& slider;
//	};
//
//	UI::TextRenderUnit labelRenderUnit;
//	UI::TextRenderUnit valueRenderUnit;
//	UI::PanelRenderUnit barRenderUnit;
//	UI::PanelRenderUnit sliderRenderUnit;
//
//	EventDispatcher<SliderValueUpdatedEvent> sliderValueUpdatedEventDispatcher;
//
//	Slider();
//
//	void SetLabel(StringViewUTF8 label);
//	void SetLabelFontStyle(const UI::FontStyle& fontStyle);
//	void SetLabelColor(ColorRGBAf color);	
//
//	void SetValue(float value);
//	void SetValueFontStyle(const UI::FontStyle& fontStyle);
//	void SetValueColor(ColorRGBAf color);
//
//	void SetBarColor(ColorRGBAf color);
//	void SetSliderColor(ColorRGBAf color);
//	
//	Graphics::RenderUnit* GetRenderUnit(uint index) override;
//
//	inline float GetValue() const { return value; }
//private:
//	UI::StringUTF8TextContainer labelTextContainer;
//	UI::StringUTF8TextContainer valueTextContainer;
//	float value;
//};

class MainScreen : public UI::Screen
{	
public:
	ResourceManager& resourceManager;

	ResourceRef<TextStyle> textStyle;
	UI::Nodes::Text text;

	ResourceRef<FontAtlas> atlas;

	MainScreen(ResourceManager& resourceManager);
};