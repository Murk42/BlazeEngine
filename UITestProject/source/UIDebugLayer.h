#pragma once
#include "BlazeEngine/UI/Common/System.h"

class UIDebugLayer : public AppLayer
{
public:
	UIDebugLayer(Graphics::GraphicsContextBase& graphicsContext, UI::System* UISystemToDebug);
	~UIDebugLayer();

	void SetUISystemToDebug(UI::System* newUISystemToDebug);

	void Update() override;
	Input::EventProcessedState OnEvent(const Input::GenericInputEvent& event, bool processed) override;
	void Render() override;

	void WindowChangedEvent(const UI::System::WindowChangedEvent& event);
	void UISystemDestructionEvent(const UI::System::DestructionEvent& event);
private:
	ResourceManager resourceManager;
	UI::System* UISystemToDebug;
	UI::System UISystem;

	ResourceRef<UI::FontManager> fontManager{ resourceManager, "fontManager", resourceManager };
};