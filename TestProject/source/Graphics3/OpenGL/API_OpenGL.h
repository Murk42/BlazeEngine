#pragma once
#include "../API/API_dynamic.h"
#include "GL/glew.h"
#include "SDL2/SDL.h"
#include <atomic>

namespace Graphics3
{	
	namespace OpenGL
	{		
		class RenderWindow_OpenGL;

		class GraphicsContext_OpenGL : public Dynamic::GraphicsContextBase
		{
		public:
			GraphicsContext_OpenGL();
			GraphicsContext_OpenGL(ImplementationInfo*);
			GraphicsContext_OpenGL(GraphicsContext_OpenGL&&) noexcept;
			~GraphicsContext_OpenGL() override;

			void Destroy();

			String GetImplementationName() const override;
			void WaitForIdle() const override;

			WindowSDL CreateWindow(String title, Vec2i pos, Vec2i size);
			void DestroyWindow(WindowSDL& window);			

			inline SDL_GLContext GetContext() const { return context; }

			GraphicsContext_OpenGL& operator=(GraphicsContext_OpenGL&&) noexcept;
		private:
			SDL_GLContext context;

			WindowSDL initWindow;
			WindowSDL::WindowSDLHandle currentWindowHandle;
		};
		class Semaphore_OpenGL : public Dynamic::SemaphoreBase
		{
		public:			
			Semaphore_OpenGL(GraphicsContext_OpenGL& graphicsContext);
		};

		class RenderContext_OpenGL : public Dynamic::RenderContextBase
		{
		public:
			RenderContext_OpenGL(GraphicsContext_OpenGL*);
			RenderContext_OpenGL(RenderContext_OpenGL&&) noexcept;
			~RenderContext_OpenGL() override;

			void WaitForIdle() const override;			

			void Clear() override;		

			inline GraphicsContext_OpenGL* GetGraphicsContext() const { return graphicsContext; }

			RenderContext_OpenGL& operator=(RenderContext_OpenGL&&) noexcept;
		private:
			GraphicsContext_OpenGL* graphicsContext;
		};

		class Framebuffer_OpenGL : public Dynamic::FramebufferBase
		{
		public:
			Framebuffer_OpenGL();
			Framebuffer_OpenGL(Framebuffer_OpenGL&&) noexcept;
			~Framebuffer_OpenGL() override;

			void Clear() override;

			inline uint GetID() const { return id; };
			inline Vec2i GetSize() const override { return size; }

			Framebuffer_OpenGL& operator=(Framebuffer_OpenGL&&) noexcept;
		private:
			uint id;			
			Vec2i size;

			friend class RenderWindow_OpenGL;
			friend class RenderWindowFramebuffer_OpenGL;
		};

		class RenderWindowFramebuffer_OpenGL : public Dynamic::RenderWindowFramebufferBase, public Framebuffer_OpenGL
		{
		public:
			RenderWindowFramebuffer_OpenGL();
			RenderWindowFramebuffer_OpenGL(RenderWindowFramebuffer_OpenGL&&) noexcept;
			~RenderWindowFramebuffer_OpenGL() override;

			void Clear() override;
			Vec2i GetSize() const override;
			inline OpenGL::RenderWindow_OpenGL* GetOpenGLRenderWindow() const { return renderWindow; }

			operator Dynamic::FramebufferBase& ();

			RenderWindowFramebuffer_OpenGL& operator=(RenderWindowFramebuffer_OpenGL&&) noexcept;

		private:
			RenderWindow_OpenGL* renderWindow;

			Dynamic::RenderWindowBase* GetRenderWindow() const override;

			friend class RenderWindow_OpenGL;
		};

		class RenderWindow_OpenGL : public Dynamic::RenderWindowBase, private EventHandler<Input::Events::WindowResizedEvent>
		{
		public:
			RenderWindow_OpenGL(GraphicsContext_OpenGL& graphicsContext, StringView title, Vec2i pos, Vec2i size);
			RenderWindow_OpenGL(RenderWindow_OpenGL&&) noexcept;
			~RenderWindow_OpenGL() override;

			void Destroy();

			inline GraphicsContext_OpenGL* GetGraphicsContext() const { return graphicsContext; }			
			inline WindowSDL* GetWindowSDL() { return &window; }
			inline Vec2i GetSize() const override { return size; }

			Dynamic::RenderWindowFramebufferBase& AcquireNextFramebuffer(Dynamic::SemaphoreBase& semaphore) override;
			RenderWindowFramebuffer_OpenGL& AcquireNextFramebuffer(Semaphore_OpenGL& semaphore);

			void SwapWindowFramebuffers();

			RenderWindow_OpenGL& operator=(RenderWindow_OpenGL&&) noexcept;
		private:					
			inline WindowBase* GetWindow() const override { return &(WindowBase&)window; }

			std::atomic_flag acquirable;

			RenderWindowFramebuffer_OpenGL framebuffer;
			GraphicsContext_OpenGL* graphicsContext;
			WindowSDL window;			
			Vec2i size;

			void OnEvent(Input::Events::WindowResizedEvent event) override;
		};		
	}

#if defined (GRAPHICS_OPENGL)
	using namespace OpenGL;
#endif
}