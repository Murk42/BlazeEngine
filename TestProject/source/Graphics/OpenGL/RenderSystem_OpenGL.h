#pragma once
#define GRAPHICS_OPENGL

#include "Resources/ResourceManager.h"
#include "Pipelines/MainRenderPipeline_OpenGL.h"

class RenderSystem_OpenGL
{	
	Graphics::OpenGL::GraphicsContext_OpenGL gc;
	Graphics::OpenGL::RenderWindow_OpenGL rw{ gc, { .title = "Marko", .size = Vec2u(640, 480) } };

	Graphics::OpenGL::Line2DRenderer_OpenGL line2DRenderer;
	Graphics::OpenGL::TexturedRectRenderer_OpenGL texturedRectRenderer;
	Graphics::OpenGL::PanelRenderer_OpenGL panelRenderer;
	Graphics::OpenGL::TextRenderer_OpenGL textRenderer;

	MainRenderPipeline_OpenGL rp;
	Graphics::OpenGL::UIRenderPipeline_OpenGL UIRP;		

	Resource::ResourceManager& resourceManager;	
	LambdaEventHandler<Input::Events::WindowResizedEvent> windowResizedEventHandler;
public:
	RenderSystem_OpenGL(Resource::ResourceManager& resourceManager);

	void SetScreen(UI::Screen* screen);

	WindowBase& GetWindow() { return rw.GetWindow(); }

	void Frame();
};