#pragma once
#define GRAPHICS_OPENGL
#include "BlazeEngineGraphics/BlazeEngineGraphics.h"

#include "Renderers/TexturedRectRenderer_OpenGL.h"
#include "Renderers/PanelRenderer_OpenGL.h"
#include "Renderers/TextRenderer_OpenGL.h"
#include "Pipelines/MainRenderPipeline_OpenGL.h"
#include "Pipelines/UIRenderPipeline_OpenGL.h"

class RenderSystem_OpenGL
{	
	Graphics::OpenGL::GraphicsContext_OpenGL gc;
	Graphics::OpenGL::RenderWindow_OpenGL rw{ gc, "Marko", Vec2i(INT_MAX), Vec2u(640, 480) };

	Graphics::OpenGL::Line2DRenderer_OpenGL line2DRenderer;
	Graphics::OpenGL::TexturedRectRenderer_OpenGL texturedRectRenderer;
	Graphics::OpenGL::PanelRenderer_OpenGL panelRenderer;
	Graphics::OpenGL::TextRenderer_OpenGL textRenderer;

	MainRenderPipeline_OpenGL rp;
	Graphics::OpenGL::UIRenderPipeline_OpenGL UIRP;	

	Resource::ResourceManager& resourceManager;	
public:
	RenderSystem_OpenGL(Resource::ResourceManager& resourceManager);

	void SetActiveScreen(UI2::Screen* screen);

	WindowBase& GetWindow() { return rw.GetWindow(); }

	void Frame();
};