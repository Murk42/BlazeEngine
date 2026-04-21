#include "pch.h"
#include "BlazeEngine/UI/Nodes/Slider.h"
#include "BlazeEngine/Runtime/IO/Input.h"

namespace Blaze::UI::Nodes
{
	Slider::Slider()
		: renderUnitDirty(true), handleSize(30, 12), trackThickness(1), value(0.0f), draggingMouseOffset(0)
	{
		dataMap.SetTypeName("Slider");
		
		SetTransform({ .size = { 200, 20 } });

		finalTransformUpdatedEventDispatcher.AddHandler<&Slider::FinalTransformUpdatedEvent>(*this);
		mouseHitStatusChangedEventDispatcher.AddHandler<&Slider::MouseHitStatusChangedEvent>(*this);
		mouseMotionEventDispatcher.AddHandler<&Slider::MouseMotionEvent>(*this);
	}
	Slider::Slider(Node& parent, const NodeTransform& transform, float value, Vec2f handleSize, float trackThickness, const SliderStyle& style)
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
	void Slider::SetStyle(const SliderStyle& style)
	{
		this->style = style;
			
		handleRenderUnit.style = style.handleStyle;
		trackRenderUnit.style = style.trackStyle;

		UpdateColor();
	}
	SliderStyle Slider::GetStyle() const
	{
		return style;
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

			if (valueChangedCallback)
				valueChangedCallback(value);
			valueChangedEventDispatcher.Call({ *this });

			InvalidateHitTest();
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

		InvalidateHitTest();
	}
	void Slider::SetTrackThickness(float thickness)
	{
		if (thickness < 1.0f)
			thickness = 1.0f;

		trackThickness = thickness;
		renderUnitDirty = true;
	}
	void Slider::SetValueChangedCallback(std::function<void(float)> valueChangedCallback)
	{
		this->valueChangedCallback = valueChangedCallback;
	}
	bool Slider::PreRender(const Graphics::RenderContext& renderContext)
	{
		if (!renderUnitDirty)
			return false;

		renderUnitDirty = false;

		UpdateTrackRenderUnit();
		UpdateHandleRenderUnit();

		return false;
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
	Vec2f Slider::CalculateHandlePos() const
	{
		const auto& transform = GetTransform();

		float slideLength = transform.size.x - handleSize.x;
			
		Vec2f pos{
			slideLength > 0.0f ? slideLength * value : slideLength * 0.5f,
			(transform.size.y - handleSize.y) / 2
		};
		
		return pos;
	}
	void Slider::UpdateColor()
	{
		if (IsDown())
		{
			handleRenderUnit.style.fillColor = Mix(style.handleStyle.fillColor, style.pressedTint);
			handleRenderUnit.style.borderColor = Mix(style.handleStyle.borderColor, style.pressedTint);
		}
		else if (IsMouseOver())
		{
			handleRenderUnit.style.fillColor = Mix(style.handleStyle.fillColor, style.highlightTint);
			handleRenderUnit.style.borderColor = Mix(style.handleStyle.borderColor, style.highlightTint);
		}
		else
		{
			handleRenderUnit.style.fillColor = style.handleStyle.fillColor;
			handleRenderUnit.style.borderColor = style.handleStyle.borderColor;
		}
	}
	void Slider::UpdateHandleRenderUnit()
	{
		const auto& finalTransform = GetFinalTransform();

		Vec2f finalHandleSize = handleSize * GetFinalScale();

		handleRenderUnit.pos = TransformFromLocalToFinalSpace(CalculateHandlePos());
		handleRenderUnit.right = finalTransform.Right() * finalHandleSize.x;
		handleRenderUnit.up = finalTransform.Up() * finalHandleSize.y;
	}
	void Slider::UpdateTrackRenderUnit()
	{
		const auto& finalTransform = GetFinalTransform();

		trackRenderUnit.pos = finalTransform.position + Vec2f(0, (finalTransform.size.y - trackThickness) / 2);
		trackRenderUnit.right = finalTransform.Right() * finalTransform.size.x;
		trackRenderUnit.up = finalTransform.Up() * trackThickness * GetFinalScale();
	}
	void Slider::SetValueForHandleRelativePos(Vec2f handleRelativePos)
	{
		const auto& transform = GetTransform();
		if (transform.size.x - handleSize.x > 0.0f)
			SetValue(value + (handleRelativePos.x - draggingMouseOffset) / (transform.size.x - handleSize.x));
	}
	bool Slider::HitTestHandle(Vec2f mouseScreenPos, Vec2f* handleRelativePosOut)
	{
		Vec2f handleRelativePos = TransformFromFinalToLocalSpace(mouseScreenPos) - CalculateHandlePos();

		if (handleRelativePosOut != nullptr)
			*handleRelativePosOut = handleRelativePos;

		return handleRelativePos.x >= 0 && handleRelativePos.y >= 0 && handleRelativePos.x < handleSize.x && handleRelativePos.y < handleSize.y;
	}
	void Slider::IsDownChanged(bool byMouse, Vec2f pos)
	{
		if (IsDown())
		{	
			UpdateColor();

			if (!byMouse)
				return;			

			Vec2f handleRelativePos;
			if (HitTestHandle(pos, &handleRelativePos))
				draggingMouseOffset = handleRelativePos.x;
			else
			{
				draggingMouseOffset = handleSize.x / 2;
				SetValueForHandleRelativePos(handleRelativePos);
			}
		}
		else
		{
			UpdateColor();

			if (!IsMouseOver())
				Input::SetCursorType(Input::CursorType::Default);
		}
	}
	void Slider::FinalTransformUpdatedEvent(const Node::FinalTransformUpdatedEvent& event)
	{
		renderUnitDirty = true;
	}
	void Slider::MouseHitStatusChangedEvent(const UIMouseHitStatusChangedEvent& event)
	{		
		UpdateColor();

		if (IsDown() || IsMouseOver())
			Input::SetCursorType(Input::CursorType::ResizeEW);
		else
			Input::SetCursorType(Input::CursorType::Default);
	}
	void Slider::MouseMotionEvent(const UIMouseMotionEvent& event)
	{
		if (IsDown() && event.mouseID == GetPressingMouseID())
		{
			Vec2f handleLocalPos = TransformFromFinalToLocalSpace(event.pos) - CalculateHandlePos();
			SetValueForHandleRelativePos(handleLocalPos);
		}
	}
}