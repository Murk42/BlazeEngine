#include "pch.h"
#include "BlazeEngine/UI/Debug/UIDebugScreen.h"
#include "BlazeEngine/Runtime/IO/Input.h"

namespace Blaze
{
	UIDebugScreen::UIDebugScreen(UI::FontManager& fontManager)
		: fontManager(fontManager)
	{
		titleLabel.SetBlocksHitTestFlag(false);
		mouseInfo.SetBlocksHitTestFlag(false);
		mouseInfoPanel.SetBlocksHitTestFlag(false);

		HUD.Disable();

		toggleButton.SetPressedCallback([&]() { HUD.SetEnabledFlag(!HUD.GetNodeEnabledFlag()); });
	}
	inline void UIDebugScreen::HUDEnabledStateChanged(const NodeEnabledStateChangedEvent& event)
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
	void UIDebugScreen::SetPointerData(const Map<Input::MouseID, UI::PointerData>& pointerData)
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
}