#include "pch.h"
#include "MainAppRuntimeThread.h"

MainThread::MainThread()
{
}

void MainThread::Run()
{	
	window.SetHiddenFlag(false);

	while (!exit)
	{
		Input::GenericInputEvent event;
		while (window.ProcessInputEvent(event))
			SendInputEventToLayers(event);

		UpdateLayers();

		auto& target = window.AcquireNextFramebuffer();
		target.ClearColor(Vec4u(0, 0, 0, 0));
		target.ClearDepth();

		RenderLayers(target);

		window.PresentFramebuffer(target);
	}
}

void MainThread::WindowClosed(const Window::CloseEvent& event)
{
	exit = true;
}
