#include "pch.h"
#include "UIDebugLayer.h"
#include "UIDebugScreen.h"

UIDebugLayer::UIDebugLayer(Graphics::GraphicsContextBase& graphicsContext, UI::System* UISystemToDebug)
	: UISystemToDebug(nullptr), UISystem(graphicsContext)
{
	fontManager->AddFontFace("default", resourceManager.LoadResource<UI::FontFace>("default", UI::FontFace::LoadDefault()));
	if (auto textRenderer = UISystemToDebug->GetGraphicsSubSystem().GetRendererRegistry().GetRenderer<Graphics::AntialiasedTextRenderer>())
	{
		fontManager->CreateFontAtlas("default", { 32.0f, 20.0f }, *textRenderer);
	}
	else
		BLAZE_LOG_WARNING("Failed to find AntialiasedTextRenderer in the UI system that is being debugged");

	UISystem.SetScreen<UIDebugScreen>(resourceManager);
	SetUISystemToDebug(UISystemToDebug);
}
UIDebugLayer::~UIDebugLayer()
{
	SetUISystemToDebug(nullptr);
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
		UISystem.SetRendererRegistry({});
	}

	UISystemToDebug = newUISystemToDebug;

	if (UISystemToDebug != nullptr)
	{
		UISystem.SetWindow(UISystemToDebug->GetWindow());
		UISystem.SetRendererRegistry(UISystemToDebug->GetGraphicsSubSystem().GetRendererRegistry());
		UISystemToDebug->windowChangedEventDispatcher.AddHandler<&UIDebugLayer::WindowChangedEvent>(*this);
		UISystemToDebug->destructionEventDispatcher.AddHandler<&UIDebugLayer::UISystemDestructionEvent>(*this);
	}
}
void UIDebugLayer::Update()
{
	UISystem.GetScreen()->Update();

	if (UISystemToDebug != nullptr)
	{
		static_cast<UIDebugScreen*>(UISystem.GetScreen())->SetPointerData(UISystemToDebug->GetInputSubSystem().GetPointerMap());
	}
}
Input::EventProcessedState UIDebugLayer::OnEvent(const Input::GenericInputEvent& event, bool processed)
{
	if (auto result = UISystem.ProcessInputEvent(event, processed))
		return result;

	return Input::EventProcessedState::NotProcessed;
}
void UIDebugLayer::Render()
{
	UISystem.Render();
}
void UIDebugLayer::WindowChangedEvent(const UI::System::WindowChangedEvent& event)
{
	UISystem.SetWindow(event.system.GetWindow());
}
void UIDebugLayer::UISystemDestructionEvent(const UI::System::DestructionEvent& event)
{
	SetUISystemToDebug(nullptr);
}