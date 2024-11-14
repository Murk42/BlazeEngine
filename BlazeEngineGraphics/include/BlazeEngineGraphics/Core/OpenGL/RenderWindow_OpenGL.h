#pragma once
#include "BlazeEngineCore/BlazeEngineCore.h"
#include "BlazeEngine/Window/WindowSDL.h"
#include "BlazeEngine/Event/EventHandler.h"
#include "BlazeEngine/Input/InputEvents.h"
#include "BlazeEngineGraphics/BlazeEngineGraphicsDefines.h"
#include "RenderWindowFramebuffer_OpenGL.h"

namespace Blaze::Graphics::OpenGL
{
	class GraphicsContext_OpenGL;	
	class Semaphore_OpenGL;		

	struct WindowSDLCreateOptions_OpenGL
	{
		StringUTF8 title = StringUTF8();
		Vec2i pos = Vec2i(INT_MAX, INT_MAX);
		Vec2u size = Vec2u(1, 1);
		WindowSDLOpenMode openMode = WindowSDLOpenMode::Normal;
		WindowSDLStyleFlags styleFlags = WindowSDLStyleFlags::Resizable;
	};

	class BLAZE_GRAPHICS_API RenderWindow_OpenGL : private EventHandler<Input::Events::WindowResizedEvent>
	{
	public:
		/*Parity*/RenderWindow_OpenGL(GraphicsContext_OpenGL& graphicsContext, const WindowSDLCreateOptions_OpenGL& createOptions);
		/*Parity*/~RenderWindow_OpenGL();		

		/*Parity*/RenderWindowFramebuffer_OpenGL& AcquireNextFramebuffer(Semaphore_OpenGL* semaphore);
		/*Parity*/void PresentFramebuffer(RenderWindowFramebuffer_OpenGL& framebuffer, ArrayView<Semaphore_OpenGL*> waitSemaphores);

		/*Parity*/inline GraphicsContext_OpenGL& GetGraphicsContext() { return graphicsContext; }
		/*Parity*/inline WindowSDL& GetWindowSDL() { return window; }				
		/*Parity*/Vec2u GetSize() const;		
	private:
		std::atomic_flag acquirable;

		RenderWindowFramebuffer_OpenGL framebuffer;
		GraphicsContext_OpenGL& graphicsContext;
		WindowSDL window;		

		void OnEvent(Input::Events::WindowResizedEvent event) override;
	};
}