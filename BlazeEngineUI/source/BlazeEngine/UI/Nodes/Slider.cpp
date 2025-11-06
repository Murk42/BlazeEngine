#include "pch.h"
#include "BlazeEngine/UI/Nodes/Slider.h"
#include "BlazeEngine/UI/Input/InputManager.h"

namespace Blaze::UI::Nodes
{
	Slider::Slider()
		: renderUnitDirty(true), highlighted(false), handleSize(30, 12), trackThickness(1), value(0.0f)
	{
		dataMap.SetTypeName("Slider");

		SetMouseMotionReportPolicy(MouseMotionReportPolicy::ReportWhileSelected);
		SetTransform({ .size = { 200, 20 } });
		SetStyle(Style());

		finalTransformUpdatedEventDispatcher.AddHandler<&Slider::FinalTransformUpdatedEvent>(*this);
		mouseHitStatusChangedEventDispatcher.AddHandler<&Slider::MouseHitStatusChangedEvent>(*this);
		pressedStateChangedEventDispatcher.AddHandler<&Slider::PressedStateChangedEvent>(*this);
		mouseButtonDownEventDispatcher.AddHandler<&Slider::MouseButtonDownEvent>(*this);
		mouseMotionEventDispatcher.AddHandler<&Slider::MouseMotionEvent>(*this);
	}
	Slider::~Slider()
	{
		finalTransformUpdatedEventDispatcher.RemoveHandler<&Slider::FinalTransformUpdatedEvent>(*this);
		mouseHitStatusChangedEventDispatcher.RemoveHandler<&Slider::MouseHitStatusChangedEvent>(*this);
		pressedStateChangedEventDispatcher.RemoveHandler<&Slider::PressedStateChangedEvent>(*this);
		mouseButtonDownEventDispatcher.RemoveHandler<&Slider::MouseButtonDownEvent>(*this);
		mouseMotionEventDispatcher.RemoveHandler<&Slider::MouseMotionEvent>(*this);
	}
	int Slider::HitTest(Vec2f screenPosition)
	{
		auto transform = GetFinalTransform();
		Vec2f right = Vec2f(Math::Cos(transform.rotation), Math::Sin(transform.rotation));
		Vec2f up = Vec2f(-right.y, right.x);

		Vec2f nodePosition = screenPosition - transform.position;

		nodePosition = {
			right.DotProduct(nodePosition),
			up.DotProduct(nodePosition),
		};

		auto InRect = [](Vec2f point, Rectf rect) { return point.x >= rect.x && point.x < rect.x + rect.w && point.y >= rect.y && point.y < rect.y + rect.h; };

		Rectf handleHitbox;
		handleHitbox.size = {
			handleSize.x,
			std::max({handleSize.y, trackThickness, 10.0f})
		};
		handleHitbox.pos = {
			(transform.size.x - handleHitbox.size.x) * value,
			(transform.size.y - handleHitbox.size.y) / 2
		};

		if (InRect(nodePosition, handleHitbox))
			return 2;

		Rectf trackHitbox;
		trackHitbox.size = {
			transform.size.x,
			trackThickness > 10 ? trackThickness : 10
		};
		trackHitbox.pos = {
			0,
			(transform.size.y - trackHitbox.size.y) / 2
		};

		if (InRect(nodePosition, trackHitbox))
			return 1;

		return 0;
	}
	void Slider::SetStyle(const Style& style)
	{
		highlightTint = style.highlightTint;
		pressedTint = style.pressedTint;

		handleFillColor = style.handleFillColor;
		handleBorderColor = style.handleBorderColor;
		handleRenderUnit.borderWidth = style.handleBorderWidth;
		handleRenderUnit.cornerRadius = style.handleCornerRadius;

		trackRenderUnit.fillColor = style.trackFillColor;
		trackRenderUnit.borderColor = style.trackBorderColor;
		trackRenderUnit.borderWidth = style.trackBorderWidth;
		trackRenderUnit.cornerRadius = style.trackCornerRadius;

		UpdateColor();
	}
	void Slider::SetStepCount(uint32 stepCount)
	{
		this->stepCount = stepCount;
		renderUnitDirty = true;
	}
	void Slider::SetValue(float value)
	{
		if (this->value != value)
		{
			this->value = value;
			renderUnitDirty = true;

			valueChangedEventDispatcher.Call({ *this });

			//To update the hit test
			MarkFinalTransformDirty();
		}
	}
	void Slider::SetHandleSize(Vec2f handleSize)
	{
		this->handleSize = handleSize;
		renderUnitDirty = true;

		//To update the hit test
		MarkFinalTransformDirty();
	}
	void Slider::SetTrackThickness(float thickness)
	{
		trackThickness = thickness;
		renderUnitDirty = true;
	}
	void Slider::PreRender(const UIRenderContext& renderContext)
	{
		CleanFinalTransform();

		if (!renderUnitDirty)
			return;

		renderUnitDirty = false;

		auto transform = GetFinalTransform();
		Vec2f right = Vec2f(Math::Cos(transform.rotation), Math::Sin(transform.rotation));
		Vec2f up = Vec2f(-right.y, right.x);

		trackRenderUnit.pos = transform.position + Vec2f(0, (transform.size.y - trackThickness) / 2);
		trackRenderUnit.right = right * transform.size.x;
		trackRenderUnit.up = up * trackThickness;

		handleRenderUnit.pos = transform.position + right * (transform.size.x - handleSize.x) * value + up * (transform.size.y - handleSize.y) / 2;
		handleRenderUnit.right = right * handleSize.x;
		handleRenderUnit.up = up * handleSize.y;
	}
	UIRenderUnitBase* Slider::GetRenderUnit(uintMem index)
	{
		switch (index)
		{
		case 0:
			return &trackRenderUnit;
		case 1:
			return &handleRenderUnit;
		default:
			return nullptr;
		}
	}
	static float Mix(float a, float b, float t)
	{
		return b * t + a * (1 - t);
	}
	static ColorRGBAf Mix(ColorRGBAf a, ColorRGBAf b)
	{
		return { Mix(a.r, b.r, b.a), Mix(a.g, b.g, b.a), Mix(a.b, b.b, b.a), a.a };
	}
	void Slider::UpdateColor()
	{
		if (IsPressed())
		{
			handleRenderUnit.fillColor = Mix(handleFillColor, pressedTint);
			handleRenderUnit.borderColor = Mix(handleBorderColor, pressedTint);
		}
		else if (highlighted)
		{
			handleRenderUnit.fillColor = Mix(handleFillColor, highlightTint);
			handleRenderUnit.borderColor = Mix(handleBorderColor, highlightTint);
		}
		else
		{
			handleRenderUnit.fillColor = handleFillColor;
			handleRenderUnit.borderColor = handleBorderColor;
		}
	}
	void Slider::FinalTransformUpdatedEvent(const Node::FinalTransformUpdatedEvent& event)
	{
		renderUnitDirty = true;
	}
	void Slider::MouseHitStatusChangedEvent(const InputNode::MouseHitStatusChangedEvent& event)
	{
		highlighted = event.newHitStatus > 0;
		UpdateColor();

		if (event.newHitStatus > 0)
			Input::SetCursorType(Input::CursorType::ResizeEW);
		else
			Input::SetCursorType(Input::CursorType::Default);
	}
	void Slider::PressedStateChangedEvent(const ButtonBase::PressedStateChangedEvent& event)
	{
		UpdateColor();
		if (IsPressed())
		{
			auto inputManager = InputManager::GetScreenInputManager(*GetScreen());
			auto transform = GetFinalTransform();
			Vec2f right = Vec2f(Math::Cos(transform.rotation), Math::Sin(transform.rotation));

			pressedOffset = right.DotProduct(inputManager->GetScreenMousePos() - transform.position) - value * (transform.size.x - handleSize.x);
		}
	}
	void Slider::MouseButtonDownEvent(const InputNode::MouseButtonDownEvent& event)
	{
		if (GetMouseHitStatus() == 1 && event.button == Input::MouseButton::Left)
		{
			auto transform = GetFinalTransform();
			Vec2f right = Vec2f(Math::Cos(transform.rotation), Math::Sin(transform.rotation));
			float newValue = (right.DotProduct(event.screenPos - transform.position) - handleSize.x / 2) / (transform.size.x - handleSize.x);

			if (newValue < 0.0f)
				newValue = 0.0f;
			else if (newValue > 1.0f)
				newValue = 1.0f;

			SetValue(newValue);
		}
	}
	void Slider::MouseMotionEvent(const InputNode::MouseMotionEvent& event)
	{
		if (IsPressed())
		{
			auto transform = GetFinalTransform();
			Vec2f right = Vec2f(Math::Cos(transform.rotation), Math::Sin(transform.rotation));
			float newValue = (right.DotProduct(event.screenPos - transform.position) - pressedOffset) / (transform.size.x - handleSize.x);

			if (newValue < 0.0f)
				newValue = 0.0f;
			else if (newValue > 1.0f)
				newValue = 1.0f;

			SetValue(newValue);
		}
	}
}