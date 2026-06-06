#pragma once
#include "BlazeEngine/UI/Core/Screen.h"
#include "BlazeEngine/UI/Nodes/EmptyNode.h"
#include "BlazeEngine/UI/Nodes/Label.h"
#include "BlazeEngine/UI/Nodes/Panel.h"
#include "BlazeEngine/UI/Nodes/PanelButton.h"
#include "BlazeEngine/UI/Input/InputSubSystem.h"

namespace Blaze
{
	class UIDebugScreen : public UI::Screen
	{
	public:		
		UI::FontManager& fontManager;

		UI::Nodes::TextStyle textStyle0{ .fontManager = &fontManager, .fontName = "default", .fontHeight = 32 };
		UI::Nodes::TextStyle textStyle1{ .fontManager = &fontManager, .fontName = "default", .fontHeight = 20 };
		UI::Nodes::TextStyle textStyle2{ .fontManager = &fontManager, .fontName = "default", .fontHeight = 12 };
		
		UI::Nodes::PanelButtonStyle buttonPanelStyle{ .panelStyle = {.fillColor = 0x404040ff_rgba, .borderColor = 0x00000000_rgba, .borderWidth = 0, .cornerRadius = 0 }, .highlightTint = 0xffffff30_rgba, .pressedTint = 0x00000050_rgba };

		UI::Nodes::PanelStyle panelStyle{ .fillColor = 0x000000B0_rgba, .borderColor = 0x00000000_rgba, .borderWidth = 0, .cornerRadius = 0 };

		UI::Nodes::Label titleLabel{ *this, {.position = {0, 10}, .parentPivot = {0.5, 0}, .pivot = {0.5, 0}}, textStyle0 };
		UI::Nodes::PanelButton toggleButton{ titleLabel, {.position = {10, 0}, .parentPivot = {1, 0.5}, .pivot = {0, 0.5}, .size { 40, 40 }}, buttonPanelStyle };

		UI::Nodes::EmptyNode HUD{ *this, {  } };

		UI::Nodes::Panel mouseInfoPanel{ *this, {.position = {3, -3}, .parentPivot = {0, 1}, .pivot = {0, 1}, .size = { 300, 100 }}, {.fillColor = 0x000000D0_rgba, .borderColor = 0xffffffff_rgba, .borderWidth = 1, .cornerRadius = 0 } };
		UI::Nodes::Label mouseInfo{ mouseInfoPanel, {.position = {3, -3}, .parentPivot = {0, 1}, .pivot = {0, 1}, .size = { 0, 0 }},  textStyle2 };
		
		UIDebugScreen(UI::FontManager& fontManager);

		void HUDEnabledStateChanged(const NodeEnabledStateChangedEvent& event);
		EVENT_MEMBER_FUNCTION(UIDebugScreen, HUDEnabledStateChanged, HUD.enabledStateChangedEventDispatcher);

		void SetPointerData(const Map<Input::MouseID, UI::PointerData>& pointerData);
	};
}