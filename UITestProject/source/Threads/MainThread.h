#pragma once

class AppThread : public GraphicsAppRuntimeThread
{
public:
	AppThread();
private:
	bool exit = false;

	Graphics::GraphicsContext_OpenGL graphicsContext{ Graphics::GraphicsContextProperties_OpenGL {.contextFlags = Graphics::ContextFlags::Debug } };
	Graphics::RenderWindow_OpenGL window{ graphicsContext,  WindowCreateOptions{.hidden = true, .pos = {INT_MAX, INT_MAX}, .size = {1600, 1000} } };

	Graphics::PanelRenderer_OpenGL panelRenderer{ graphicsContext };
	Graphics::TexturedRectRenderer_OpenGL texturedRectRenderer{ graphicsContext };
	Graphics::AntialiasedTextRenderer_OpenGL antialiasedTextRenderer{ graphicsContext };
	Graphics::Quad2DRenderer_OpenGL quad2DRenderer{ graphicsContext };
	Graphics::RendererRegistry rendererRegistry{ panelRenderer, texturedRectRenderer, antialiasedTextRenderer, quad2DRenderer };

	void Run() override;

	void Render();

	void WindowClosed(const Window::CloseEvent& event);
	EVENT_MEMBER_FUNCTION(AppThread, WindowClosed, window.closeEventDispatcher)
};