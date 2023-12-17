#include "pch.h"
#include "RenderSystem_OpenGL.h"

RenderSystem_OpenGL::RenderSystem_OpenGL(Resource::ResourceManager& resourceManager) :
	resourceManager(resourceManager), line2DRenderer(gc), texturedRectRenderer(gc), rp(gc, rw), 
	panelRenderer(gc), textRenderer(gc),
	UIRP(texturedRectRenderer, panelRenderer, textRenderer)
{		
	Bitmap bm;
	bm.Load("D:/Programming/Projects/BlazeEngine/TestProject/assets/images/image.bmp", true);	

	Graphics::OpenGLWrapper::Texture2D texture;
	texture.Create(bm);
	texture.SetSettings({
		.min = Graphics::OpenGLWrapper::TextureSampling::Linear,
		.mag = Graphics::OpenGLWrapper::TextureSampling::Linear,
		.mip = Graphics::OpenGLWrapper::TextureSampling::Linear,
		.mipmaps = true,
		});	

	Font font{ "assets/fonts/Consola.ttf" };	

	resourceManager.LoadFont("default", std::move(font));	
	resourceManager.LoadTexture2DOpenGL("image", std::move(texture));
}

void RenderSystem_OpenGL::SetActiveScreen(UI2::Screen* screen)
{
	UIRP.SetScreen(screen);
}


void RenderSystem_OpenGL::Frame()
{		
	rw.MakeActive();

	Blaze::Graphics::OpenGLWrapper::ClearTarget();
	Blaze::Graphics::OpenGLWrapper::SetRenderArea({ }, rw.GetSize());

	rp.Execute();
	 
	UIRP.Render(rw.GetSize());
	
	SDL_GL_SwapWindow((SDL_Window*)rw.GetWindow().GetHandle());
}