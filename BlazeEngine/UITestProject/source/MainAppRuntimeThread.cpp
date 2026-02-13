#include "pch.h"
#include "MainAppRuntimeThread.h"
#include "MainScreen.h"
#include "UIDebugLayer.h"

class MainAppRuntimeThread : public Blaze::AppRuntimeThread
{
public:
	App& app;
	ResourceManager resourceManager;

	Graphics::GraphicsContext_OpenGL graphicsContext{ Graphics::GraphicsContextProperties_OpenGL { .contextFlags = Graphics::ContextFlags::Debug } };
	Graphics::RenderWindow_OpenGL window{ graphicsContext, WindowCreateOptions{.hidden = true, .pos = {INT_MAX, INT_MAX}, .size = {1600, 1000} }, Graphics::RenderWindowOptions_OpenGL{} };

	Graphics::PanelRenderer_OpenGL panelRenderer{ graphicsContext };
	Graphics::TexturedRectRenderer_OpenGL texturedRectRenderer{ graphicsContext };
	Graphics::AntialiasedTextRenderer_OpenGL antialiasedTextRenderer{ graphicsContext};
	Graphics::RendererRegistry rendererRegistry{ panelRenderer, texturedRectRenderer, antialiasedTextRenderer };

	ResourceRef<UI::FontManager> fontManager { resourceManager, "fontManager", resourceManager};

	MainAppRuntimeThread(App& app);

	void StartRender() override
	{
		window.ClearRenderBuffers();
	}
	void EndRender() override
	{
		window.Present();

		if (window.GetHiddenFlag())
			window.SetHiddenFlag(false);
	}

	void WindowClosed(const Window::CloseEvent& event)
	{
		MarkForExit();
	}
	EVENT_MEMBER_FUNCTION(MainAppRuntimeThread, WindowClosed, window.closeEventDispatcher)
};

class MainLayer : public AppLayer
{
public:
	MainAppRuntimeThread& runtimeThread;

	UI::System UISystem{ runtimeThread.graphicsContext, runtimeThread.rendererRegistry, runtimeThread.GetWindow() };

	Stopwatch sw1, sw2;

	MainLayer(MainAppRuntimeThread& runtimeThread)
		: runtimeThread(runtimeThread)
	{
		UISystem.SetScreen<MainScreen>(runtimeThread.resourceManager);

		//runtimeThread.AddLayer<ClientUIDebugLayer>(&UISystem);
	}
	void OnAttach() override
	{
	}
	void OnDetach() override
	{
	}
	void Update() override
	{
		UISystem.GetScreen()->Update();

		//auto transform = static_cast<MainScreen*>(UISystem.GetScreen())->blinkingLabel.GetTransform();
		//transform.rotation = sw2.GetTime() * 2;
		//static_cast<MainScreen*>(UISystem.GetScreen())->blinkingLabel.SetTransform(transform);
		//if (sw1.GetTime() > 1.0f)
		//{
		//
		//	static_cast<MainScreen*>(UISystem.GetScreen())->blinkingLabel.SetTransform(transform);
		//	if (static_cast<MainScreen*>(UISystem.GetScreen())->blinkingLabel.IsEnabled())
		//		static_cast<MainScreen*>(UISystem.GetScreen())->blinkingLabel.Disable();
		//	else
		//		static_cast<MainScreen*>(UISystem.GetScreen())->blinkingLabel.Enable();
		//
		//	sw1.Reset();
		//}
	}
	Input::EventProcessedState OnEvent(const Input::GenericInputEvent& event, bool processed) override
	{
		if (auto result = UISystem.ProcessInputEvent(event, processed))
			return result;

		if (event.TryProcess([&](const Input::KeyDownEvent& event)
			{

				if (event.key == Input::Key::F11)
					if (runtimeThread.window.GetWindowPresentMode() == WindowPresentMode::Fullscreen)
						runtimeThread.window.SetWindowPresentMode(WindowPresentMode::Normal);
					else
						runtimeThread.window.SetWindowPresentMode(WindowPresentMode::Fullscreen);

			}))
			return Input::EventProcessedState::Consumed;

		return { };
	}
	void Render() override 
	{
		UISystem.Render();
	}
};

MainAppRuntimeThread::MainAppRuntimeThread(App& app)
	: AppRuntimeThread(window), app(app)
{
	AddLayer<MainLayer>(*this);

	fontManager->AddFontFace("default", resourceManager.LoadResource<UI::FontFace>("default", UI::FontFace::LoadDefault()));
	fontManager->CreateFontAtlas("default", { 9.0f, 20.0f, 64.0f, 128.0f }, antialiasedTextRenderer);
}
AppRuntimeThreadCreationData GetMainRuntimeThreadCreationData(App& app)
{
	return AppRuntimeThreadCreationData::New<MainAppRuntimeThread>(app);
}