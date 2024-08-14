#include "pch.h"
#include "BlazeEngineGraphics/UI/Elements/PanelRenderUnit.h"
#include "BlazeEngineGraphics/UI/Nodes/Panel.h"

namespace Blaze::UIGraphics
{
	PanelNodeRenderUnit::PanelNodeRenderUnit(UI::Node* node)
		: RenderUnit("PanelRenderer"), node(node), rendered(true), rd({
			.pos = Vec2f(0, 0),
			.size = Vec2f(0, 0),
			.rotation = 0,
			.borderWidth = 1,
			.cornerRadius = 5,
			.fillColor = 0x000000ff,
			.borderColor = 0x222222ff,
			})
	{		
		node->finalTransformUpdatedEventDispatcher.AddHandler(*this);
	}
	void PanelNodeRenderUnit::BeginStream()
	{
		node->CleanFinalTransform();
		rendered = false;
	}	
	void* PanelNodeRenderUnit::Advance()
	{
		if (rendered)
			return nullptr;

		rendered = true;		

		return &rd;
	}
	void PanelNodeRenderUnit::SetFillColor(ColorRGBAf color)
	{
		rd.fillColor = color;
	}
	void PanelNodeRenderUnit::SetBorderColor(ColorRGBAf color)
	{
		rd.borderColor = color;
	}
	void PanelNodeRenderUnit::SetCornerRadius(float radius)
	{ 
		rd.cornerRadius = radius;
	}
	void PanelNodeRenderUnit::SetBorderWidth(float width)
	{
		rd.borderWidth = width;
	}
	void PanelNodeRenderUnit::OnEvent(UI::Node::FinalTransformUpdatedEvent event)
	{
		rd.pos = event.finalTransform.position;
		rd.size = event.finalTransform.size;
		rd.rotation = event.finalTransform.rotation;
	}
}