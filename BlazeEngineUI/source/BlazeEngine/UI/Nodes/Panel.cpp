#include "pch.h"
#include "BlazeEngine/UI/Nodes/Panel.h"

namespace Blaze::UI::Nodes
{
	Panel::Panel()
		: blocksHitTest(true)
	{
		dataMap.SetTypeName("Panel");
	}
	Panel::Panel(Node& parent, const NodeTransform& transform, const PanelStyle& style)
		: Panel()
	{
		SetStyle(style);
		SetTransform(transform);
		SetParent(&parent);
	}
	Panel::~Panel()
	{
	}
	void Panel::SetStyle(const PanelStyle& style)
	{
		renderUnit.style = style;
	}
	PanelStyle Panel::GetStyle() const
	{
		return renderUnit.style;
	}
	void Panel::SetBlocksHitTestFlag(bool blocksHitTest)
	{
		this->blocksHitTest = blocksHitTest;
	}
	RenderUnitBase* Panel::GetRenderUnit(uintMem index)
	{
		if (index == 0)
			return &renderUnit;
		return nullptr;
	}
	Node::HitStatus Panel::HitTest(Vec2f screenPosition)
	{
		return blocksHitTest ? Node::HitTest(screenPosition) : HitStatus::NotHit;
	}
	void Panel::FinalTransformUpdatedEvent(const Node::FinalTransformUpdatedEvent& event)
	{
		auto finalTransform = GetFinalTransform();
		renderUnit.pos = finalTransform.position;
		renderUnit.right = finalTransform.Right() * finalTransform.size.x;
		renderUnit.up = finalTransform.Up() * finalTransform.size.y;
	}
}