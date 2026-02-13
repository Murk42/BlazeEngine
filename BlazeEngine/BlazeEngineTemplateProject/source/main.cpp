#include "pch.h"

class MainAppRuntimeThread : public Blaze::AppRuntimeThread
{
public:
	Graphics::OpenGL::GraphicsContext_OpenGL graphicsContext{ { } };
	Graphics::OpenGL::RenderWindow_OpenGL window{ graphicsContext, WindowCreateOptions{.hidden = true }, Graphics::OpenGL::RenderWindowOptions_OpenGL{ } };

	MainAppRuntimeThread() 
		: AppRuntimeThread(window ) 
	{
	}

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

CLIENT_API void Setup()
{
	Debug::Logger::AddOutputFile("log.txt");

	ResourceManager resourceManager;

	App app;

	app.RegisterRuntimeThread(AppRuntimeThreadCreationData::New<MainAppRuntimeThread>());

	while (app.Update())
	{
		Input::Update();
	}
}