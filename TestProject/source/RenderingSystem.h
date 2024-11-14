#pragma once

class RenderingSystem
{
public:
	RenderingSystem();

	inline Window& GetWindow() { return renderWindow.GetWindowSDL(); }

	void Render();

	void SetViewMatrix(Mat4f matrix);
private:
	Graphics::OpenGL::GraphicsContext_OpenGL graphicsContext;
	Graphics::OpenGL::RenderWindow_OpenGL renderWindow;
	Graphics::OpenGL::Line3DRenderer_OpenGL line3DRenderer;

	Mat4f projectionMatrix;
	Mat4f viewMatrix;

	LambdaEventHandler<Input::Events::WindowResizedEvent> windowResizedEventHandler;
};