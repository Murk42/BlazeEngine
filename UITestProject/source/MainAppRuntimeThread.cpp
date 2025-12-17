#include "pch.h"
#include "MainAppRuntimeThread.h"
#include "MainScreen.h"
#include "UIDebugLayer.h"

class MainAppRuntimeThread : public Blaze::AppRuntimeThread
{
public:
	App& app;
	ResourceManager resourceManager;

	Graphics::GraphicsContext_OpenGL graphicsContext{ { } };
	Graphics::RenderWindow_OpenGL window{ graphicsContext, WindowCreateOptions{ .hidden = true }, Graphics::RenderWindowOptions_OpenGL{ } };

	Graphics::PanelRenderer_OpenGL panelRenderer;
	Graphics::TexturedRectRenderer_OpenGL texturedRectRenderer;
	Graphics::RendererRegistry rendererRegistry{ panelRenderer, texturedRectRenderer };

	ResourceRef<UI::FontFace> fontFace = resourceManager.LoadResource<UI::FontFace>("font face", UI::FontFace::LoadDefault());;
	ResourceRef<UI::FontAtlas> fontAtlas0 = resourceManager.LoadResource<UI::FontAtlas>("fontAtlas32", *fontFace, 32.0f, fontFace->GetAllGlyphsInidices(), UI::FontGlyphRenderers::AntialiasedFontGlyphRenderer(), resourceManager);
	ResourceRef<UI::FontAtlas> fontAtlas1 = resourceManager.LoadResource<UI::FontAtlas>("fontAtlas20", *fontFace, 20.0f, fontFace->GetAllGlyphsInidices(), UI::FontGlyphRenderers::AntialiasedFontGlyphRenderer(), resourceManager);

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

	UI::System UISystem{ runtimeThread.rendererRegistry, runtimeThread.GetWindow()};

	MainLayer(MainAppRuntimeThread& runtimeThread)
		: runtimeThread(runtimeThread)
	{
		UISystem.SetScreen<MainScreen>(runtimeThread.resourceManager);

		runtimeThread.AddLayer<ClientUIDebugLayer>(&UISystem);
	}
	void OnAttach() override 
	{
	}
	void OnDetach() override 
	{
	}
	void Update() override 
	{
	}
	bool OnEvent(const Input::GenericInputEvent& event, bool isProcessed)
	{
		return UISystem.ProcessInputEvent(event, isProcessed);
	}
	void Render() override 
	{
		UISystem.Render();
	}
};

MainAppRuntimeThread::MainAppRuntimeThread(App& app)
	: AppRuntimeThread(window), app(app), panelRenderer(graphicsContext), texturedRectRenderer(graphicsContext)
{
	AddLayer<MainLayer>(*this);
}
AppRuntimeThreadCreationData GetMainRuntimeThreadCreationData(App& app)
{
	return AppRuntimeThreadCreationData().SetRuntimeThreadData<MainAppRuntimeThread>(app);
}