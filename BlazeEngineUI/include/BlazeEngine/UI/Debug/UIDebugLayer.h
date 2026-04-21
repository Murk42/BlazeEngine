#pragma once
#include "BlazeEngine/Graphics/App/GraphicsAppLayer.h"
#include "BlazeEngine/UI/Common/System.h"
#include "BlazeEngine/UI/Text/FontManager.h"

namespace Blaze
{
	class UIDebugScreen;

	class UIDebugLayer : public GraphicsAppLayer
	{
	public:
		UIDebugLayer();
		~UIDebugLayer();

		void SetupGraphics(const GraphicsAppRuntimeThreadData& data) override;

		void SetUISystemToDebug(UI::System* newUISystemToDebug);

		void Update() override;
		Input::EventProcessedState OnEvent(const Input::GenericInputEvent& event, bool processed) override;
		void Render(const Graphics::RenderContext& renderContext) override;

		void WindowChangedEvent(const UI::System::WindowChangedEvent& event);
		void UISystemDestructionEvent(const UI::System::DestructionEvent& event);
	private:
		UI::FontManager fontManager{ resourceManager };

		Handle<UIDebugScreen> screen;
		UI::System UISystem;

		UI::System* UISystemToDebug;

	};
}