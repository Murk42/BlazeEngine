#pragma once
#include "RegionSplitter.h"

class Slider : UI::InputNode, Graphics::RenderObject
{
public:
	struct SliderValueUpdatedEvent
	{
		Slider& slider;
	};

	UI::TextRenderUnit labelRenderUnit;
	UI::TextRenderUnit valueRenderUnit;
	UI::PanelRenderUnit barRenderUnit;
	UI::PanelRenderUnit sliderRenderUnit;

	EventDispatcher<SliderValueUpdatedEvent> sliderValueUpdatedEventDispatcher;

	Slider();

	void SetLabel(StringViewUTF8 label);
	void SetLabelFontStyle(const UI::FontStyle& fontStyle);
	void SetLabelColor(ColorRGBAf color);	

	void SetValue(float value);
	void SetValueFontStyle(const UI::FontStyle& fontStyle);
	void SetValueColor(ColorRGBAf color);

	void SetBarColor(ColorRGBAf color);
	void SetSliderColor(ColorRGBAf color);
	
	Graphics::RenderUnit* GetRenderUnit(uint index) override;

	inline float GetValue() const { return value; }
private:
	UI::StringUTF8TextContainer labelTextContainer;
	UI::StringUTF8TextContainer valueTextContainer;
	float value;
};

extern Array<Font> fonts;
extern Map<String, UI::FontStyle> fontStyles;

class ReplaceTextMenu : 
	public UI::Node
{
public:
	UI::Nodes::Panel backgroundPanel;
	UI::Nodes::Panel mainPanel;
	UI::Nodes::Panel targetTextPanel;
	UI::Nodes::EditableText targetText;
	UI::Nodes::Panel replaceTextPanel;
	UI::Nodes::EditableText replaceText;
	UI::Nodes::TextButton replaceButton;
	UI::Nodes::TextButton cancelButton;	

	UI::Nodes::EditableText& target;

	ReplaceTextMenu(UI::Nodes::EditableText& target);
	~ReplaceTextMenu();
};

class MainScreen : public UI::Screen
{	
public:
	ResourceManager& resourceManager;

	UI::Nodes::Panel explanationPanel;
	UI::Nodes::SelectableText explanationText;
	UI::Nodes::Image image;
	UI::Nodes::EditableText editableText;			
	UI::Nodes::TextButton clearTextButton;
	UI::Nodes::TextButton replaceTextButton;	
	UI::Nodes::TextButton reloadImageTextButton;	

	Graphics::OpenGLWrapper::Texture2D APODTexture;

	ReplaceTextMenu replaceTextMenu;

	MainScreen(ResourceManager& resourceManager);	

	void LoadAPOD();
};