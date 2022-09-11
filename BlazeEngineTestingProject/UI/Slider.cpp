#include "Slider.h"

namespace Blaze::UI
{
	void Slider::SetRawValue(float newValue)
	{
		newValue = std::clamp(newValue, 0.0f, 1.0f);

		float size = backgroundPanel.GetSize().x - sliderPanel.GetSize().x;
		sliderPanel.SetPos(Vec2f(newValue * size, 0));

		float old = value;
		value = newValue;

		if (value != old)
			valueChanged();
	}

	void Slider::OnEvent(Event::MouseMotion event)
	{
		float x = event.pos.x + mouseOffset.x;
		float size = backgroundPanel.GetSize().x - sliderPanel.GetSize().x;

		SetRawValue(x / size);
	}

	void Slider::OnEvent(Event::MouseScroll event)
	{
		if (!GetManager()->IsFocusTaken())
		{
			if (backgroundButton.state == ButtonState::Hovered)
			{
				float size = backgroundPanel.GetSize().x - sliderPanel.GetSize().x;
				SetRawValue(value + (float)event.value / size * 5);
			}
		}
	}

	void Slider::AttachedToManager()
	{
		auto* manager = GetManager();
		uint layer = GetLayer();

		manager->AddElement(&backgroundPanel, layer);
		manager->AddElement(&backgroundButton, layer);
		manager->AddElement(&sliderPanel, layer);
		manager->AddElement(&sliderButton, layer);
	}

	Slider::Slider()
		: value(0), min(0), max(1)
	{
		EventManager::AddEventHandler<Event::MouseScroll>(*this);

		sliderButton.entered += [&]() {
			if (!GetManager()->IsFocusTaken())
				sliderPanel.fillColor = 0x484848ff;
		};
		sliderButton.pressed += [&]() {
			if (!GetManager()->TakeFocus())
			{
				sliderPanel.fillColor = 0x404040ff;
				moving = true;
				mouseOffset = sliderPanel.GetPos() - (Vec2f)Input::GetMousePos();
				EventManager::AddEventHandler<Event::MouseMotion>(*this);
			}
		};
		sliderButton.released += [&]() {
			if (moving)
			{
				sliderPanel.fillColor = 0x484848ff;
				moving = false;
				GetManager()->ReturnFocus();
				EventManager::RemoveEventHandler<Event::MouseMotion>(*this);
			}
		};
		sliderButton.left += [&]() {
			sliderPanel.fillColor = 0x555555ff;
		};
		
		SetRect({ 0, 0, 200, 20 });

		backgroundPanel.borderWidth = 0;
		backgroundPanel.fillColor = 0x333333ff;

		backgroundButton.SetAnchor(&backgroundPanel);
		backgroundButton.SetSize(backgroundPanel.GetSize());

		backgroundButton.pressed += [&]() {
			float size = backgroundPanel.GetSize().x - sliderPanel.GetSize().x;
			SetRawValue(((float)Input::GetMousePos().x - backgroundButton.GetAlignedPos().x - sliderPanel.GetSize().x / 2) / size);
		};

		sliderPanel.SetAnchor(&backgroundPanel);
		sliderPanel.SetSize({ 30, backgroundPanel.GetSize().y });
		sliderPanel.borderWidth = 0;
		sliderPanel.fillColor = 0x555555ff;
		sliderPanel.SetAnchorAlignment(Align::Left);
		sliderPanel.SetLocalAlignment(Align::Left);

		sliderButton.SetAnchor(&sliderPanel);
		sliderButton.SetSize(sliderPanel.GetSize());
	}

	Slider::~Slider()
	{
	}

	void Slider::SetRect(Rectf rect)
	{
		UIElement::SetRect(rect);
		sliderButton.SetSize(rect.size);
		sliderPanel.SetSize(Vec2f(sliderPanel.GetSize().x, rect.h));
		backgroundPanel.SetRect(rect);
		backgroundButton.SetSize(backgroundPanel.GetSize());
	}

	void Slider::SetPos(Vec2f pos)
	{
		backgroundPanel.SetPos(pos);
		UIElement::SetPos(pos);
	}

	void Slider::SetSize(Vec2f size)
	{
		backgroundPanel.SetSize(size);
		sliderPanel.SetSize(Vec2f(sliderPanel.GetSize().x, size.y));
		sliderButton.SetSize(sliderPanel.GetSize());
		backgroundButton.SetSize(backgroundPanel.GetSize());
		UIElement::SetSize(size);
	}

	void Slider::SetClickableFlag(bool clickable)
	{
		backgroundButton.SetClickableFlag(clickable);
		sliderButton.SetClickableFlag(clickable);
		UIElement::SetClickableFlag(clickable);
	}

	void UI::Slider::SetActiveFlag(bool active)
	{
		backgroundPanel.SetActiveFlag(active);
		backgroundButton.SetActiveFlag(active);
		sliderPanel.SetActiveFlag(active);
		sliderButton.SetActiveFlag(active);
		UIElement::SetActiveFlag(active);
	}

	void Slider::SetLocalAlignment(Align align)
	{
		backgroundPanel.SetLocalAlignment(align);
		UIElement::SetLocalAlignment(align);
	}

	void Slider::SetAnchorAlignment(Align align)
	{
		backgroundPanel.SetAnchorAlignment(align);
		UIElement::SetAnchorAlignment(align);
	}

	void Slider::SetAnchor(UIElement* anchor)
	{
		backgroundPanel.SetAnchor(anchor);
		UIElement::SetAnchor(anchor);
	}

	void Slider::SetSliderWidth(float width)
	{
		sliderPanel.SetSize(Vec2f(width, sliderPanel.GetSize().y));
		sliderButton.SetSize(sliderPanel.GetSize());
	}

	void Slider::SetBackgroundColor(ColorRGBAf color)
	{
		backgroundPanel.fillColor = color;
	}

	void Slider::SetSliderColor(ColorRGBAf color)
	{
		sliderPanel.fillColor = color;
	}

	void Slider::SetRange(float min, float max)
	{
		this->min = min;
		this->max = max;
	}

	void Slider::SetValue(float value)
	{
		SetRawValue((value - min) / (max - min));
	}

	float Slider::GetValue() const
	{
		return value * (max - min) + min;
	}
	UIElementParsingData SliderManager::GetElementParsingData()
	{
		UIElementParsingData data = UIBaseElementManager::GetElementParsingData();

		data.AddMember<Slider, ColorRGBAf>("backgroundColor", &Slider::SetBackgroundColor, &Slider::GetBackgroundColor);
		data.AddMember<Slider, ColorRGBAf>("sliderColor", &Slider::SetSliderColor, &Slider::GetSliderColor);
		data.AddMember<Slider, float>("sliderWidth", &Slider::SetSliderWidth, &Slider::GetSliderWidth);
		data.AddMember<Slider, Vec2f>("range", [](UIScene&, Slider& slider, Vec2f range) {
				slider.SetRange(range.x, range.y);
			}, [](UIScene&, Slider& slider) {
				return slider.GetRange();
			});
		data.AddMember<Slider, float>("value", &Slider::SetValue, &Slider::GetValue);
		data.AddMember<Slider, String>("valueChanged", [](UIScene& scene, Slider& slider, const String& name) {
			slider.valueChanged += scene.GetEventFunction(name);
			}, nullptr);

		return data;
	}

}