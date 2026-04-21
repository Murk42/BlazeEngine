#include "pch.h"
#include "BlazeEngine/UI/Nodes/PanelButton.h"
#include "BlazeEngine/Runtime/IO/Input.h"

namespace Blaze::UI::Nodes
{
	PanelButton::PanelButton()
	{
		dataMap.SetTypeName("PanelButton");
	}
	PanelButton::PanelButton(Node& parent, const NodeTransform& transform, const PanelButtonStyle& style)
		: PanelButton()
	{
		SetStyle(style);
		SetTransform(transform);
		SetParent(&parent);
	}
	PanelButton::~PanelButton()
	{
	}
	void PanelButton::SetPressedCallback(const std::function<void()>& callback)
	{
		this->pressedCallback = callback;
	}
	void PanelButton::SetStyle(const PanelButtonStyle& style)
	{
		this->style = style;

		renderUnit.style = style.panelStyle;

		UpdateColor();
	}
	PanelButtonStyle PanelButton::GetStyle() const
	{
		return style;
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
		if (IsDown())
		{
			renderUnit.style.fillColor = Mix(style.panelStyle.fillColor, style.pressedTint);
			renderUnit.style.borderColor = Mix(style.panelStyle.borderColor, style.pressedTint);
		}
		else if (IsMouseOver())
		{
			renderUnit.style.fillColor = Mix(style.panelStyle.fillColor, style.highlightTint);
			renderUnit.style.borderColor = Mix(style.panelStyle.borderColor, style.highlightTint);
		}
		else
		{
			renderUnit.style.fillColor = style.panelStyle.fillColor;
			renderUnit.style.borderColor = style.panelStyle.borderColor;
		}
	}
	void PanelButton::IsDownChanged(bool byMouse, Vec2f pos)
	{
		UpdateColor();
	}
	void PanelButton::IsMouseOverChanged(Vec2f pos)
	{
		UpdateColor();

		if (IsMouseOver())
			Input::SetCursorType(Input::CursorType::Pointer);
		else
			Input::SetCursorType(Input::CursorType::Default);
	}
	void PanelButton::Pressed()
	{
		if (pressedCallback)
			pressedCallback();
	}
	void PanelButton::FinalTransformUpdatedEvent(const Node::FinalTransformUpdatedEvent& event)
	{
		NodeFinalTransform transform = GetFinalTransform();
		renderUnit.pos = transform.position;
		renderUnit.right = transform.Right() * transform.size.x;
		renderUnit.up = transform.Up() * transform.size.y;
	}
}