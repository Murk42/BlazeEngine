#pragma once
#include "BlazeEngine/UI/Graphics/RenderUnits/PanelRenderUnit.h"
#include "BlazeEngine/UI/Graphics/RenderNode.h"

namespace Blaze::UI::Nodes
{
	class BLAZE_API Panel :
		public Node,
		public RenderNode
	{
	public:
		struct Style
		{
			ColorRGBAf fillColor = 0x111111ff;
			ColorRGBAf borderColor = 0xeeeeeeff;
			float borderWidth = 1;
			float cornerRadius = 10;
		};

		Panel();
		Panel(Node& parent, const NodeTransform& transform, const Style& style);
		~Panel();

		void SetStyle(const Style& style);
		Style GetStyle() const;

		void PreRender(const UIRenderContext& renderContext) override;
		UIRenderUnitBase* GetRenderUnit(uintMem index) override;
	private:
		PanelRenderUnit renderUnit;
		bool renderUnitDirty;

		void OnEvent(const Node::FinalTransformUpdatedEvent& event);
	};
}