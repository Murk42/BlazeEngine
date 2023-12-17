#pragma once
#include "Graphics4/Dynamic/Core/GraphicsContext.h"
#include <SDL2/SDL.h>

namespace Graphics4::OpenGL
{	
	class GraphicsContext_OpenGL : public Dynamic::GraphicsContextBase
	{
	public:
		GraphicsContext_OpenGL();
		GraphicsContext_OpenGL(StringView implementationName);
		GraphicsContext_OpenGL(GraphicsContext_OpenGL&&) noexcept;
		~GraphicsContext_OpenGL() override;

		void Destroy();

		String GetImplementationName() const override;
		void WaitForIdle() const override;

		WindowSDL CreateWindow(String title, Vec2i pos, Vec2u size);
		void DestroyWindow(WindowSDL& window);

		inline SDL_GLContext GetContext() const { return context; }

		GraphicsContext_OpenGL& operator=(GraphicsContext_OpenGL&&) noexcept;
	private:
		SDL_GLContext context;

		WindowSDL initWindow;
		WindowSDL::WindowSDLHandle currentWindowHandle;
	};
}