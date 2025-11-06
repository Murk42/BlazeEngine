#pragma once
#include "BlazeEngine/UI/Common/ButtonBase.h"
#include "BlazeEngine/UI/Input/InputNode.h"
#include "BlazeEngine/UI/Nodes/Panel.h"
#include "BlazeEngine/UI/Graphics/RenderNode.h"

namespace Blaze::UI::Nodes
{
	class BLAZE_API PanelButton : public ButtonBase, public RenderNode
	{
	public:
		struct Style
		{
			Panel::Style panelStyle = {};
			ColorRGBAf highlightTint = ColorRGBAf(0.2f, 0.2f, 0.2f, 0.3f);
			ColorRGBAf pressedTint = ColorRGBAf(0.1f, 0.1f, 0.1f, 0.5f);
		};
		PanelButton();
		PanelButton(Node& parent, const NodeTransform& transform, const Style& style);
		~PanelButton();

		void SetStyle(const Style& style);
		Style GetStyle() const;

		void PreRender(const UIRenderContext& renderContext) override;
		UIRenderUnitBase* GetRenderUnit(uintMem index) override;
	private:
		PanelRenderUnit renderUnit;
		bool renderUnitDirty;

		ColorRGBAf fillColor;
		ColorRGBAf borderColor;
		ColorRGBAf highlightTint;
		ColorRGBAf pressedTint;

		bool highlighted;

		void UpdateColor();

		void FinalTransformUpdatedEvent(const Node::FinalTransformUpdatedEvent& event);
		void MouseHitStatusChangedEvent(const MouseHitStatusChangedEvent& event);
		void PressedStateChangedEvent(const PressedStateChangedEvent& event);
	};
}