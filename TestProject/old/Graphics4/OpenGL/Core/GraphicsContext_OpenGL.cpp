#include "pch.h"
#include "GraphicsContext_OpenGL.h"
#include "GL/glew.h"

namespace Graphics4::OpenGL
{
	bool glewInitialized = false;

	static constexpr uint openGLMajorVersion = 4;
	static constexpr uint openGLMinorVersion = 5;
	static constexpr uint openGLProfile = 0; //0 - Core, 1 - Compatibility

	static constexpr uint depthBufferBitCount = 16;

	//Setting this to 0 results in a opengl error !
	static constexpr uint stencilBufferBitCount = 8;

	GraphicsContext_OpenGL::GraphicsContext_OpenGL()
		: currentWindowHandle(nullptr)
	{
		SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);

		SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS,
			SDL_GL_CONTEXT_DEBUG_FLAG |
			SDL_GL_CONTEXT_ROBUST_ACCESS_FLAG
		);

		SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, openGLMajorVersion);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, openGLMinorVersion);

		SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, depthBufferBitCount);

		initWindow = WindowSDL("", Vec2i(INT_MAX), Vec2u(640, 480), WindowHandleGraphicsaAPI::OpenGL);

		context = SDL_GL_CreateContext((SDL_Window*)initWindow.GetHandle());

		if (!glewInitialized)
		{
			glewInit();
			glewInitialized = true;
		}

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glClearColor(0, 0, 0, 1);

	}
	GraphicsContext_OpenGL::GraphicsContext_OpenGL(StringView implementationName)
		: GraphicsContext_OpenGL()
	{
	}
	GraphicsContext_OpenGL::GraphicsContext_OpenGL(GraphicsContext_OpenGL&& other) noexcept
		: GraphicsContextBase(std::move(other))
	{
		initWindow = std::move(other.initWindow);
		context = other.context;

		other.context = NULL;
	}
	GraphicsContext_OpenGL::~GraphicsContext_OpenGL()
	{
		Destroy();
	}

	void GraphicsContext_OpenGL::Destroy()
	{
		SDL_GL_DeleteContext(context);

		initWindow.Destroy();
		context = NULL;
	}

	String GraphicsContext_OpenGL::GetImplementationName() const
	{
		return "OpenGL";
	}

	void GraphicsContext_OpenGL::WaitForIdle() const
	{
		glFlush();
	}

	WindowSDL GraphicsContext_OpenGL::CreateWindow(String title, Vec2i pos, Vec2u size)
	{
		if (initWindow.IsNullWindow())
			return WindowSDL(title, pos, size, WindowHandleGraphicsaAPI::OpenGL);
		else
			return std::move(initWindow);
	}

	void GraphicsContext_OpenGL::DestroyWindow(WindowSDL& window)
	{
		WindowSDL::WindowSDLHandle currentWindowHandle = SDL_GL_GetCurrentWindow();

		if (currentWindowHandle == window.GetHandle() && initWindow.IsNullWindow())
			initWindow = std::move(window);
		else
			window.Destroy();
	}

	GraphicsContext_OpenGL& GraphicsContext_OpenGL::operator=(GraphicsContext_OpenGL&& other) noexcept
	{
		Destroy();

		GraphicsContextBase::operator=(std::move(other));

		initWindow = std::move(other.initWindow);
		context = other.context;

		other.context = NULL;

		return *this;
	}
}