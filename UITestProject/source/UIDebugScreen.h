#pragma once

class UIDebugScreen : public UI::Screen
{
public:
	class LabeledButton : public UI::Nodes::EmptyNode
	{
	public:
		UI::Nodes::PanelButton button;
		//UI::Layout::ParentSizeBinding buttonSizeBinding{ &button };

		UI::Nodes::Label label;

		LabeledButton(UI::Node& parent, UI::NodeTransform transform, u8StringView label, UI::TextStyle textStyle, const UI::Nodes::PanelButton::Style& buttonStyle) :
			EmptyNode(parent, transform),
			button(*this, { .size = transform.size }, buttonStyle),
			label(button, { .pos = { 0, 3}, .parentPivot = {0.5, 0.5}, .pivot = {0.5, 0.5} }, textStyle, label)
		{
			this->label.SetBlocksHitTestFlag(false);
		}
	};

	ResourceManager& resourceManager;

	UI::TextStyle textStyle0{ .fontName = "default", .fontHeight = 32.0f };
	UI::TextStyle textStyle1{ .fontName = "default", .fontHeight = 20.0f };

	UI::Nodes::Panel::Style panelStyle{ .fillColor = 0x000000B0, .borderColor = 0x0, .borderWidth = 0, .cornerRadius = 0 };
	UI::Nodes::PanelButton::Style buttonPanelStyle{ .panelStyle = {.fillColor = 0x404040ff, .borderColor = 0x0, .borderWidth = 0, .cornerRadius = 0 } };

	UI::Nodes::Label titleLabel{ *this, {.pos = {0, 10}, .parentPivot = {0.5, 0}, .pivot = {0.5, 0}}, textStyle0 };
	UI::Nodes::PanelButton toggleButton{ titleLabel, {.pos = {10, 0}, .parentPivot = {1, 0.5}, .pivot = {0, 0.5}, .size { 40, 40 }}, buttonPanelStyle };

	UI::Nodes::EmptyNode HUD{ *this, {  } };

	UI::Nodes::Panel mouseInfoPanel{ *this, {.pos = {3, -3}, .parentPivot = {0, 1}, .pivot = {0, 1}, .size = { 300, 100 }}, {.fillColor = 0x000000D0, .borderColor = 0xffffffff, .borderWidth = 1, .cornerRadius = 0 } };
	UI::Nodes::Label mouseInfo{ mouseInfoPanel, {.pos = {3, -3}, .parentPivot = {0, 1}, .pivot = {0, 1}, .size = { 0, 0 }},  textStyle1 };
	//UI::Layout::ParentSizeBinding mouseInfoPanelSizeBinding{ &mouseInfoPanel };

	UIDebugScreen(ResourceManager& resourceManager)
		: Screen(resourceManager), resourceManager(resourceManager)
	{
		titleLabel.SetBlocksHitTestFlag(false);
		mouseInfo.SetBlocksHitTestFlag(false);
		mouseInfoPanel.SetBlocksHitTestFlag(false);

		HUD.Disable();

		toggleButton.SetPressedEventCallback([&](const UI::Nodes::PanelButton::PressedEvent& event) { HUD.SetEnabledFlag(!HUD.GetNodeEnabledFlag()); });
	}

	void HUDEnabledStateChanged(const NodeEnabledStateChangedEvent& event)
	{
		if (HUD.IsEnabled())
		{
			titleLabel.SetText("Debug layer - ON");
			mouseInfoPanel.Enable();
			mouseInfo.Enable();
		}
		else
		{
			titleLabel.SetText("Debug layer - OFF");
			mouseInfoPanel.Disable();
			mouseInfo.Disable();
		}
	}
	EVENT_MEMBER_FUNCTION(UIDebugScreen, HUDEnabledStateChanged, HUD.enabledStateChangedEventDispatcher);

	void SetPointerData(const Map<Input::MouseID, UI::PointerData>& pointerData)
	{
		String string;
		if (pointerData.Empty())
		{
			string += "No pointer data";
		}
		else
		{
			for (auto& pair : pointerData)
			{
				StringView mouseName = Input::GetMouseName(pair.key);

				if (mouseName.Empty())
					mouseName = "Unnamed";

				string += Format("Mouse \"{}\" (id {})\n", mouseName, StringView::Parse(pair.key.GetNumberID()));

				UI::InputNode* capturingNode = pair.value.GetCapturingNode();

				string += Format("Pos: ({.1f}, {.1f})\n", pair.value.GetPos().x, pair.value.GetPos().y);

				if (capturingNode == nullptr)
					string += "Capturing node - none\n";
				else
					string += Format("Capturing node - {}\n", capturingNode->dataMap.GetTypeName());

				string += "Hit data:\n";

				if (pair.value.GetHitData().Empty())
				{
					string += "\tNo hits";
				}
				else
					for (auto& hitData : pair.value.GetHitData())
					{
						auto name = hitData.node.dataMap.GetName();
						StringView status = hitData.hitStatus == Node::HitStatus::NotHit ? StringView("no hit") : hitData.hitStatus == Node::HitStatus::HitNotBlocking ? StringView("hit not blocking") : StringView("hit blocking");

						if (name.Empty())
							string += Format("\t{} - {}\n", hitData.node.dataMap.GetTypeName(), status);
					}
			}
		}

		mouseInfo.SetText(string);
	}
};