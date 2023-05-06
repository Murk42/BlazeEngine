#pragma once
#include "BlazeEngine/Application/UI System2/UIElement.h"
#include "BlazeEngine/DataStructures/Color.h"
#include "BlazeEngine/Resources/Font/Font.h"
#include "BlazeEngine/Graphics/Renderers/TextRenderCache.h"

namespace Blaze::UI2::Elements
{
	class LabelSystem;

	class BLAZE_API Label : public UIElement
	{		
		bool dirty;
		Graphics::TextRenderCache renderCache;

		FontResolution* fontResolution;		
		StringUTF8 text;
		std::vector<ColorRGBAf> colors;
		float fontHeight;
	public:		
		Label();
		Label(StringViewUTF8 text, float fontHeight, ColorRGBAf color);
		Label(StringViewUTF8 text, float fontHeight, ColorRGBAf color, FontResolution* fontResolution);		
		~Label();

		void SetText(StringViewUTF8 text);		
		void SetFontHeight(float fontHeight);
		void SetFontResolution(FontResolution* fontResolution);
		void SetColors(std::span<ColorRGBAf> colors);
		void SetColor(ColorRGBAf color);
		
		inline FontResolution* GetFontResolution() const { return fontResolution; }
		inline size_t GetFontHeight() const { return fontHeight; }
		inline StringUTF8 GetText() const { return text; }		

		UIELEMENT(Label, LabelSystem);
		friend class System;
	};

	class BLAZE_API LabelSystem : public ECS::System
	{		
	public:
		void Update(ECS::Component*) override;

	};
};