#include "pch.h"
#include "BlazeEngineGraphics/UI/Graphics/RenderUnits/PanelRenderUnit.h"

namespace Blaze::UI
{
	PanelRenderUnit::PanelRenderUnit(Node* node)
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
	PanelRenderUnit::~PanelRenderUnit()
	{
		node->finalTransformUpdatedEventDispatcher.RemoveHandler(*this);
	}
	void PanelRenderUnit::BeginStream()
	{
		node->CleanFinalTransform();
		rendered = false;
	}	
	void* PanelRenderUnit::Advance()
	{
		if (rendered)
			return nullptr;

		rendered = true;		

		return &rd;
	}
	void PanelRenderUnit::SetFillColor(ColorRGBAf color)
	{
		rd.fillColor = color;
	}
	void PanelRenderUnit::SetBorderColor(ColorRGBAf color)
	{
		rd.borderColor = color;
	}
	void PanelRenderUnit::SetCornerRadius(float radius)
	{ 
		rd.cornerRadius = radius;
	}
	void PanelRenderUnit::SetBorderWidth(float width)
	{
		rd.borderWidth = width;
	}
	void PanelRenderUnit::OnEvent(const Node::FinalTransformUpdatedEvent& event)
	{
		auto finalTransform = node->GetFinalTransform();
		rd.pos = finalTransform.position;
		rd.size = finalTransform.size;
		rd.rotation = finalTransform.rotation;
	}
}