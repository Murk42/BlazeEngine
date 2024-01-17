#pragma once


class MainRenderPipeline_OpenGL
{	
public:
	MainRenderPipeline_OpenGL(
		Blaze::Graphics::OpenGL::GraphicsContext_OpenGL& graphicsContext, 
		Blaze::Graphics::OpenGL::RenderWindow_OpenGL& renderWindow		
	);

	void Execute();
private:
	Blaze::Graphics::OpenGL::GraphicsContext_OpenGL& graphicsContext;
	Blaze::Graphics::OpenGL::RenderWindow_OpenGL& renderWindow;	
};