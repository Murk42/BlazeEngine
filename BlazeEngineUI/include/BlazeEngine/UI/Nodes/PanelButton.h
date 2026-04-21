#pragma once
#include "BlazeEngine/Core/Event/EventMemberFunctionTie.h"
#include "BlazeEngine/UI/Common/ButtonBase.h"
#include "BlazeEngine/UI/Input/InputNode.h"
#include "BlazeEngine/UI/Graphics/RenderUnits/PanelRenderUnit.h"
#include "BlazeEngine/UI/Graphics/RenderableNode.h"
#include <functional>

namespace Blaze::UI::Nodes
{
	struct PanelButtonStyle
	{
		PanelStyle panelStyle = {};
		ColorRGBAf highlightTint = ColorRGBAf(0.2f, 0.2f, 0.2f, 0.4f);
		ColorRGBAf pressedTint = ColorRGBAf(0.1f, 0.1f, 0.1f, 0.5f);
	};
	class BLAZE_API PanelButton : public ButtonBase, public RenderableNode
	{
	public:
		PanelButton();
		PanelButton(Node& parent, const NodeTransform& transform, const PanelButtonStyle& style = { });
		~PanelButton();

		void SetPressedCallback(const std::function<void()>& callback);

		void SetStyle(const PanelButtonStyle& style);
		PanelButtonStyle GetStyle() const;
		
		RenderUnitBase* GetRenderUnit(uintMem index) override;
	private:
		PanelRenderUnit renderUnit;
		PanelButtonStyle style;

		std::function<void()> pressedCallback;

		void UpdateColor();

		void Pressed() override;
		void IsDownChanged(bool byMouse, Vec2f pos) override;
		void IsMouseOverChanged(Vec2f pos) override;

		void FinalTransformUpdatedEvent(const Node::FinalTransformUpdatedEvent& event);
		EVENT_MEMBER_FUNCTION(PanelButton, FinalTransformUpdatedEvent, finalTransformUpdatedEventDispatcher);		
	};
}