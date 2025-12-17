#include "pch.h"
#include "BlazeEngine/UI/Nodes/Panel.h"

namespace Blaze::UI::Nodes
{
	Panel::Panel()
	{
		dataMap.SetTypeName("Panel");
	}
	Panel::Panel(Node& parent, const NodeTransform& transform, const Style& style)
		: Panel()
	{
		SetStyle(style);
		SetTransform(transform);
		SetParent(&parent);
	}
	Panel::~Panel()
	{
	}
	void Panel::SetStyle(const Panel::Style& style)
	{
		renderUnit.fillColor = style.fillColor;
		renderUnit.borderColor = style.borderColor;
		renderUnit.borderWidth = style.borderWidth;
		renderUnit.cornerRadius = style.cornerRadius;
	}
	Panel::Style Panel::GetStyle() const
	{
		return {
			.fillColor = renderUnit.fillColor,
			.borderColor = renderUnit.borderColor,
			.borderWidth = renderUnit.borderWidth,
			.cornerRadius = renderUnit.cornerRadius
		};
	}
	void Panel::PreRender(const RenderContext& renderContext)
	{
		auto finalTransform = GetFinalTransform();
		renderUnit.pos = finalTransform.position;
		renderUnit.right = finalTransform.Right() * finalTransform.size.x;
		renderUnit.up = finalTransform.Up() * finalTransform.size.y;
	}
	RenderUnitBase* Panel::GetRenderUnit(uintMem index)
	{
		if (index == 0)
			return &renderUnit;
		return nullptr;
	}
}