#include "pch.h"
#include "BlazeEngine/UI/Debug/UIDebugLayer.h"
#include "BlazeEngine/UI/Debug/UIDebuggableLayer.h"
#include "BlazeEngine/UI/Debug/UIDebugScreen.h"
#include "BlazeEngine/UI/Graphics/Renderers/AntialiasedTextRenderer.h"
#include "BlazeEngine/UI/Text/FontFace.h"

namespace  Blaze
{
	UIDebugLayer::UIDebugLayer()
		: UISystemToDebug(nullptr)
	{
		screen = Handle<UIDebugScreen>::Create(fontManager);
		UISystem.SetScreen(screen);	

		fontManager.AddFontFace("default", resourceManager.LoadResource<UI::FontFace>(UI::FontFace::LoadDefault()));
	}
	UIDebugLayer::~UIDebugLayer()
	{
		SetUISystemToDebug(nullptr);
	}
	void UIDebugLayer::SetupGraphics(const GraphicsAppRuntimeThreadData& data)
	{
		UISystem.InitializeGraphics(data.graphicsContext, data.rendererRegistry);		

		if (auto textRenderer = UISystem.GetGraphicsSubSystem().GetRendererRegistry().GetRenderer<Graphics::AntialiasedTextRenderer>())
			fontManager.CreateFontAtlas("default", { 32 }, *textRenderer, false);
		else
			BLAZE_LOG_WARNING("Failed to find AntialiasedTextRenderer for creating a font");	
	}
	void UIDebugLayer::SetUISystemToDebug(UI::System* newUISystemToDebug)
	{
		if (UISystemToDebug == newUISystemToDebug)
			return;

		if (UISystemToDebug != nullptr)
		{
			UISystemToDebug->windowChangedEventDispatcher.RemoveHandler<&UIDebugLayer::WindowChangedEvent>(*this);
			UISystemToDebug->destructionEventDispatcher.RemoveHandler<&UIDebugLayer::UISystemDestructionEvent>(*this);
			UISystem.SetWindow(nullptr);
		}

		UISystemToDebug = newUISystemToDebug;

		if (UISystemToDebug != nullptr)
		{
			UISystem.SetWindow(UISystemToDebug->GetWindow());
			UISystemToDebug->windowChangedEventDispatcher.AddHandler<&UIDebugLayer::WindowChangedEvent>(*this);
			UISystemToDebug->destructionEventDispatcher.AddHandler<&UIDebugLayer::UISystemDestructionEvent>(*this);
		}
	}
	void UIDebugLayer::Update()
	{
		screen->Update();

		if (UISystemToDebug != nullptr)
		{
			screen->SetPointerData(UISystemToDebug->GetInputSubSystem().GetPointerMap());
		}
		else
		{
			auto layer = GetLayer<UIDebuggableLayer>();
			SetUISystemToDebug(layer->GetUISystem());
		}
	}
	Input::EventProcessedState UIDebugLayer::OnEvent(const Input::GenericInputEvent& event, bool processed)
	{
		if (auto result = UISystem.ProcessInputEvent(event, processed))
			return result;

		return Input::EventProcessedState::NotProcessed;
	}
	void UIDebugLayer::Render(const Graphics::RenderContext& renderContext)
	{
		UISystem.Render(renderContext);
	}
	void UIDebugLayer::WindowChangedEvent(const UI::System::WindowChangedEvent& event)
	{
		UISystem.SetWindow(event.system.GetWindow());
	}
	void UIDebugLayer::UISystemDestructionEvent(const UI::System::DestructionEvent& event)
	{
		SetUISystemToDebug(nullptr);
	}
}