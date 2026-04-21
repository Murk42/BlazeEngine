#pragma once

class MainThread : public AppGraphicsRuntimeThread
{
public:
	Graphics::OpenGL::GraphicsContext_OpenGL graphicsContext{ { } };
	Graphics::OpenGL::RenderWindow_OpenGL window{ graphicsContext, WindowCreateOptions{.hidden = true }, Graphics::OpenGL::RenderWindowOptions_OpenGL{ } };

	MainThread();

	void Run() override;

	void WindowClosed(const Window::CloseEvent& event);
	EVENT_MEMBER_FUNCTION(MainThread, WindowClosed, window.closeEventDispatcher)
private:
	bool exit = false;
};
