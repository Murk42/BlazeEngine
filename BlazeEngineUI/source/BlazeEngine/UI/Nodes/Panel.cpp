#include "pch.h"
#include "BlazeEngine/UI/Nodes/Panel.h"

namespace Blaze::UI::Nodes
{
	Panel::Panel()
		: renderUnitDirty(true)
	{
		dataMap.SetTypeName("Panel");

		finalTransformUpdatedEventDispatcher.AddHandler<&Panel::OnEvent>(*this);
	}
	Panel::Panel(Node& parent, const NodeTransform& transform, const Style& style)
		: Panel()
	{
		SetParent(&parent);
		SetTransform(transform);
		SetStyle(style);
	}
	Panel::~Panel()
	{
		finalTransformUpdatedEventDispatcher.RemoveHandler<&Panel::OnEvent>(*this);
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
	void Panel::PreRender(const UIRenderContext& renderContext)
	{
		CleanFinalTransform();

		if (!renderUnitDirty)
			return;

		renderUnitDirty = false;

		GetFinalTransform().GetRectVectors(renderUnit.pos, renderUnit.right, renderUnit.up);
	}
	UIRenderUnitBase* Panel::GetRenderUnit(uintMem index)
	{
		if (index == 0)
			return &renderUnit;
		return nullptr;
	}
	void Panel::OnEvent(const Node::FinalTransformUpdatedEvent& event)
	{
		renderUnitDirty = true;
	}
}