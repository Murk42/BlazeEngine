#pragma once
#include "BlazeEngine/Core/Common/Color.h"
#include "BlazeEngine/Core/Event/EventDispatcher.h"
#include "BlazeEngine/Core/Resource/ResourceRef.h"
#include "BlazeEngine/UI/Text/FontFace.h"

namespace Blaze::UI
{
	class TextStyle
	{
	public:
		struct TextStyleChangedEvent
		{
			enum class ValueChangedType
			{
				FontFace,
				FontSize,
				Color,
				Underline,
				Strikethrough,
				BaselineOffset
			};

			ValueChangedType valueChangedType;
			TextStyle& textStyle;
		};
		EventDispatcher<TextStyleChangedEvent> textStyleChangedEventDispatcher;

		TextStyle();
		TextStyle(ResourceRef<FontFace> fontFace, float fontSize, ColorRGBA color = 0xffffffff, bool underline = false, bool strikethrough = false, float baselineOffset = 0.0f);
		TextStyle(const TextStyle&) = default;

		void SetFontFace(Resource<FontFace> newFontFace);
		void SetFontSize(float newFontSize);
		void SetColor(ColorRGBA newColor);
		void SetUnderline(bool newUnderline);
		void SetStrikethrough(bool newStrikethrough);
		void SetBaselineOffset(float newBaselineOffset);

		inline ResourceRef<FontFace> GetFontFace() const { return fontFace; }
		inline float GetFontSize() const { return fontSize; }
		inline ColorRGBA GetColor() const { return color; }
		inline bool HasUnderline() const { return underline; }
		inline bool HasStrikethrough() const { return strikethrough; }
		inline float GetBaselineOffset() const { return baselineOffset; }
	private:
		ResourceRef<FontFace> fontFace;

		float fontSize;
		ColorRGBA color;

		bool underline : 1;
		bool strikethrough : 1;

		float baselineOffset; //for super-script for example
	};
}