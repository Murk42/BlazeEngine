#pragma once
#include "BlazeEngine/Application/UI/UIComponent.h"
#include "BlazeEngine/Application/UI/UISystem.h"
#include "BlazeEngine/Resources/Font/Font.h"
#include "BlazeEngine/Event/LambdaEventHandler.h"
#include "BlazeEngine/Resources/Font/TextLayouter.h"

namespace Blaze::UI::Components
{
	enum class LabelSizeControl
	{
		WidthControlsHeight,
		HeightControlsWidth,
		CustomSize,		
	};

	class BLAZE_API LabelSystem : public UISystem
	{
	public:
		void Render(UIScene*, UIComponent*) override;
		void PreTransform(UIScene*, UIComponent*) override;
	};

	class BLAZE_API Label : public UIComponent
	{				
	public:		
		Label();
		Label(const Label&);
		Label(Label&&) noexcept;		
		Label(StringViewUTF8 text, ArrayView<ColorRGBAf> colors, LabelSizeControl sizeControl, FontResolution* fontResolution);
		~Label();

		void SetText(StringViewUTF8 text);					
		void SetColors(ArrayView<ColorRGBAf> colors);
		void SetColor(ColorRGBAf color);		
		void SetSizeControl(LabelSizeControl control);
		
		void Setup(StringViewUTF8 text, ArrayView<ColorRGBAf> colors, LabelSizeControl sizeControl);
		
		inline StringUTF8 GetText() const { return text; }		
		inline LabelSizeControl GetSizeControl() const { return sizeControl; }		
		inline const TextLayouterBase& GetTextLayouter() const { return textLayouter; }
				
		COMPONENT(Label, LabelSystem)
	private:
		uint8 state;

		FontResolution* fontResolution;		
		StringUTF8 text;
		Array<ColorRGBAf> colors;		
		LabelSizeControl sizeControl;
		SingleLineTextLayouter textLayouter;

		friend class System;
	};			
};