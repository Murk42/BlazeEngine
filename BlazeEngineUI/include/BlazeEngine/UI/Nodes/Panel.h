#pragma once
#include "BlazeEngine/Core/Event/EventMemberFunctionTie.h"
#include "BlazeEngine/UI/Graphics/RenderUnits/PanelRenderUnit.h"
#include "BlazeEngine/UI/Graphics/RenderableNode.h"

namespace Blaze::UI::Nodes
{
	class BLAZE_API Panel :
		public Node,
		public RenderableNode
	{
	public:
		Panel();
		Panel(Node& parent, const NodeTransform& transform, const PanelStyle& style = {});
		~Panel();

		void SetStyle(const PanelStyle& style);
		PanelStyle GetStyle() const;

		void SetBlocksHitTestFlag(bool blocksHitTest);
		inline bool GetBlocksHitTestFlag(bool blocksHitTest) { return blocksHitTest; }

		RenderUnitBase* GetRenderUnit(uintMem index) override;

		HitStatus HitTest(Vec2f screenPosition) override;
	private:
		PanelRenderUnit renderUnit;
		bool blocksHitTest;

		void FinalTransformUpdatedEvent(const Node::FinalTransformUpdatedEvent& event);
		EVENT_MEMBER_FUNCTION(Panel, FinalTransformUpdatedEvent, finalTransformUpdatedEventDispatcher);
	};
}