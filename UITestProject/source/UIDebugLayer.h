#pragma once
#include "BlazeEngine/Runtime/App/AppLayerCreationData.h"
#include "BlazeEngine/UI/Common/System.h"

class ClientUIDebugLayer : public AppLayer
{
public:
	ClientUIDebugLayer(UI::System* UISystemToDebug);
	~ClientUIDebugLayer();

	void SetUISystemToDebug(UI::System* newUISystemToDebug);

	void Update() override;
	bool OnEvent(const Input::GenericInputEvent& event, bool isProcessed);
	void Render() override;

	void WindowChangedEvent(const UI::System::WindowChangedEvent& event);
	void UISystemDestructionEvent(const UI::System::DestructionEvent& event);
private:
	ResourceManager resourceManager;
	UI::System* UISystemToDebug;
	UI::System UISystem;
};