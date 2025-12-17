#include "pch.h"
#include "UIDebugLayer.h"
#include "LayoutControl.h"

class UIDebugLayer;

class ClientUIDebugScreen : public UI::Screen
{
public:
	class LabeledButton : public UI::Nodes::EmptyNode
	{
	public:
		UI::Nodes::PanelButton button;
		UI::Layout::ParentSizeBinding buttonSizeBinding{ &button };

		UI::Nodes::Label label;

		LabeledButton(UI::Node& parent, UI::NodeTransform transform, ResourceRef<UI::FontAtlas> atlas, u8StringView label, const UI::Nodes::PanelButton::Style& buttonStyle) :
			EmptyNode(parent, transform),
			button(*this, { .size = transform.size }, buttonStyle),
			label(button, { .pos = { 0, 3}, .parentPivot = {0.5, 0.5}, .pivot = {0.5, 0.5} }, label, * atlas)
		{
			this->label.SetBlocksHitTestFlag(false);
		}
	};

	ResourceManager& resourceManager;

	ResourceRef<UI::FontFace> fontFace = resourceManager.LoadResource<UI::FontFace>("font face", UI::FontFace::LoadDefault());;
	ResourceRef<UI::FontAtlas> fontAtlas32 = resourceManager.LoadResource<UI::FontAtlas>("fontAtlas32", *fontFace, 32.0f, fontFace->GetAllGlyphsInidices(), UI::FontGlyphRenderers::AntialiasedFontGlyphRenderer(), resourceManager);
	ResourceRef<UI::FontAtlas> fontAtlas20 = resourceManager.LoadResource<UI::FontAtlas>("fontAtlas20", *fontFace, 20.0f, fontFace->GetAllGlyphsInidices(), UI::FontGlyphRenderers::AntialiasedFontGlyphRenderer(), resourceManager);

	UI::Nodes::Panel::Style panelStyle { .fillColor = 0x00000080, .borderColor = 0x0, .borderWidth = 0, .cornerRadius = 0 };
	UI::Nodes::PanelButton::Style buttonPanelStyle{ .panelStyle = {.fillColor = 0x404040ff, .borderColor = 0x0, .borderWidth = 0, .cornerRadius = 0 } };

	UI::Nodes::Label titleLabel{ *this, {.pos = {0, 15}, .parentPivot = {0.5, 0}, .pivot = {0.5, 0}} };
	UI::Nodes::PanelButton toggleButton{ titleLabel, {.pos = {10, -5}, .parentPivot = {1, 0.5}, .pivot = {0, 0.5}, .size { 40, 40 }}, buttonPanelStyle };

	UI::Nodes::EmptyNode HUD{ *this, {  } };
	UI::Layout::ParentSizeBinding HUDParentSizeBinding{ &HUD };

	UI::Nodes::Panel mouseInfoPanel{ HUD, {.pos = {3, -3}, .parentPivot = {0, 1}, .pivot = {0, 1}, .size = { 300, 100 }}, panelStyle };
	UI::Nodes::Label mouseInfo{ mouseInfoPanel, {.pos = {3, -3}, .parentPivot = {0, 1}, .pivot = {0, 1}, .size = { 0, 0 }} };
	//UI::Layout::ParentSizeBinding mouseInfoPanelSizeBinding{ &mouseInfoPanel };

	ClientUIDebugScreen(ResourceManager& resourceManager)
		: resourceManager(resourceManager)
	{
		titleLabel.SetBlocksHitTestFlag(false);
		mouseInfo.SetBlocksHitTestFlag(false);

		HUD.Disable();

		toggleButton.SetPressedEventCallback([&](const UI::Nodes::PanelButton::PressedEvent& event) { HUD.SetEnabledFlag(!HUD.GetNodeEnabledFlag()); });
	}

	void HUDEnabledStateChanged(const EnabledStateChangedEvent& event)
	{
		if (HUD.IsEnabled())
		{
			titleLabel.BuildText("Debug layer - ON", *fontAtlas32);
			titleLabel.SetColor(0xffff0055);
		}
		else
		{
			titleLabel.BuildText("Debug layer - OFF", *fontAtlas32);
			titleLabel.SetColor(0xffff0055);
		}
	}
	EVENT_MEMBER_FUNCTION(ClientUIDebugScreen, HUDEnabledStateChanged, HUD.enabledStateChangedEventDispatcher);

	void SetPointerData(const Map<Input::MouseID, UI::PointerData>& pointerData)
	{
		String string;
		for (auto& pair : pointerData)
		{
			StringView mouseName = Input::GetMouseName(pair.key);
			if (mouseName.Empty())
				mouseName = "Unnamed";
			string += Format("Mouse \"{}\" (id {})\n", mouseName, StringView::Parse(pair.key.GetNumberID()));
		}

		mouseInfo.BuildWrappedText(string, *fontAtlas20, 500.0f);
		mouseInfo.SetColor(0xf5f5f5ff);
	}
};

ClientUIDebugLayer::ClientUIDebugLayer(UI::System* UISystemToDebug)
	: UISystemToDebug(nullptr)
{
	UISystem.SetScreen<ClientUIDebugScreen>(resourceManager);
	SetUISystemToDebug(UISystemToDebug);
}
ClientUIDebugLayer::~ClientUIDebugLayer()
{
	SetUISystemToDebug(nullptr);
}
void ClientUIDebugLayer::SetUISystemToDebug(UI::System* newUISystemToDebug)
{
	if (UISystemToDebug == newUISystemToDebug)
		return;

	if (UISystemToDebug != nullptr)
	{
		UISystemToDebug->windowChangedEventDispatcher.RemoveHandler<&ClientUIDebugLayer::WindowChangedEvent>(*this);
		UISystemToDebug->destructionEventDispatcher.RemoveHandler<&ClientUIDebugLayer::UISystemDestructionEvent>(*this);
		UISystem.SetWindow(nullptr);
		UISystem.SetRendererRegistry({});
	}

	UISystemToDebug = newUISystemToDebug;

	if (UISystemToDebug != nullptr)
	{
		UISystem.SetWindow(UISystemToDebug->GetWindow());
		UISystem.SetRendererRegistry(UISystemToDebug->GetGraphicsSubSystem().GetRendererRegistry());
		UISystemToDebug->windowChangedEventDispatcher.AddHandler<&ClientUIDebugLayer::WindowChangedEvent>(*this);
		UISystemToDebug->destructionEventDispatcher.AddHandler<&ClientUIDebugLayer::UISystemDestructionEvent>(*this);
	}
}
void ClientUIDebugLayer::Update()
{
	if (UISystemToDebug != nullptr)
	{
		static_cast<ClientUIDebugScreen*>(UISystem.GetScreen())->SetPointerData(UISystemToDebug->GetInputSubSystem().GetPointerMap());
	}
}
bool ClientUIDebugLayer::OnEvent(const Input::GenericInputEvent& event, bool isProcessed)
{
	return UISystem.ProcessInputEvent(event, isProcessed);
}
void ClientUIDebugLayer::Render()
{
	UISystem.Render();
}
void ClientUIDebugLayer::WindowChangedEvent(const UI::System::WindowChangedEvent& event)
{
	UISystem.SetWindow(event.system.GetWindow());
}
void ClientUIDebugLayer::UISystemDestructionEvent(const UI::System::DestructionEvent& event)
{
	SetUISystemToDebug(nullptr);
}