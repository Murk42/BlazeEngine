#include "pch.h"
#include "RenderSystem_OpenGL.h"

RenderSystem_OpenGL::RenderSystem_OpenGL(Resource::ResourceManager& resourceManager) :
	gc({
		.majorVersion = 4,
		.minorVersion = 6,
		}),
	resourceManager(resourceManager), line2DRenderer(gc), texturedRectRenderer(gc), rp(gc, rw), 
	panelRenderer(gc), textRenderer(gc), 
	UIRP(texturedRectRenderer, panelRenderer, textRenderer)	
{		
	Bitmap bm;
	bm.Load("assets/images/image.bmp", true);	

	Graphics::OpenGLWrapper::Texture2D texture;
	texture.Create(bm);
	texture.SetSettings({
		.min = Graphics::OpenGLWrapper::TextureSampling::Linear,
		.mag = Graphics::OpenGLWrapper::TextureSampling::Linear,
		.mip = Graphics::OpenGLWrapper::TextureSampling::Linear,
		.mipmaps = true,
		});		
	
	resourceManager.LoadTexture2DOpenGL("image", std::move(texture));	

	rw.GetWindow().resizedEventDispatcher.AddHandler(windowResizedEventHandler);

	windowResizedEventHandler.SetFunction([&](Input::Events::WindowResizedEvent event) {
		Blaze::Graphics::OpenGLWrapper::SetRenderArea({ }, rw.GetSize());
		});

	Graphics::OpenGLWrapper::EnableBlending(true);
//	Graphics::OpenGLWrapper::
}

void RenderSystem_OpenGL::SetScreen(UI::Screen* screen)
{
	UIRP.SetScreen(screen);
}

void RenderSystem_OpenGL::Frame()
{			
	rw.MakeActive();
	
	Blaze::Graphics::OpenGLWrapper::ClearTarget();

	rp.Execute();	
	 
	UIRP.Render(rw.GetSize());
		
	rw.GetWindow().SwapBuffers();	
}