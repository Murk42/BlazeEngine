#pragma once
#include "Graphics4/Dynamic/Core/RenderWindow.h"
#include "RenderWindowFramebuffer_OpenGL.h"

namespace Graphics4::OpenGL
{
	class GraphicsContext_OpenGL;	
	class Semaphore_OpenGL;

	class RenderWindow_OpenGL : public Dynamic::RenderWindowBase, private EventHandler<Input::Events::WindowResizedEvent>
	{
	public:
		RenderWindow_OpenGL(GraphicsContext_OpenGL& graphicsContext, StringView title, Vec2i pos, Vec2u size);
		RenderWindow_OpenGL(RenderWindow_OpenGL&&) noexcept;
		~RenderWindow_OpenGL() override;

		void Destroy();

		inline GraphicsContext_OpenGL* GetGraphicsContext() const { return graphicsContext; }

		inline bool GetWindow(WindowSDL*& window) const { window = (WindowSDL*)&this->window; return true; }
		inline bool GetWindow(WindowBase*& window) const override { window = &(WindowBase&)this->window; return true; }
		inline Vec2u GetSize() const override { return size; }

		bool AcquireNextFramebuffer(Dynamic::RenderWindowFramebufferBase*& framebuffer, Dynamic::SemaphoreBase& semaphore) override;
		bool AcquireNextFramebuffer(RenderWindowFramebuffer_OpenGL*& framebuffer, Semaphore_OpenGL& semaphore);
		void PresentFramebuffer(Dynamic::RenderWindowFramebufferBase& framebuffer, Array<Dynamic::SemaphoreBase*> waitSemaphores);
		void PresentFramebuffer(RenderWindowFramebuffer_OpenGL& framebuffer, Array<Semaphore_OpenGL*> waitSemaphores);

		RenderWindow_OpenGL& operator=(RenderWindow_OpenGL&&) noexcept;
	private:

		std::atomic_flag acquirable;

		RenderWindowFramebuffer_OpenGL framebuffer;
		GraphicsContext_OpenGL* graphicsContext;
		WindowSDL window;
		Vec2u size;

		void OnEvent(Input::Events::WindowResizedEvent event) override;
	};
}