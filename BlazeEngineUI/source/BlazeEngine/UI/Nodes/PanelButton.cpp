#include "pch.h"
#include "BlazeEngine/UI/Nodes/PanelButton.h"
#include "BlazeEngine/Runtime/IO/Input.h"

namespace Blaze::UI::Nodes
{
	PanelButton::PanelButton()
		: highlighted(false), renderUnitDirty(true)
	{
		dataMap.SetTypeName("PanelButton");

		fillColor = renderUnit.fillColor;
		borderColor = renderUnit.borderColor;
		highlightTint = ColorRGBAf(1, 1, 1, 0.04f);
		pressedTint = ColorRGBAf(0, 0, 0, 0.4f);

		finalTransformUpdatedEventDispatcher.AddHandler<&PanelButton::FinalTransformUpdatedEvent>(*this);
		mouseHitStatusChangedEventDispatcher.AddHandler<&PanelButton::MouseHitStatusChangedEvent>(*this);
		pressedStateChangedEventDispatcher.AddHandler<&PanelButton::PressedStateChangedEvent>(*this);

	}
	PanelButton::PanelButton(Node& parent, const NodeTransform& transform, const Style& style)
		: PanelButton()
	{
		SetStyle(style);
		SetTransform(transform);
		SetParent(&parent);
	}
	PanelButton::~PanelButton()
	{
		finalTransformUpdatedEventDispatcher.RemoveHandler<&PanelButton::FinalTransformUpdatedEvent>(*this);
		mouseHitStatusChangedEventDispatcher.RemoveHandler<&PanelButton::MouseHitStatusChangedEvent>(*this);
		pressedStateChangedEventDispatcher.RemoveHandler<&PanelButton::PressedStateChangedEvent>(*this);
	}
	void PanelButton::SetStyle(const Style& style)
	{
		fillColor = style.panelStyle.fillColor;
		borderColor = style.panelStyle.borderColor;
		renderUnit.cornerRadius = style.panelStyle.cornerRadius;
		renderUnit.borderWidth = style.panelStyle.borderWidth;
		highlightTint = style.highlightTint;
		pressedTint = style.pressedTint;
		UpdateColor();
	}
	PanelButton::Style PanelButton::GetStyle() const
	{
		return Style{
			.panelStyle = {
				.fillColor = fillColor,
				.borderColor = borderColor,
				.borderWidth = renderUnit.borderWidth,
				.cornerRadius = renderUnit.cornerRadius,
			},
			.highlightTint = highlightTint,
			.pressedTint = pressedTint
		};
	}
	void PanelButton::PreRender(const RenderContext& renderContext)
	{
		CleanFinalTransform();

		if (!renderUnitDirty)
			return;

		renderUnitDirty = false;

		NodeFinalTransform transform = GetFinalTransform();
		renderUnit.pos = transform.position;
		renderUnit.right = transform.Right() * transform.size.x;
		renderUnit.up= transform.Up() * transform.size.y;
	}
	RenderUnitBase* PanelButton::GetRenderUnit(uintMem index)
	{
		return index == 0 ? &renderUnit : nullptr;
	}
	static float Mix(float a, float b, float t)
	{
		return b * t + a * (1 - t);
	}
	static ColorRGBAf Mix(ColorRGBAf a, ColorRGBAf b)
	{
		return { Mix(a.r, b.r, b.a), Mix(a.g, b.g, b.a), Mix(a.b, b.b, b.a), a.a };
	}
	void PanelButton::UpdateColor()
	{
		if (IsPressed())
		{
			renderUnit.fillColor = Mix(fillColor, pressedTint);
			renderUnit.borderColor = Mix(borderColor, pressedTint);
		}
		else if (highlighted)
		{
			renderUnit.fillColor = Mix(fillColor, highlightTint);
			renderUnit.borderColor = Mix(borderColor, highlightTint);
		}
		else
		{
			renderUnit.fillColor = fillColor;
			renderUnit.borderColor = borderColor;
		}
	}
	void PanelButton::FinalTransformUpdatedEvent(const Node::FinalTransformUpdatedEvent& event)
	{
		renderUnitDirty = true;
	}
	void PanelButton::MouseHitStatusChangedEvent(const UIMouseHitStatusChangedEvent& event)
	{
		highlighted = event.newHitStatus != Node::HitStatus::NotHit;

		UpdateColor();

		if (event.newHitStatus != Node::HitStatus::HitBlocking)
			Input::SetCursorType(Input::CursorType::Pointer);
		else
			Input::SetCursorType(Input::CursorType::Default);
	}
	void PanelButton::PressedStateChangedEvent(const ButtonBase::PressedStateChangedEvent& event)
	{
		UpdateColor();
	}
}