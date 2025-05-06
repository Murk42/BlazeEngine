#include "pch.h"
#include "RenderingSystem.h"

RenderingSystem::RenderingSystem() :
	graphicsContext(Graphics::OpenGL::GraphicsContextProperties_OpenGL{
		}),
	renderWindow(graphicsContext, Graphics::OpenGL::WindowCreateOptions_OpenGL{
		.title = "Title",
		.pos = Vec2i(INT_MAX),
		.size = Vec2u(1000, 500),
		.openMode = WindowOpenMode::Normal,
		.styleFlags = WindowStyleFlags::Resizable,		
	}),
	line3DRenderer(graphicsContext)
{	
	windowResizedEventHandler.SetFunction([&](Window::WindowResizedEvent event) {
		graphicsContext.SetRenderArea(Vec2i(), event.size);
		projectionMatrix = Mat4f::PerspectiveMatrix(Math::PI / 2, (float)event.size.x / event.size.y, 0.1, 1000);
		line3DRenderer.SetProjectionMatrix(projectionMatrix);		
		});
	renderWindow.GetWindow().resizedEventDispatcher.AddHandler(windowResizedEventHandler);
	
	graphicsContext.SetClearColor(0x100000ff);
	graphicsContext.EnableDepthTest(true);
	graphicsContext.EnableBlending(true); 		
}

void RenderingSystem::Render()
{
	graphicsContext.ClearTarget();
	auto& framebuffer = renderWindow.AcquireNextFramebuffer(nullptr);

	class Line3DRenderStream : public Graphics::RenderStream
	{
	public:
		uint index;
		Graphics::OpenGL::Line3DRenderData_OpenGL renderData[6]{
			Graphics::OpenGL::Line3DRenderData_OpenGL{ .color = 0xffffffff, .p1 = Vec3f(-0.5, 0, 3), .p2 = Vec3f(0.5, 0, 3), .width = 10.0f },
			Graphics::OpenGL::Line3DRenderData_OpenGL{.color = 0xffffffff, .p1 = Vec3f(-0.5, 0, 3), .p2 = Vec3f(0, 0, 3.5), .width = 10.0f },
			Graphics::OpenGL::Line3DRenderData_OpenGL{.color = 0xffffffff, .p1 = Vec3f(0.5, 0, 3), .p2 = Vec3f(0, 0, 3.5), .width = 10.0f },
			Graphics::OpenGL::Line3DRenderData_OpenGL{.color = 0xffffffff, .p1 = Vec3f(-0.5, 0, 3), .p2 = Vec3f(0.0, 0.5, 3.25), .width = 10.0f },
			Graphics::OpenGL::Line3DRenderData_OpenGL{.color = 0xffffffff, .p1 = Vec3f(0.5, 0, 3), .p2 = Vec3f(0.0, 0.5, 3.25), .width = 10.0f },
			Graphics::OpenGL::Line3DRenderData_OpenGL{.color = 0xffffffff, .p1 = Vec3f(0.0, 0, 3.5), .p2 = Vec3f(0.0, 0.5, 3.25), .width = 10.0f }
		};

		void BeginStream() override
		{
			index = 0;
		}
		void* Advance() override
		{
			if (index >= _countof(renderData))
				return nullptr;

			return renderData + index++;
		}
	} renderStream;
	line3DRenderer.Render(renderStream, framebuffer.GetFramebuffer().GetSize());
	
	renderWindow.PresentFramebuffer(framebuffer, { });
}

void RenderingSystem::SetViewMatrix(Mat4f matrix)
{
	this->viewMatrix = matrix;
	line3DRenderer.SetViewMatrix(matrix);
}
