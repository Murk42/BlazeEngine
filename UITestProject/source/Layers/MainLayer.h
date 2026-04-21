#pragma once
#include "Screens/MainScreen.h"

class AppThread;

class MainLayer : public GraphicsAppLayer, public UIDebuggableLayer
{
public:
	MainLayer();
	~MainLayer() override;

	void SetupGraphics(const GraphicsAppRuntimeThreadData& data) override;
	UI::System* GetUISystem() override { return &UISystem; }
	
	void Update() override;
	Input::EventProcessedState OnEvent(const Input::GenericInputEvent& event, bool processed) override;

	void Render(const Graphics::RenderContext& renderContext) override;
private:
	UI::FontManager fontManager{ resourceManager };

	MainScreen screen{ resourceManager, fontManager };
	UI::System UISystem{ screen };
};