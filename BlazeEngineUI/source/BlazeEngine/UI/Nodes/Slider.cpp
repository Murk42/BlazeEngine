#include "pch.h"
#include "BlazeEngine/UI/Nodes/Slider.h"
#include "BlazeEngine/UI/Input/InputSubSystem.h"

namespace Blaze::UI::Nodes
{
	Slider::Slider()
		: renderUnitDirty(true), highlighted(false), handleSize(30, 12), trackThickness(1), value(0.0f), draggingMouseOffset(0)
	{
		dataMap.SetTypeName("Slider");

		SetStyle(Style());
		SetTransform({ .size = { 200, 20 } });

		finalTransformUpdatedEventDispatcher.AddHandler<&Slider::FinalTransformUpdatedEvent>(*this);
		mouseHitStatusChangedEventDispatcher.AddHandler<&Slider::MouseHitStatusChangedEvent>(*this);
		pressedStateChangedEventDispatcher.AddHandler<&Slider::PressedStateChangedEvent>(*this);
		mouseMotionEventDispatcher.AddHandler<&Slider::MouseMotionEvent>(*this);
	}
	Slider::Slider(Node& parent, const NodeTransform& transform, float value, Vec2f handleSize, float trackThickness, const Style& style)
		: Slider()
	{
		SetStyle(style);
		SetTrackThickness(trackThickness);
		SetHandleSize(handleSize);
		SetValue(value);
		SetTransform(transform);
		SetParent(&parent);
	}
	Slider::~Slider()
	{
		finalTransformUpdatedEventDispatcher.RemoveHandler<&Slider::FinalTransformUpdatedEvent>(*this);
		mouseHitStatusChangedEventDispatcher.RemoveHandler<&Slider::MouseHitStatusChangedEvent>(*this);
		pressedStateChangedEventDispatcher.RemoveHandler<&Slider::PressedStateChangedEvent>(*this);
		mouseMotionEventDispatcher.RemoveHandler<&Slider::MouseMotionEvent>(*this);
	}
	/*Node::HitStatus Slider::HitTest(Vec2f screenPosition)
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
			std::max(handleSize.y, trackThickness)
		};
		handleHitbox.pos = {
			transform.size.x > handleHitbox.size.x ? (transform.size.x - handleHitbox.size.x) * value : (transform.size.x - handleHitbox.size.x) * 0.5f,
			(transform.size.y - handleHitbox.size.y) / 2
		};

		if (InRect(nodePosition, handleHitbox))
			return HitStatus::HitBlocking;

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
			return HitStatus::HitBlocking;

		return HitStatus::NotHit;
	}*/
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
		if (value < 0.0f)
			value = 0.0f;
		else if (value > 1.0f)
			value = 1.0f;

		if (this->value != value)
		{
			this->value = value;
			renderUnitDirty = true;

			valueChangedEventDispatcher.Call({ *this });

			InvalidateHitStatus();
		}
	}
	void Slider::SetHandleSize(Vec2f handleSize)
	{
		if (handleSize.x < 1.0f)
			handleSize.x = 1.0f;
		if (handleSize.y < 1.0f)
			handleSize.y = 1.0f;

		this->handleSize = handleSize;
		renderUnitDirty = true;

		InvalidateHitStatus();
	}
	void Slider::SetTrackThickness(float thickness)
	{
		if (thickness < 1.0f)
			thickness = 1.0f;

		trackThickness = thickness;
		renderUnitDirty = true;
	}
	void Slider::PreRender(const RenderContext& renderContext)
	{
		if (!renderUnitDirty)
			return;

		renderUnitDirty = false;

		auto transform = GetFinalTransform();
		Vec2f right = Vec2f(Math::Cos(transform.rotation), Math::Sin(transform.rotation));
		Vec2f up = Vec2f(-right.y, right.x);

		trackRenderUnit.pos = transform.position + Vec2f(0, (transform.size.y - trackThickness) / 2);
		trackRenderUnit.right = right * transform.size.x;
		trackRenderUnit.up = up * trackThickness;

		if (handleSize.x < transform.size.x)
			handleRenderUnit.pos = transform.position + right * (transform.size.x - handleSize.x) * value + up * (transform.size.y - handleSize.y) / 2;
		else
			handleRenderUnit.pos = transform.position + right * (transform.size.x - handleSize.x) * 0.5f + up * (transform.size.y - handleSize.y) / 2;

		handleRenderUnit.right = right * handleSize.x;
		handleRenderUnit.up = up * handleSize.y;
	}
	RenderUnitBase* Slider::GetRenderUnit(uintMem index)
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
	void Slider::UpdateCursor()
	{
		if (highlighted || IsPressed())
			Input::SetCursorType(Input::CursorType::ResizeEW);
		else
			Input::SetCursorType(Input::CursorType::Default);
	}
	bool Slider::HitTestHandle(Vec2f mouseScreenPos)
	{
		auto transform = GetFinalTransform();
		Vec2f right = Vec2f(Math::Cos(transform.rotation), Math::Sin(transform.rotation));
		Vec2f up = Vec2f(-right.y, right.x);

		Vec2f nodePosition = mouseScreenPos - transform.position;

		nodePosition = {
			right.DotProduct(nodePosition),
			up.DotProduct(nodePosition),
		};


		Rectf handleHitbox;
		handleHitbox.size = {
			handleSize.x,
			std::max(handleSize.y, trackThickness)
		};
		handleHitbox.pos = {
			transform.size.x > handleHitbox.size.x ? (transform.size.x - handleHitbox.size.x) * value : (transform.size.x - handleHitbox.size.x) * 0.5f,
			(transform.size.y - handleHitbox.size.y) / 2
		};

		auto InRect = [](Vec2f point, Rectf rect) { return point.x >= rect.x && point.x < rect.x + rect.w && point.y >= rect.y && point.y < rect.y + rect.h; };
		return InRect(nodePosition, handleHitbox);
	}
	void Slider::FinalTransformUpdatedEvent(const Node::FinalTransformUpdatedEvent& event)
	{
		renderUnitDirty = true;
	}
	void Slider::MouseHitStatusChangedEvent(const UIMouseHitStatusChangedEvent& event)
	{
		highlighted = event.newHitStatus != HitStatus::NotHit;

		UpdateColor();
		UpdateCursor();
	}
	void Slider::PressedStateChangedEvent(const ButtonBase::PressedStateChangedEvent& event)
	{
		if (!event.mouseID.IsValid() || GetInputSubSystem() == nullptr)
			return;

		if (IsPressed())
		{
			Vec2f mousePos = GetInputSubSystem()->GetScreenMousePos(event.mouseID);

			if (HitTestHandle(mousePos))
			{
				auto transform = GetFinalTransform();
				Vec2f right = Vec2f(Math::Cos(transform.rotation), Math::Sin(transform.rotation));
				draggingMouseOffset = right.DotProduct(mousePos - transform.position) - value * (transform.size.x - handleSize.x);

				UpdateCursor();
			}
			else
			{
				auto transform = GetFinalTransform();
				Vec2f right = Vec2f(Math::Cos(transform.rotation), Math::Sin(transform.rotation));
				float newValue = (right.DotProduct(mousePos - transform.position) - handleSize.x / 2) / (transform.size.x - handleSize.x);

				SetValue(newValue);
			}

			UpdateColor();
		}
		else
		{
			UpdateCursor();
			UpdateColor();
		}
	}
	void Slider::MouseMotionEvent(const UIMouseMotionEvent& event)
	{
		if (IsPressed() && event.mouseID == GetPressingMouseID())
		{
			auto transform = GetFinalTransform();

			if (transform.size.x < handleSize.x)
				SetValue(0.5);
			else
			{
				Vec2f right = Vec2f(Math::Cos(transform.rotation), Math::Sin(transform.rotation));
				float newValue = (right.DotProduct(event.pos - transform.position) - draggingMouseOffset) / (transform.size.x - handleSize.x);

				SetValue(newValue);
			}
		}
	}
}