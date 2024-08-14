#include "pch.h"
#include "BlazeEngineGraphics/Core/OpenGL/SDLOpenGLContext_OpenGL.h"
#include "BlazeEngineGraphics/Core/OpenGL/GraphicsContext_OpenGL.h"

namespace Blaze::Graphics::OpenGL
{	
	GraphicsContext_OpenGL* activeGraphicsContext;

	static StringView GetSDLError()
	{
		const char* ptr = SDL_GetError();
		uintMem len = strlen(ptr);
		return StringView(ptr, len);
	}

	void SetActiveOpenGLGraphicsContext(GraphicsContext_OpenGL& graphicsContext)
	{		
		SDL_GLContext activeContext = SDL_GL_GetCurrentContext();		

		if (activeContext == NULL)
		{
			Debug::Logger::LogFatal("Blaze Engine Graphics", "SDL_GL_GetCurrentContext() returned NULL. SDL_Error() returned: \"" + GetSDLError() + "\"");
			return;
		}
		
		if (activeContext != (SDL_GLContext)graphicsContext.GetSDLOpenGLContext().handle)		
			SetActiveOpenGLGraphicsContextForced(graphicsContext);		
	}

	void SetActiveOpenGLGraphicsContextForced(GraphicsContext_OpenGL& graphicsContext)
	{		
		SDL_GLContext newContext = (SDL_GLContext)graphicsContext.GetSDLOpenGLContext().handle;		
		activeGraphicsContext = &graphicsContext;

		if (SDL_GL_MakeCurrent((SDL_Window*)graphicsContext.GetActiveWindowSDLHandle(), newContext) < 0)		
			Debug::Logger::LogFatal("Blaze Engine Graphics", "SDL_GL_MakeCurrent() returned a negative number. SDL_Error() returned: \"" + GetSDLError() + "\"");		
	}

	GraphicsContext_OpenGL* GetActiveOpenGLGraphicsContext()
	{
		if (activeGraphicsContext == nullptr)
			return nullptr;

		auto activeContext = SDL_GL_GetCurrentContext();

		if (activeContext == NULL)
		{
			Debug::Logger::LogFatal("Blaze Engine Graphics", "SDL_GL_GetCurrentContext() returned NULL. SDL_Error() returned: \"" + GetSDLError() + "\"");
			return nullptr;
		}
		
		if (activeGraphicsContext->GetSDLOpenGLContext().handle == activeContext)
			return activeGraphicsContext;
		else
		{
			activeGraphicsContext = nullptr;
			return nullptr;
		}
	}
	void UnsetActiveOpenGLGraphicsContext(GraphicsContext_OpenGL& graphicsContext)
	{
		if (activeGraphicsContext == &graphicsContext)
			activeGraphicsContext = nullptr;
	}
}