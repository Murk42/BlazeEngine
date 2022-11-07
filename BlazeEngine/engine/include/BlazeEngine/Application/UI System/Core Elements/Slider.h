#pragma once
#include "BlazeEngine/Core/EngineCore.h"
#include "BlazeEngine/Application/UI System/UIElement.h"
#include "BlazeEngine/Application/UI System/Core Elements/Panel.h"
#include "BlazeEngine/Application/UI System/Core Elements/Button.h"
#include "BlazeEngine/Event/EventManager.h"

namespace Blaze::UI
{
	class SliderManager;

	class BLAZE_API Slider : public UIElement,
		EventHandler<Event::MouseMotion>,
		EventHandler<Event::MouseScroll>
	{
		Panel backgroundPanel;
		Button backgroundButton;
		Panel sliderPanel;
		Button sliderButton;
		float value;
		float min, max;

		bool moving;
		Vec2f mouseOffset;

		void SetRawValue(float value);

		void OnEvent(Event::MouseMotion event) override;
		void OnEvent(Event::MouseScroll event) override;

		void AttachedToManager() override;
		void DetachedFromManager() override;
	public:
		Slider();
		~Slider();

		UIEvent valueChanged;
		
		void SetPos(Vec2f pos) override;
		void SetSize(Vec2f size) override;
		
		void SetActiveFlag(bool active) override;
		void SetLocalAlignment(Align align) override;
		void SetAnchorAlignment(Align align) override;
		void SetAnchor(UIElement* anchor) override;
		void SetDepth(float value) override;

		void SetSliderWidth(float width);

		void SetBackgroundColor(ColorRGBAf color);
		void SetSliderColor(ColorRGBAf color);

		void SetRange(float min, float max);

		void SetValue(float value);

		inline ColorRGBAf GetBackgroundColor() const { return backgroundPanel.fillColor; }
		inline ColorRGBAf GetSliderColor()  const{ return sliderPanel.fillColor; }
		inline float GetSliderWidth() const { return sliderPanel.GetSize().x; }
		inline Vec2f GetRange() const { return Vec2f(min, max); }

		float GetValue() const;

		static constexpr const char* typeName = "Slider";
		using ManagerType = SliderManager;
	};

	class BLAZE_API SliderManager : public UIElementManager<Slider>
	{
	public:		
	};
}