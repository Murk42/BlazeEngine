#include "pch.h"
#include "Threads/MainThread.h"

AppThread::AppThread()
	: GraphicsAppRuntimeThread({ graphicsContext, rendererRegistry, window })
{ 
}

void AppThread::Run()
{
	window.SetHiddenFlag(false);

	while (!exit)
	{
		Input::GenericInputEvent event;
		while (window.GetInputEvent(event))
			SendInputEventToLayers(event);

		ForEachLayer<AppLayer>([](auto& layer) {
				layer.Update();
			});

		Render();
	}
}
void AppThread::Render()
{
	auto& target = window.AcquireNextFramebuffer();
	target.ClearColor(Vec4u(0, 0, 0, 0));
	target.ClearDepth();

	Graphics::RenderContext_OpenGL renderContext{ target };

	ForEachLayer<GraphicsAppLayer>([&](GraphicsAppLayer& layer)
		{
			layer.Render(renderContext);
		});

	window.PresentFramebuffer(target);
}
inline void AppThread::WindowClosed(const Window::CloseEvent& event)
{
	exit = true;
}