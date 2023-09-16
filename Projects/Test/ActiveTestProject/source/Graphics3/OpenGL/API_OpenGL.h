#pragma once
#include "../API/API_dynamic.h"
#include "GL/glew.h"
#include "SDL/SDL.h"
#include <atomic>

namespace Graphics3
{	
	namespace OpenGL
	{		
		class GraphicsContext_OpenGL : public Dynamic::GraphicsContextBase
		{
		public:
			GraphicsContext_OpenGL();
			GraphicsContext_OpenGL(GraphicsContext_OpenGL&&) noexcept;
			~GraphicsContext_OpenGL() override;

			void WaitForIdle() const override;

			void Clear() override;

			SDL_Window* CreateWindow(String title, Vec2i size);
			void DestroyWindow(SDL_Window* window);

			inline SDL_GLContext GetContext() const { return context; }

			GraphicsContext_OpenGL& operator=(GraphicsContext_OpenGL&&) noexcept;
		private:
			SDL_GLContext context;
			SDL_Window* initWindow;
			bool initWindowTaken;
		};

		class Semaphore_OpenGL : public Dynamic::SemaphoreBase
		{
		public:
			Semaphore_OpenGL();
			void Clear() override;
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
			Dynamic::RenderWindowBase* GetRenderWindow() const override;
			operator Dynamic::FramebufferBase& ();

			RenderWindowFramebuffer_OpenGL& operator=(RenderWindowFramebuffer_OpenGL&&) noexcept;

		private:
			RenderWindow_OpenGL* renderWindow;

			friend class RenderWindow_OpenGL;
		};

		class RenderWindow_OpenGL : public Dynamic::RenderWindowBase, private EventHandler<Input::Events::WindowResizedEvent>
		{
		public:
			RenderWindow_OpenGL(GraphicsContext_OpenGL* graphicsContext, String title, Vec2i size);
			RenderWindow_OpenGL(RenderWindow_OpenGL&&) noexcept;
			~RenderWindow_OpenGL() override;

			void Clear() override;

			inline GraphicsContext_OpenGL* GetGraphicsContext() const { return graphicsContext; }
			inline Window* GetWindow() const override { return engineWindow; }
			inline Vec2i GetSize() const override { return size; }

			RenderWindowFramebuffer_OpenGL& AquireNextFramebuffer(Semaphore_OpenGL& semaphore);
			void PresentRenderWindow(RenderContext_OpenGL&, RenderWindowFramebuffer_OpenGL&, std::initializer_list<Semaphore_OpenGL*> semaphores);

			RenderWindow_OpenGL& operator=(RenderWindow_OpenGL&&) noexcept;
		private:					
			std::atomic_flag aquirable;

			RenderWindowFramebuffer_OpenGL framebuffer;
			GraphicsContext_OpenGL* graphicsContext;
			SDL_Window* window;	
			Window* engineWindow;
			Vec2i size;

			void OnEvent(Input::Events::WindowResizedEvent event) override;
		};

		GraphicsContext_OpenGL CreateGraphicsContext(ImplementationInfo* implementationInfo);
		Semaphore_OpenGL CreateSemaphore(GraphicsContext_OpenGL& graphicsContext);
		RenderContext_OpenGL CreateGraphicsRenderContext(GraphicsContext_OpenGL& graphicsContext);
		RenderContext_OpenGL CreatePresentRenderContext(GraphicsContext_OpenGL& graphicsContext);
		RenderWindow_OpenGL CreateRenderWindow(GraphicsContext_OpenGL& graphicsContext, String title, Vec2i size);
		RenderWindowFramebuffer_OpenGL& AquireNextFramebuffer(RenderWindow_OpenGL& renderWindow, Semaphore_OpenGL& signalSemaphore);
		void PresentRenderWindow(RenderContext_OpenGL& renderContext, RenderWindow_OpenGL& renderWindow, RenderWindowFramebuffer_OpenGL& framebuffer, std::initializer_list<Semaphore_OpenGL*> waitSemaphores);

		using GraphicsContext = GraphicsContext_OpenGL;
		using Semaphore = Semaphore_OpenGL;
		using RenderContext = RenderContext_OpenGL;
		using Framebuffer = Framebuffer_OpenGL;
		using RenderWindowFramebuffer = RenderWindowFramebuffer_OpenGL;
		using RenderWindow = RenderWindow_OpenGL;
	}

#if defined (GRAPHICS_OPENGL)
	using namespace OpenGL;
#endif
}